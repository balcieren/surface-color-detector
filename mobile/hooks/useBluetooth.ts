import { useCallback, useEffect, useRef, useState } from "react";
import {
  Alert,
  Linking,
  NativeEventEmitter,
  NativeModules,
  PermissionsAndroid,
  Platform,
} from "react-native";

// Check if BLE Manager is available
let BleManager: any = null;
let BleManagerModule: any = null;
let bleManagerEmitter: NativeEventEmitter | null = null;
let isBleAvailable = false;

try {
  BleManager = require("react-native-ble-manager").default;
  BleManagerModule = NativeModules.BleManager;
  bleManagerEmitter = new NativeEventEmitter(BleManagerModule);
  isBleAvailable = true;
} catch (e) {
  console.log("BLE Manager not available - running in mock mode");
}

// ESP32 BLE Service and Characteristic UUIDs
const SERVICE_UUID = "4fafc201-1fb5-459e-8fcc-c5c9c331914b";
const CHARACTERISTIC_UUID = "beb5483e-36e1-4688-b7f5-ea07361b26a8";
const DEVICE_NAME = "Surface Color Detector";

interface RGBColor {
  r: number;
  g: number;
  b: number;
}

interface Peripheral {
  id: string;
  name?: string;
  localName?: string;
  rssi?: number;
  advertising?: any;
}

interface BluetoothState {
  isScanning: boolean;
  isConnected: boolean;
  isConnecting: boolean;
  isMockMode: boolean;
  peripherals: Map<string, Peripheral>;
  connectedPeripheralId: string | null;
  currentColor: RGBColor | null;
  error: string | null;
}

export function useBluetooth() {
  const mockIntervalRef = useRef<ReturnType<typeof setInterval> | null>(null);
  const listenersRef = useRef<any[]>([]);

  const [state, setState] = useState<BluetoothState>({
    isScanning: false,
    isConnected: false,
    isConnecting: false,
    isMockMode: !isBleAvailable,
    peripherals: new Map(),
    connectedPeripheralId: null,
    currentColor: null,
    error: null,
  });

  // Initialize BLE Manager
  useEffect(() => {
    if (!isBleAvailable || !BleManager) {
      console.log("Running in mock mode - BLE not available");
      return;
    }

    BleManager.start({ showAlert: false })
      .then(() => console.log("BleManager started"))
      .catch((error: any) => {
        console.error("BleManager start error:", error);
        setState((prev) => ({ ...prev, isMockMode: true }));
      });

    // Set up event listeners
    if (bleManagerEmitter) {
      const listeners = [
        bleManagerEmitter.addListener(
          "BleManagerDiscoverPeripheral",
          handleDiscoverPeripheral
        ),
        bleManagerEmitter.addListener("BleManagerStopScan", handleStopScan),
        bleManagerEmitter.addListener(
          "BleManagerConnectPeripheral",
          handleConnectPeripheral
        ),
        bleManagerEmitter.addListener(
          "BleManagerDisconnectPeripheral",
          handleDisconnectPeripheral
        ),
        bleManagerEmitter.addListener(
          "BleManagerDidUpdateValueForCharacteristic",
          handleUpdateValue
        ),
      ];
      listenersRef.current = listeners;
    }

    // Request permissions on Android
    if (Platform.OS === "android") {
      requestAndroidPermissions();
    }

    return () => {
      // Clean up listeners
      listenersRef.current.forEach((listener) => listener.remove());
      if (mockIntervalRef.current) {
        clearInterval(mockIntervalRef.current);
      }
    };
  }, []);

  // Request Android permissions
  const requestAndroidPermissions = async () => {
    if (Platform.OS !== "android") return true;

    const apiLevel = Platform.Version;

    if (apiLevel >= 31) {
      const result = await PermissionsAndroid.requestMultiple([
        PermissionsAndroid.PERMISSIONS.BLUETOOTH_SCAN,
        PermissionsAndroid.PERMISSIONS.BLUETOOTH_CONNECT,
        PermissionsAndroid.PERMISSIONS.ACCESS_FINE_LOCATION,
      ]);

      return Object.values(result).every(
        (status) => status === PermissionsAndroid.RESULTS.GRANTED
      );
    } else {
      const result = await PermissionsAndroid.request(
        PermissionsAndroid.PERMISSIONS.ACCESS_FINE_LOCATION
      );
      return result === PermissionsAndroid.RESULTS.GRANTED;
    }
  };

  // Handle discovered peripheral
  const handleDiscoverPeripheral = (peripheral: Peripheral) => {
    if (!peripheral.name) {
      peripheral.name = "Unknown Device";
    }

    setState((prev) => {
      const newPeripherals = new Map(prev.peripherals);
      newPeripherals.set(peripheral.id, peripheral);
      return { ...prev, peripherals: newPeripherals };
    });

    // Auto-connect if it's our device
    if (
      peripheral.name === DEVICE_NAME ||
      peripheral.localName === DEVICE_NAME
    ) {
      BleManager?.stopScan();
      connectToPeripheral(peripheral.id);
    }
  };

  // Handle scan stop
  const handleStopScan = () => {
    setState((prev) => ({ ...prev, isScanning: false }));
  };

  // Handle connect
  const handleConnectPeripheral = (data: { peripheral: string }) => {
    console.log("Connected to:", data.peripheral);
  };

  // Handle disconnect
  const handleDisconnectPeripheral = (data: { peripheral: string }) => {
    console.log("Disconnected from:", data.peripheral);
    setState((prev) => ({
      ...prev,
      isConnected: false,
      connectedPeripheralId: null,
      currentColor: null,
      error: "Device disconnected",
    }));
  };

  // Handle characteristic update (color data)
  const handleUpdateValue = (data: {
    peripheral: string;
    characteristic: string;
    value: number[];
  }) => {
    // Convert byte array to string
    const colorString = String.fromCharCode(...data.value);
    const color = parseColorData(colorString);

    if (color) {
      setState((prev) => ({ ...prev, currentColor: color }));
    }
  };

  // Parse color data from ESP32
  const parseColorData = (data: string): RGBColor | null => {
    try {
      // Expected format: "R,G,B,ColorName" or "R:XXX,G:XXX,B:XXX"
      let r: number, g: number, b: number;

      if (data.includes("R:")) {
        const match = data.match(/R:(\d+),G:(\d+),B:(\d+)/);
        if (match) {
          r = parseInt(match[1], 10);
          g = parseInt(match[2], 10);
          b = parseInt(match[3], 10);
        } else {
          return null;
        }
      } else {
        const parts = data.split(",").map((p) => parseInt(p.trim(), 10));
        if (parts.length >= 3) {
          [r, g, b] = parts;
        } else {
          return null;
        }
      }

      if (
        !isNaN(r) &&
        !isNaN(g) &&
        !isNaN(b) &&
        r >= 0 &&
        r <= 255 &&
        g >= 0 &&
        g <= 255 &&
        b >= 0 &&
        b <= 255
      ) {
        return { r, g, b };
      }
      return null;
    } catch {
      return null;
    }
  };

  // Generate mock color
  const generateMockColor = (): RGBColor => ({
    r: Math.floor(Math.random() * 256),
    g: Math.floor(Math.random() * 256),
    b: Math.floor(Math.random() * 256),
  });

  // Start mock mode
  const startMockMode = () => {
    setState((prev) => ({
      ...prev,
      isConnected: true,
      isConnecting: false,
      currentColor: generateMockColor(),
    }));

    mockIntervalRef.current = setInterval(() => {
      setState((prev) => ({
        ...prev,
        currentColor: generateMockColor(),
      }));
    }, 2000);
  };

  // Connect to peripheral
  const connectToPeripheral = async (peripheralId: string) => {
    if (!BleManager) return;

    try {
      setState((prev) => ({
        ...prev,
        isConnecting: true,
        error: null,
      }));

      await BleManager.connect(peripheralId);

      // Wait for bonding
      await new Promise((resolve) => setTimeout(resolve, 900));

      // Retrieve services
      const peripheralData = await BleManager.retrieveServices(peripheralId);
      console.log("Peripheral services:", peripheralData);

      // Start notification for color updates
      await BleManager.startNotification(
        peripheralId,
        SERVICE_UUID,
        CHARACTERISTIC_UUID
      );

      setState((prev) => ({
        ...prev,
        isConnecting: false,
        isConnected: true,
        connectedPeripheralId: peripheralId,
        error: null,
      }));
    } catch (error: any) {
      console.error("Connection error:", error);
      setState((prev) => ({
        ...prev,
        isConnecting: false,
        isConnected: false,
        error: error?.message || "Failed to connect",
      }));
    }
  };

  // Scan for devices
  const scan = useCallback(async () => {
    // Mock mode
    if (state.isMockMode || !BleManager) {
      setState((prev) => ({
        ...prev,
        isScanning: true,
        error: null,
      }));

      setTimeout(() => {
        setState((prev) => ({
          ...prev,
          isScanning: false,
          isConnecting: true,
        }));

        setTimeout(() => {
          startMockMode();
        }, 1000);
      }, 2000);

      return;
    }

    // Check Bluetooth state
    try {
      const btState = await BleManager.checkState();

      if (btState === "off") {
        if (Platform.OS === "ios") {
          Alert.alert(
            "Enable Bluetooth",
            "Please enable Bluetooth in Settings to continue.",
            [
              { text: "Cancel", style: "cancel" },
              {
                text: "Open Settings",
                onPress: () => Linking.openURL("App-Prefs:Bluetooth"),
              },
            ]
          );
          return;
        } else {
          await BleManager.enableBluetooth();
        }
      }

      // Clear previous peripherals and start scan
      setState((prev) => ({
        ...prev,
        peripherals: new Map(),
        isScanning: true,
        error: null,
      }));

      await BleManager.scan([SERVICE_UUID], 10, true);

      // Stop scan after 10 seconds if not connected
      setTimeout(() => {
        BleManager.stopScan();
        setState((prev) => {
          if (prev.isScanning && !prev.isConnected) {
            return {
              ...prev,
              isScanning: false,
              error: "Device not found. Make sure ESP32 is powered on.",
            };
          }
          return prev;
        });
      }, 10000);
    } catch (error: any) {
      console.error("Scan error:", error);
      setState((prev) => ({
        ...prev,
        isScanning: false,
        error: error?.message || "Failed to scan",
      }));
    }
  }, [state.isMockMode]);

  // Disconnect
  const disconnect = useCallback(async () => {
    // Stop mock mode
    if (mockIntervalRef.current) {
      clearInterval(mockIntervalRef.current);
      mockIntervalRef.current = null;
    }

    if (state.connectedPeripheralId && BleManager) {
      try {
        await BleManager.disconnect(state.connectedPeripheralId);
      } catch (error) {
        console.error("Disconnect error:", error);
      }
    }

    setState((prev) => ({
      ...prev,
      isConnected: false,
      connectedPeripheralId: null,
      currentColor: null,
      peripherals: new Map(),
      error: null,
    }));
  }, [state.connectedPeripheralId]);

  // Clear error
  const clearError = useCallback(() => {
    setState((prev) => ({ ...prev, error: null }));
  }, []);

  return {
    ...state,
    scan,
    disconnect,
    clearError,
  };
}

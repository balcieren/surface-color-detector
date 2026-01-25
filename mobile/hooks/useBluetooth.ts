import { useCallback, useEffect, useRef, useState } from "react";
import { Alert, Linking, PermissionsAndroid, Platform } from "react-native";
import { Buffer } from "buffer";

// Check if BLE library is available (not on web)
let BleManager: any = null;
let isBleAvailable = false;

// BLE is only available on native platforms
if (Platform.OS !== "web") {
  try {
    const blePlx = require("react-native-ble-plx");
    BleManager = blePlx.BleManager;
    isBleAvailable = true;
  } catch (e) {
    console.log("BLE PLX not available - running in mock mode");
  }
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

interface ScannedDevice {
  id: string;
  name: string | null;
  rssi: number | null;
}

interface BluetoothState {
  isScanning: boolean;
  isConnected: boolean;
  isConnecting: boolean;
  isMockMode: boolean;
  peripherals: Map<string, ScannedDevice>;
  connectedPeripheralId: string | null;
  currentColor: RGBColor | null;
  error: string | null;
}

export function useBluetooth() {
  const managerRef = useRef<any>(null);
  const deviceRef = useRef<any>(null);
  const subscriptionRef = useRef<any>(null);
  const mockIntervalRef = useRef<ReturnType<typeof setInterval> | null>(null);

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

    // Create BLE Manager instance (may fail in Expo Go)
    try {
      managerRef.current = new BleManager();
    } catch (e) {
      console.log("BLE Manager creation failed - running in mock mode", e);
      setState((prev) => ({ ...prev, isMockMode: true }));
      return;
    }

    // Listen for state changes
    const stateSubscription = managerRef.current.onStateChange(
      (bleState: string) => {
        console.log("BLE State:", bleState);
        if (bleState === "PoweredOff") {
          setState((prev) => ({
            ...prev,
            error: "Bluetooth is turned off",
            isConnected: false,
          }));
        }
      },
      true
    );

    return () => {
      stateSubscription?.remove();
      subscriptionRef.current?.remove();
      managerRef.current?.destroy();
      if (mockIntervalRef.current) {
        clearInterval(mockIntervalRef.current);
      }
    };
  }, []);

  // Request Android permissions
  const requestAndroidPermissions = async (): Promise<boolean> => {
    if (Platform.OS !== "android") return true;

    const apiLevel = Platform.Version;

    try {
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
    } catch (error) {
      console.error("Permission error:", error);
      return false;
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
        !isNaN(r!) &&
        !isNaN(g!) &&
        !isNaN(b!) &&
        r! >= 0 &&
        r! <= 255 &&
        g! >= 0 &&
        g! <= 255 &&
        b! >= 0 &&
        b! <= 255
      ) {
        return { r: r!, g: g!, b: b! };
      }
      return null;
    } catch {
      return null;
    }
  };

  // Decode base64 to string
  const decodeBase64 = (base64: string): string => {
    try {
      return Buffer.from(base64, "base64").toString("utf-8");
    } catch {
      return "";
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

  // Connect to device and start monitoring
  const connectToDevice = async (device: any) => {
    try {
      setState((prev) => ({
        ...prev,
        isConnecting: true,
        error: null,
      }));

      // Connect to device
      const connectedDevice = await device.connect();
      deviceRef.current = connectedDevice;

      // Discover services and characteristics
      await connectedDevice.discoverAllServicesAndCharacteristics();

      // Setup disconnect listener
      connectedDevice.onDisconnected((error: any, disconnectedDevice: any) => {
        console.log("Device disconnected:", disconnectedDevice?.id);
        subscriptionRef.current?.remove();
        setState((prev) => ({
          ...prev,
          isConnected: false,
          connectedPeripheralId: null,
          currentColor: null,
          error: error ? "Device disconnected unexpectedly" : null,
        }));
      });

      // Start monitoring characteristic for color updates
      subscriptionRef.current = connectedDevice.monitorCharacteristicForService(
        SERVICE_UUID,
        CHARACTERISTIC_UUID,
        (error: any, characteristic: any) => {
          if (error) {
            console.error("Monitor error:", error);
            return;
          }

          if (characteristic?.value) {
            const decodedData = decodeBase64(characteristic.value);
            const color = parseColorData(decodedData);

            if (color) {
              setState((prev) => ({ ...prev, currentColor: color }));
            }
          }
        }
      );

      setState((prev) => ({
        ...prev,
        isConnecting: false,
        isConnected: true,
        connectedPeripheralId: connectedDevice.id,
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
    if (state.isMockMode || !managerRef.current) {
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

    // Request permissions on Android
    if (Platform.OS === "android") {
      const hasPermissions = await requestAndroidPermissions();
      if (!hasPermissions) {
        setState((prev) => ({
          ...prev,
          error: "Bluetooth permissions not granted",
        }));
        return;
      }
    }

    // Check Bluetooth state
    const bleState = await managerRef.current.state();

    if (bleState === "PoweredOff") {
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
        // Android - try to enable
        try {
          await managerRef.current.enable();
        } catch {
          setState((prev) => ({
            ...prev,
            error: "Please enable Bluetooth",
          }));
          return;
        }
      }
    }

    // Clear previous peripherals and start scan
    setState((prev) => ({
      ...prev,
      peripherals: new Map(),
      isScanning: true,
      error: null,
    }));

    // Set timeout to stop scan
    const scanTimeout = setTimeout(() => {
      managerRef.current?.stopDeviceScan();
      setState((prev) => {
        if (prev.isScanning && !prev.isConnected) {
          return {
            ...prev,
            isScanning: false,
            error: "Device not found. Make sure device is powered on.",
          };
        }
        return prev;
      });
    }, 10000);

    // Start scanning
    managerRef.current.startDeviceScan(
      [SERVICE_UUID],
      { allowDuplicates: false },
      (error: any, device: any) => {
        if (error) {
          console.error("Scan error:", error);
          clearTimeout(scanTimeout);
          setState((prev) => ({
            ...prev,
            isScanning: false,
            error: error.message || "Scan failed",
          }));
          return;
        }

        if (device) {
          // Add to peripherals list
          setState((prev) => {
            const newPeripherals = new Map(prev.peripherals);
            newPeripherals.set(device.id, {
              id: device.id,
              name: device.name || device.localName || "Unknown Device",
              rssi: device.rssi,
            });
            return { ...prev, peripherals: newPeripherals };
          });

          // Auto-connect if it's our device
          if (device.name === DEVICE_NAME || device.localName === DEVICE_NAME) {
            clearTimeout(scanTimeout);
            managerRef.current.stopDeviceScan();
            connectToDevice(device);
          }
        }
      }
    );
  }, [state.isMockMode]);

  // Disconnect
  const disconnect = useCallback(async () => {
    // Stop mock mode
    if (mockIntervalRef.current) {
      clearInterval(mockIntervalRef.current);
      mockIntervalRef.current = null;
    }

    // Remove subscription
    subscriptionRef.current?.remove();
    subscriptionRef.current = null;

    // Disconnect device
    if (deviceRef.current) {
      try {
        await deviceRef.current.cancelConnection();
      } catch (error) {
        console.error("Disconnect error:", error);
      }
      deviceRef.current = null;
    }

    setState((prev) => ({
      ...prev,
      isConnected: false,
      connectedPeripheralId: null,
      currentColor: null,
      peripherals: new Map(),
      error: null,
    }));
  }, []);

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

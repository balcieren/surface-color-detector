import AsyncStorage from "@react-native-async-storage/async-storage";
import { useCallback, useEffect, useState } from "react";

const STORAGE_KEY = "@robot_device_id";

export function useDeviceId() {
  const [deviceId, setDeviceIdState] = useState<string | null>(null);
  const [isLoading, setIsLoading] = useState(true);

  const load = useCallback(async () => {
    setIsLoading(true);
    const stored = await AsyncStorage.getItem(STORAGE_KEY);
    const id = stored ?? process.env.EXPO_PUBLIC_MCU_DEVICE_ID ?? null;
    if (id && !stored) await AsyncStorage.setItem(STORAGE_KEY, id);
    setDeviceIdState(id);
    setIsLoading(false);
  }, []);

  useEffect(() => {
    load();
  }, [load]);

  const setDeviceId = async (id: string) => {
    await AsyncStorage.setItem(STORAGE_KEY, id);
    setDeviceIdState(id);
  };

  const clearDeviceId = async () => {
    await AsyncStorage.removeItem(STORAGE_KEY);
    setDeviceIdState(null);
  };

  return {
    deviceId,
    isLoading,
    isConfigured: !!deviceId,
    setDeviceId,
    clearDeviceId,
    refresh: load,
  };
}

export default useDeviceId;

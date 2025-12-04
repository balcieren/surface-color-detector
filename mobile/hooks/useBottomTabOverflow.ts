import { useBottomTabBarHeight } from "@react-navigation/bottom-tabs";
import { Platform } from "react-native";

export function useBottomTabOverflow() {
  try {
    const tabBarHeight = useBottomTabBarHeight();
    return Platform.OS === "ios" ? tabBarHeight : 0;
  } catch {
    // Not inside a tab navigator
    return 0;
  }
}

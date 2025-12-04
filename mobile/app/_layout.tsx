import queryClient from "@/api/config/queryClient";
import { useColor } from "@/hooks/useColor";
import { ThemeProvider } from "@/providers/theme-provider";
import { QueryClientProvider } from "@tanstack/react-query";
import { Tabs } from "expo-router";
import { StatusBar } from "expo-status-bar";
import { Bluetooth, List } from "lucide-react-native";
import { GestureHandlerRootView } from "react-native-gesture-handler";

function TabBarIcon({
  icon: Icon,
  color,
}: {
  icon: React.ComponentType<any>;
  color: string;
}) {
  return <Icon size={24} color={color} />;
}

function TabLayout() {
  const primaryColor = useColor("primary");
  const mutedColor = useColor("mutedForeground");
  const backgroundColor = useColor("background");

  return (
    <Tabs
      screenOptions={{
        headerShown: false,
        tabBarActiveTintColor: primaryColor,
        tabBarInactiveTintColor: mutedColor,
        tabBarStyle: {
          backgroundColor: backgroundColor,
          borderTopWidth: 0.5,
        },
      }}
    >
      <Tabs.Screen
        name="index"
        options={{
          title: "Scanner",
          tabBarIcon: ({ color }) => (
            <TabBarIcon icon={Bluetooth} color={color} />
          ),
        }}
      />
      <Tabs.Screen
        name="lists"
        options={{
          title: "Lists",
          tabBarIcon: ({ color }) => <TabBarIcon icon={List} color={color} />,
        }}
      />
      <Tabs.Screen
        name="list/[id]"
        options={{
          href: null,
        }}
      />
    </Tabs>
  );
}

export default function RootLayout() {
  return (
    <QueryClientProvider client={queryClient}>
      <GestureHandlerRootView style={{ flex: 1 }}>
        <ThemeProvider>
          <TabLayout />
          <StatusBar style="auto" animated />
        </ThemeProvider>
      </GestureHandlerRootView>
    </QueryClientProvider>
  );
}

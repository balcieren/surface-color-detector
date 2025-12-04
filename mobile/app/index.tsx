import { Button } from "@/components/ui/button";
import { Card, CardContent } from "@/components/ui/card";
import { ModeToggle } from "@/components/ui/mode-toggle";
import { Text } from "@/components/ui/text";
import { View } from "@/components/ui/view";
import { useBluetooth } from "@/hooks/useBluetooth";
import { useColor } from "@/hooks/useColor";
import { useColorStore } from "@/stores/colorStore";
import * as Haptics from "expo-haptics";
import {
  Bluetooth,
  BluetoothOff,
  Check,
  ChevronDown,
  Plus,
  Save,
  WifiOff,
} from "lucide-react-native";
import { useState } from "react";
import { Alert, Modal, Pressable, ScrollView, TextInput } from "react-native";
import { useSafeAreaInsets } from "react-native-safe-area-context";

export default function ScannerScreen() {
  const insets = useSafeAreaInsets();
  const backgroundColor = useColor("background");
  const cardColor = useColor("card");
  const textColor = useColor("text");
  const mutedColor = useColor("mutedForeground");
  const primaryColor = useColor("primary");
  const dangerColor = useColor("red");
  const greenColor = useColor("green");
  const borderColor = useColor("border");

  const {
    isScanning,
    isConnected,
    isConnecting,
    isMockMode,
    currentColor,
    error,
    scan,
    disconnect,
    clearError,
  } = useBluetooth();

  const { lists, selectedListId, selectList, addColor, createList } =
    useColorStore();

  const [showListPicker, setShowListPicker] = useState(false);
  const [showNewListModal, setShowNewListModal] = useState(false);
  const [newListName, setNewListName] = useState("");

  const selectedList = lists.find((l) => l.id === selectedListId);

  const handleSaveColor = () => {
    if (!currentColor) {
      Alert.alert("Error", "No color detected");
      return;
    }

    if (!selectedListId) {
      Alert.alert("Error", "Please select a list first");
      return;
    }

    addColor(selectedListId, currentColor.r, currentColor.g, currentColor.b);
    Haptics.notificationAsync(Haptics.NotificationFeedbackType.Success);
  };

  const handleCreateList = () => {
    if (!newListName.trim()) {
      Alert.alert("Error", "Please enter a list name");
      return;
    }

    createList(newListName.trim());
    setNewListName("");
    setShowNewListModal(false);
    Haptics.notificationAsync(Haptics.NotificationFeedbackType.Success);
  };

  const rgbToHex = (r: number, g: number, b: number) => {
    return `#${((1 << 24) + (r << 16) + (g << 8) + b)
      .toString(16)
      .slice(1)
      .toUpperCase()}`;
  };

  return (
    <View
      style={{
        flex: 1,
        backgroundColor: backgroundColor,
        paddingTop: insets.top,
        paddingBottom: insets.bottom,
      }}
    >
      <ScrollView
        contentContainerStyle={{ padding: 16, gap: 16 }}
        showsVerticalScrollIndicator={false}
      >
        {/* Header */}
        <View
          style={{
            flexDirection: "row",
            alignItems: "center",
            justifyContent: "space-between",
            marginBottom: 8,
          }}
        >
          <View>
            <Text variant="title" style={{ marginBottom: 4 }}>
              Color Scanner
            </Text>
            <Text variant="caption">Connect to ESP32 to scan colors</Text>
          </View>
          <ModeToggle />
        </View>

        {/* Connection Status Card */}
        <Card>
          <CardContent>
            <View
              style={{
                flexDirection: "row",
                alignItems: "center",
                justifyContent: "space-between",
              }}
            >
              <View
                style={{ flexDirection: "row", alignItems: "center", gap: 12 }}
              >
                {isConnected ? (
                  <Bluetooth size={24} color={greenColor} />
                ) : (
                  <BluetoothOff size={24} color={mutedColor} />
                )}
                <View>
                  <Text variant="subtitle">
                    {isConnected
                      ? isMockMode
                        ? "Mock Mode"
                        : "Connected"
                      : "Disconnected"}
                  </Text>
                  <Text variant="caption">
                    {isConnected
                      ? isMockMode
                        ? "Demo - Random colors"
                        : "ESP32 Color Sensor"
                      : isConnecting
                      ? "Connecting..."
                      : isScanning
                      ? "Scanning..."
                      : "Tap to connect"}
                  </Text>
                </View>
              </View>

              {isConnected ? (
                <Button variant="destructive" size="sm" onPress={disconnect}>
                  Disconnect
                </Button>
              ) : (
                <Button
                  variant="default"
                  size="sm"
                  onPress={scan}
                  loading={isScanning || isConnecting}
                >
                  {isScanning ? "Scanning" : "Connect"}
                </Button>
              )}
            </View>
          </CardContent>
        </Card>

        {/* Error Message */}
        {error && (
          <Card style={{ backgroundColor: dangerColor + "20" }}>
            <CardContent>
              <View
                style={{
                  flexDirection: "row",
                  alignItems: "center",
                  gap: 12,
                }}
              >
                <WifiOff size={24} color={dangerColor} />
                <View style={{ flex: 1 }}>
                  <Text style={{ color: dangerColor, fontWeight: "600" }}>
                    Connection Error
                  </Text>
                  <Text variant="caption" style={{ color: dangerColor }}>
                    {error}
                  </Text>
                </View>
                <Pressable onPress={clearError}>
                  <Text style={{ color: dangerColor, fontWeight: "600" }}>
                    Dismiss
                  </Text>
                </Pressable>
              </View>
            </CardContent>
          </Card>
        )}

        {/* Color Display */}
        <Card>
          <CardContent>
            <Text variant="subtitle" style={{ marginBottom: 16 }}>
              Detected Color
            </Text>

            {/* Color Preview */}
            <View
              style={{
                width: "100%",
                height: 150,
                borderRadius: 16,
                backgroundColor: currentColor
                  ? `rgb(${currentColor.r}, ${currentColor.g}, ${currentColor.b})`
                  : cardColor,
                borderWidth: 1,
                borderColor: borderColor,
                alignItems: "center",
                justifyContent: "center",
                marginBottom: 16,
              }}
            >
              {!currentColor && (
                <Text variant="caption">
                  {isConnected ? "Waiting for color data..." : "Not connected"}
                </Text>
              )}
            </View>

            {/* RGB Values */}
            {currentColor && (
              <View style={{ gap: 12 }}>
                <View
                  style={{
                    flexDirection: "row",
                    justifyContent: "space-between",
                  }}
                >
                  <Text variant="caption">RGB</Text>
                  <Text style={{ fontWeight: "600" }}>
                    {currentColor.r}, {currentColor.g}, {currentColor.b}
                  </Text>
                </View>
                <View
                  style={{
                    flexDirection: "row",
                    justifyContent: "space-between",
                  }}
                >
                  <Text variant="caption">HEX</Text>
                  <Text style={{ fontWeight: "600" }}>
                    {rgbToHex(currentColor.r, currentColor.g, currentColor.b)}
                  </Text>
                </View>
              </View>
            )}
          </CardContent>
        </Card>

        {/* List Selection */}
        <Card>
          <CardContent>
            <Text variant="subtitle" style={{ marginBottom: 16 }}>
              Save to List
            </Text>

            {/* List Picker */}
            <Pressable
              onPress={() => setShowListPicker(true)}
              style={{
                flexDirection: "row",
                alignItems: "center",
                justifyContent: "space-between",
                backgroundColor: cardColor,
                borderWidth: 1,
                borderColor: borderColor,
                borderRadius: 12,
                padding: 16,
                marginBottom: 16,
              }}
            >
              <Text>
                {selectedList ? selectedList.name : "Select a list..."}
              </Text>
              <ChevronDown size={20} color={mutedColor} />
            </Pressable>

            {/* Save Button */}
            <Button
              variant="default"
              onPress={handleSaveColor}
              disabled={!isConnected || !currentColor || !selectedListId}
              icon={Save}
            >
              Save Color
            </Button>
          </CardContent>
        </Card>
      </ScrollView>

      {/* List Picker Modal */}
      <Modal
        visible={showListPicker}
        animationType="slide"
        transparent
        onRequestClose={() => setShowListPicker(false)}
      >
        <View
          style={{
            flex: 1,
            backgroundColor: "rgba(0,0,0,0.5)",
            justifyContent: "flex-end",
          }}
        >
          <View
            style={{
              backgroundColor: backgroundColor,
              borderTopLeftRadius: 24,
              borderTopRightRadius: 24,
              paddingTop: 16,
              paddingBottom: insets.bottom + 16,
              maxHeight: "60%",
            }}
          >
            <View
              style={{
                flexDirection: "row",
                alignItems: "center",
                justifyContent: "space-between",
                paddingHorizontal: 16,
                marginBottom: 16,
              }}
            >
              <Text variant="title">Select List</Text>
              <Pressable onPress={() => setShowListPicker(false)}>
                <Text style={{ color: primaryColor }}>Done</Text>
              </Pressable>
            </View>

            <ScrollView style={{ paddingHorizontal: 16 }}>
              {lists.map((list) => (
                <Pressable
                  key={list.id}
                  onPress={() => {
                    selectList(list.id);
                    setShowListPicker(false);
                  }}
                  style={{
                    flexDirection: "row",
                    alignItems: "center",
                    justifyContent: "space-between",
                    paddingVertical: 16,
                    borderBottomWidth: 1,
                    borderBottomColor: borderColor,
                  }}
                >
                  <View>
                    <Text style={{ fontWeight: "500" }}>{list.name}</Text>
                    <Text variant="caption">
                      {list.colors.length} color
                      {list.colors.length !== 1 ? "s" : ""}
                    </Text>
                  </View>
                  {selectedListId === list.id && (
                    <Check size={20} color={greenColor} />
                  )}
                </Pressable>
              ))}

              {lists.length === 0 && (
                <Text
                  variant="caption"
                  style={{ textAlign: "center", marginTop: 32 }}
                >
                  No lists yet. Create one below.
                </Text>
              )}
            </ScrollView>

            <View style={{ padding: 16 }}>
              <Button
                variant="outline"
                onPress={() => {
                  setShowListPicker(false);
                  setShowNewListModal(true);
                }}
                icon={Plus}
              >
                Create New List
              </Button>
            </View>
          </View>
        </View>
      </Modal>

      {/* New List Modal */}
      <Modal
        visible={showNewListModal}
        animationType="fade"
        transparent
        onRequestClose={() => setShowNewListModal(false)}
      >
        <View
          style={{
            flex: 1,
            backgroundColor: "rgba(0,0,0,0.5)",
            justifyContent: "center",
            alignItems: "center",
            padding: 32,
          }}
        >
          <View
            style={{
              backgroundColor: backgroundColor,
              borderRadius: 16,
              padding: 24,
              width: "100%",
            }}
          >
            <Text variant="title" style={{ marginBottom: 16 }}>
              New List
            </Text>

            <TextInput
              value={newListName}
              onChangeText={setNewListName}
              placeholder="Enter list name..."
              placeholderTextColor={mutedColor}
              style={{
                backgroundColor: cardColor,
                borderRadius: 12,
                padding: 16,
                fontSize: 17,
                color: textColor,
                marginBottom: 16,
              }}
              autoFocus
            />

            <View style={{ flexDirection: "row", gap: 12 }}>
              <Button
                variant="outline"
                onPress={() => {
                  setShowNewListModal(false);
                  setNewListName("");
                }}
                style={{ flex: 1 }}
              >
                Cancel
              </Button>
              <Button
                variant="default"
                onPress={handleCreateList}
                style={{ flex: 1 }}
              >
                Create
              </Button>
            </View>
          </View>
        </View>
      </Modal>
    </View>
  );
}

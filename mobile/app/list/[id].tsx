import { Button } from "@/components/ui/button";
import { Card, CardContent } from "@/components/ui/card";
import { Text } from "@/components/ui/text";
import { View } from "@/components/ui/view";
import { useColor } from "@/hooks/useColor";
import { useColorStore } from "@/stores/colorStore";
import * as Clipboard from "expo-clipboard";
import * as Haptics from "expo-haptics";
import { useLocalSearchParams, useRouter } from "expo-router";
import { ArrowLeft, Copy, Edit2, Palette, Trash2 } from "lucide-react-native";
import { useState } from "react";
import { Alert, Modal, Pressable, ScrollView, TextInput } from "react-native";
import { useSafeAreaInsets } from "react-native-safe-area-context";

export default function ListDetailScreen() {
  const { id } = useLocalSearchParams<{ id: string }>();
  const insets = useSafeAreaInsets();
  const router = useRouter();
  const backgroundColor = useColor("background");
  const cardColor = useColor("card");
  const textColor = useColor("text");
  const mutedColor = useColor("mutedForeground");
  const primaryColor = useColor("primary");
  const dangerColor = useColor("red");
  const greenColor = useColor("green");
  const borderColor = useColor("border");

  const { lists, updateListName, deleteColor, updateColorName, deleteList } =
    useColorStore();

  const list = lists.find((l) => l.id === id);

  const [showEditNameModal, setShowEditNameModal] = useState(false);
  const [editedName, setEditedName] = useState(list?.name || "");
  const [showEditColorModal, setShowEditColorModal] = useState(false);
  const [selectedColorId, setSelectedColorId] = useState<string | null>(null);
  const [editedColorName, setEditedColorName] = useState("");

  if (!list) {
    return (
      <View
        style={{
          flex: 1,
          backgroundColor: backgroundColor,
          alignItems: "center",
          justifyContent: "center",
        }}
      >
        <Text>List not found</Text>
        <Button
          variant="outline"
          onPress={() => router.back()}
          style={{ marginTop: 16 }}
        >
          Go Back
        </Button>
      </View>
    );
  }

  const handleUpdateListName = () => {
    if (!editedName.trim()) {
      Alert.alert("Error", "Please enter a list name");
      return;
    }

    updateListName(id!, editedName.trim());
    setShowEditNameModal(false);
    Haptics.notificationAsync(Haptics.NotificationFeedbackType.Success);
  };

  const handleDeleteList = () => {
    Alert.alert(
      "Delete List",
      `Are you sure you want to delete "${list.name}"? This action cannot be undone.`,
      [
        { text: "Cancel", style: "cancel" },
        {
          text: "Delete",
          style: "destructive",
          onPress: () => {
            deleteList(id!);
            router.back();
            Haptics.notificationAsync(Haptics.NotificationFeedbackType.Success);
          },
        },
      ]
    );
  };

  const handleDeleteColor = (colorId: string) => {
    Alert.alert("Delete Color", "Are you sure you want to delete this color?", [
      { text: "Cancel", style: "cancel" },
      {
        text: "Delete",
        style: "destructive",
        onPress: () => {
          deleteColor(id!, colorId);
          Haptics.notificationAsync(Haptics.NotificationFeedbackType.Success);
        },
      },
    ]);
  };

  const handleEditColor = (colorId: string, currentName?: string) => {
    setSelectedColorId(colorId);
    setEditedColorName(currentName || "");
    setShowEditColorModal(true);
  };

  const handleUpdateColorName = () => {
    if (selectedColorId) {
      updateColorName(id!, selectedColorId, editedColorName.trim());
      setShowEditColorModal(false);
      setSelectedColorId(null);
      Haptics.notificationAsync(Haptics.NotificationFeedbackType.Success);
    }
  };

  const rgbToHex = (r: number, g: number, b: number) => {
    return `#${((1 << 24) + (r << 16) + (g << 8) + b)
      .toString(16)
      .slice(1)
      .toUpperCase()}`;
  };

  const copyToClipboard = async (text: string) => {
    await Clipboard.setStringAsync(text);
    Haptics.notificationAsync(Haptics.NotificationFeedbackType.Success);
    Alert.alert("Copied", "Color code copied to clipboard");
  };

  const formatDate = (timestamp: number) => {
    const date = new Date(timestamp);
    return date.toLocaleDateString("en-US", {
      month: "short",
      day: "numeric",
      hour: "2-digit",
      minute: "2-digit",
    });
  };

  return (
    <View
      style={{
        flex: 1,
        backgroundColor: backgroundColor,
        paddingTop: insets.top,
      }}
    >
      {/* Header */}
      <View
        style={{
          flexDirection: "row",
          alignItems: "center",
          justifyContent: "space-between",
          padding: 16,
          borderBottomWidth: 1,
          borderBottomColor: borderColor,
        }}
      >
        <View style={{ flexDirection: "row", alignItems: "center", gap: 12 }}>
          <Pressable onPress={() => router.back()} hitSlop={8}>
            <ArrowLeft size={24} color={textColor} />
          </Pressable>
          <Pressable
            onPress={() => {
              setEditedName(list.name);
              setShowEditNameModal(true);
            }}
          >
            <View
              style={{ flexDirection: "row", alignItems: "center", gap: 8 }}
            >
              <Text variant="title">{list.name}</Text>
              <Edit2 size={16} color={mutedColor} />
            </View>
          </Pressable>
        </View>

        <Pressable onPress={handleDeleteList} hitSlop={8}>
          <Trash2 size={22} color={dangerColor} />
        </Pressable>
      </View>

      <ScrollView
        contentContainerStyle={{ padding: 16, gap: 12 }}
        showsVerticalScrollIndicator={false}
      >
        {/* Stats */}
        <Text variant="caption">
          {list.colors.length} color{list.colors.length !== 1 ? "s" : ""}
        </Text>

        {/* Colors */}
        {list.colors.length === 0 ? (
          <Card>
            <CardContent style={{ alignItems: "center", paddingVertical: 48 }}>
              <Palette size={48} color={mutedColor} />
              <Text
                variant="subtitle"
                style={{ marginTop: 16, textAlign: "center" }}
              >
                No Colors Yet
              </Text>
              <Text
                variant="caption"
                style={{ marginTop: 8, textAlign: "center" }}
              >
                Connect to your device and start scanning colors
              </Text>
            </CardContent>
          </Card>
        ) : (
          list.colors.map((color) => {
            const hex = rgbToHex(color.r, color.g, color.b);
            return (
              <Card key={color.id}>
                <CardContent>
                  <View
                    style={{
                      flexDirection: "row",
                      alignItems: "center",
                      gap: 16,
                    }}
                  >
                    {/* Color Preview */}
                    <View
                      style={{
                        width: 60,
                        height: 60,
                        borderRadius: 12,
                        backgroundColor: `rgb(${color.r}, ${color.g}, ${color.b})`,
                        borderWidth: 1,
                        borderColor: borderColor,
                      }}
                    />

                    {/* Color Info */}
                    <View style={{ flex: 1 }}>
                      {color.name && (
                        <Text style={{ fontWeight: "600", marginBottom: 4 }}>
                          {color.name}
                        </Text>
                      )}
                      <Text variant="caption">
                        RGB: {color.r}, {color.g}, {color.b}
                      </Text>
                      <Text variant="caption">HEX: {hex}</Text>
                      <Text
                        variant="caption"
                        style={{ fontSize: 12, marginTop: 4 }}
                      >
                        {formatDate(color.createdAt)}
                      </Text>
                    </View>

                    {/* Actions */}
                    <View style={{ gap: 8 }}>
                      <Pressable
                        onPress={() => copyToClipboard(hex)}
                        hitSlop={8}
                        style={{
                          padding: 8,
                          backgroundColor: cardColor,
                          borderRadius: 8,
                        }}
                      >
                        <Copy size={18} color={primaryColor} />
                      </Pressable>
                      <Pressable
                        onPress={() => handleEditColor(color.id, color.name)}
                        hitSlop={8}
                        style={{
                          padding: 8,
                          backgroundColor: cardColor,
                          borderRadius: 8,
                        }}
                      >
                        <Edit2 size={18} color={primaryColor} />
                      </Pressable>
                      <Pressable
                        onPress={() => handleDeleteColor(color.id)}
                        hitSlop={8}
                        style={{
                          padding: 8,
                          backgroundColor: dangerColor + "20",
                          borderRadius: 8,
                        }}
                      >
                        <Trash2 size={18} color={dangerColor} />
                      </Pressable>
                    </View>
                  </View>
                </CardContent>
              </Card>
            );
          })
        )}
      </ScrollView>

      {/* Edit List Name Modal */}
      <Modal
        visible={showEditNameModal}
        animationType="fade"
        transparent
        onRequestClose={() => setShowEditNameModal(false)}
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
              Edit List Name
            </Text>

            <TextInput
              value={editedName}
              onChangeText={setEditedName}
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
                onPress={() => setShowEditNameModal(false)}
                style={{ flex: 1 }}
              >
                Cancel
              </Button>
              <Button
                variant="default"
                onPress={handleUpdateListName}
                style={{ flex: 1 }}
              >
                Save
              </Button>
            </View>
          </View>
        </View>
      </Modal>

      {/* Edit Color Name Modal */}
      <Modal
        visible={showEditColorModal}
        animationType="fade"
        transparent
        onRequestClose={() => setShowEditColorModal(false)}
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
              Edit Color Name
            </Text>

            <TextInput
              value={editedColorName}
              onChangeText={setEditedColorName}
              placeholder="Enter color name (optional)..."
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
                  setShowEditColorModal(false);
                  setSelectedColorId(null);
                }}
                style={{ flex: 1 }}
              >
                Cancel
              </Button>
              <Button
                variant="default"
                onPress={handleUpdateColorName}
                style={{ flex: 1 }}
              >
                Save
              </Button>
            </View>
          </View>
        </View>
      </Modal>
    </View>
  );
}

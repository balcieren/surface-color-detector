import { Button } from "@/components/ui/button";
import { Card, CardContent } from "@/components/ui/card";
import { Text } from "@/components/ui/text";
import { View } from "@/components/ui/view";
import { useColor } from "@/hooks/useColor";
import { useColorStore } from "@/stores/colorStore";
import * as Haptics from "expo-haptics";
import { useRouter } from "expo-router";
import {
  ChevronRight,
  Palette,
  Plus,
  Trash2,
} from "lucide-react-native";
import { useState } from "react";
import {
  Alert,
  Modal,
  Pressable,
  ScrollView,
  TextInput,
} from "react-native";
import { useSafeAreaInsets } from "react-native-safe-area-context";

export default function ListsScreen() {
  const insets = useSafeAreaInsets();
  const router = useRouter();
  const backgroundColor = useColor("background");
  const cardColor = useColor("card");
  const textColor = useColor("text");
  const mutedColor = useColor("mutedForeground");
  const primaryColor = useColor("primary");
  const dangerColor = useColor("red");
  const borderColor = useColor("border");

  const { lists, createList, deleteList } = useColorStore();

  const [showNewListModal, setShowNewListModal] = useState(false);
  const [newListName, setNewListName] = useState("");

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

  const handleDeleteList = (listId: string, listName: string) => {
    Alert.alert(
      "Delete List",
      `Are you sure you want to delete "${listName}"? This action cannot be undone.`,
      [
        { text: "Cancel", style: "cancel" },
        {
          text: "Delete",
          style: "destructive",
          onPress: () => {
            deleteList(listId);
            Haptics.notificationAsync(Haptics.NotificationFeedbackType.Success);
          },
        },
      ]
    );
  };

  const formatDate = (timestamp: number) => {
    const date = new Date(timestamp);
    return date.toLocaleDateString("en-US", {
      month: "short",
      day: "numeric",
      year: "numeric",
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
            <Text variant="title">Color Lists</Text>
            <Text variant="caption">
              {lists.length} list{lists.length !== 1 ? "s" : ""}
            </Text>
          </View>
          <Button
            variant="default"
            size="icon"
            onPress={() => setShowNewListModal(true)}
            icon={Plus}
          />
        </View>

        {/* Lists */}
        {lists.length === 0 ? (
          <Card>
            <CardContent
              style={{ alignItems: "center", paddingVertical: 48 }}
            >
              <Palette size={48} color={mutedColor} />
              <Text
                variant="subtitle"
                style={{ marginTop: 16, textAlign: "center" }}
              >
                No Lists Yet
              </Text>
              <Text
                variant="caption"
                style={{ marginTop: 8, textAlign: "center" }}
              >
                Create your first list to start saving colors
              </Text>
              <Button
                variant="default"
                onPress={() => setShowNewListModal(true)}
                icon={Plus}
                style={{ marginTop: 24 }}
              >
                Create List
              </Button>
            </CardContent>
          </Card>
        ) : (
          lists.map((list) => (
            <Card key={list.id}>
              <Pressable
                onPress={() => router.push(`/list/${list.id}` as any)}
                style={{ padding: 16 }}
              >
                <View
                  style={{
                    flexDirection: "row",
                    alignItems: "center",
                    justifyContent: "space-between",
                  }}
                >
                  <View style={{ flex: 1 }}>
                    <Text variant="subtitle">{list.name}</Text>
                    <Text variant="caption" style={{ marginTop: 4 }}>
                      {list.colors.length} color
                      {list.colors.length !== 1 ? "s" : ""} • Updated{" "}
                      {formatDate(list.updatedAt)}
                    </Text>

                    {/* Color Preview */}
                    {list.colors.length > 0 && (
                      <View
                        style={{
                          flexDirection: "row",
                          marginTop: 12,
                          gap: 6,
                        }}
                      >
                        {list.colors.slice(0, 6).map((color) => (
                          <View
                            key={color.id}
                            style={{
                              width: 28,
                              height: 28,
                              borderRadius: 6,
                              backgroundColor: `rgb(${color.r}, ${color.g}, ${color.b})`,
                              borderWidth: 1,
                              borderColor: borderColor,
                            }}
                          />
                        ))}
                        {list.colors.length > 6 && (
                          <View
                            style={{
                              width: 28,
                              height: 28,
                              borderRadius: 6,
                              backgroundColor: cardColor,
                              borderWidth: 1,
                              borderColor: borderColor,
                              alignItems: "center",
                              justifyContent: "center",
                            }}
                          >
                            <Text
                              style={{ fontSize: 10, color: mutedColor }}
                            >
                              +{list.colors.length - 6}
                            </Text>
                          </View>
                        )}
                      </View>
                    )}
                  </View>

                  <View style={{ flexDirection: "row", alignItems: "center", gap: 8 }}>
                    <Pressable
                      onPress={(e) => {
                        e.stopPropagation();
                        handleDeleteList(list.id, list.name);
                      }}
                      hitSlop={8}
                      style={{ padding: 8 }}
                    >
                      <Trash2 size={20} color={dangerColor} />
                    </Pressable>
                    <ChevronRight size={20} color={mutedColor} />
                  </View>
                </View>
              </Pressable>
            </Card>
          ))
        )}
      </ScrollView>

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

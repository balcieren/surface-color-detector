import AsyncStorage from "@react-native-async-storage/async-storage";
import { create } from "zustand";
import { createJSONStorage, persist } from "zustand/middleware";

export interface Color {
  id: string;
  r: number;
  g: number;
  b: number;
  name?: string;
  createdAt: number;
}

export interface ColorList {
  id: string;
  name: string;
  colors: Color[];
  createdAt: number;
  updatedAt: number;
}

interface ColorStore {
  lists: ColorList[];
  selectedListId: string | null;

  // List operations
  createList: (name: string) => string;
  deleteList: (listId: string) => void;
  updateListName: (listId: string, name: string) => void;
  selectList: (listId: string | null) => void;
  getSelectedList: () => ColorList | null;

  // Color operations
  addColor: (
    listId: string,
    r: number,
    g: number,
    b: number,
    name?: string
  ) => void;
  deleteColor: (listId: string, colorId: string) => void;
  updateColorName: (listId: string, colorId: string, name: string) => void;
}

export const useColorStore = create<ColorStore>()(
  persist(
    (set, get) => ({
      lists: [],
      selectedListId: null,

      // List operations
      createList: (name: string) => {
        const id = Date.now().toString();
        const newList: ColorList = {
          id,
          name,
          colors: [],
          createdAt: Date.now(),
          updatedAt: Date.now(),
        };

        set((state) => ({
          lists: [...state.lists, newList],
          selectedListId: id,
        }));

        return id;
      },

      deleteList: (listId: string) => {
        set((state) => ({
          lists: state.lists.filter((list) => list.id !== listId),
          selectedListId:
            state.selectedListId === listId ? null : state.selectedListId,
        }));
      },

      updateListName: (listId: string, name: string) => {
        set((state) => ({
          lists: state.lists.map((list) =>
            list.id === listId ? { ...list, name, updatedAt: Date.now() } : list
          ),
        }));
      },

      selectList: (listId: string | null) => {
        set({ selectedListId: listId });
      },

      getSelectedList: () => {
        const state = get();
        return (
          state.lists.find((list) => list.id === state.selectedListId) || null
        );
      },

      // Color operations
      addColor: (
        listId: string,
        r: number,
        g: number,
        b: number,
        name?: string
      ) => {
        const newColor: Color = {
          id: Date.now().toString(),
          r,
          g,
          b,
          name,
          createdAt: Date.now(),
        };

        set((state) => ({
          lists: state.lists.map((list) =>
            list.id === listId
              ? {
                  ...list,
                  colors: [...list.colors, newColor],
                  updatedAt: Date.now(),
                }
              : list
          ),
        }));
      },

      deleteColor: (listId: string, colorId: string) => {
        set((state) => ({
          lists: state.lists.map((list) =>
            list.id === listId
              ? {
                  ...list,
                  colors: list.colors.filter((color) => color.id !== colorId),
                  updatedAt: Date.now(),
                }
              : list
          ),
        }));
      },

      updateColorName: (listId: string, colorId: string, name: string) => {
        set((state) => ({
          lists: state.lists.map((list) =>
            list.id === listId
              ? {
                  ...list,
                  colors: list.colors.map((color) =>
                    color.id === colorId ? { ...color, name } : color
                  ),
                  updatedAt: Date.now(),
                }
              : list
          ),
        }));
      },
    }),
    {
      name: "color-storage",
      storage: createJSONStorage(() => AsyncStorage),
    }
  )
);

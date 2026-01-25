# Mobile App

React Native companion app for the Surface Color Detector. Connects via Bluetooth Low Energy, displays detected colors in real-time, and lets you organize colors into lists.

---

## Features

- **Auto-connect** — Finds and connects to the ESP32 automatically
- **Live color display** — Shows RGB values, HEX code, and color preview
- **Color lists** — Create lists, save colors, manage your palette
- **Dark/light mode** — System-aware theme switching
- **Mock mode** — Works without hardware for UI development

---

## Requirements

- Node.js 18+
- Physical iOS or Android device (BLE doesn't work in simulators)
- Xcode (for iOS) or Android Studio (for Android)

---

## Setup

```bash
npm install
```

**Development build (required for BLE):**
```bash
npx expo run:ios
# or
npx expo run:android
```

Expo Go doesn't support BLE. You need a dev build.

---

## Project Structure

```
app/
├── _layout.tsx          # Root layout with providers
├── index.tsx            # Home screen: connect + view color
├── lists.tsx            # Browse saved color lists
└── list/
    └── [id].tsx         # Single list detail view

components/
└── ui/                  # Button, Card, Text, View, ModeToggle

hooks/
├── useBluetooth.ts      # BLE connection state & logic
├── useColor.ts          # Access theme colors
└── useColorScheme.ts    # Detect system theme

stores/
└── colorStore.ts        # Zustand store for colors & lists

providers/
└── ThemeProvider.tsx    # Theme context

theme/
├── colors.ts            # Color definitions (light/dark)
└── index.ts             # Theme exports
```

---

## BLE Integration

The `useBluetooth` hook handles everything:

```typescript
const {
  isConnected,      // boolean
  isScanning,       // boolean
  currentColor,     // { r, g, b } | null
  error,            // string | null
  scan,             // () => void
  disconnect,       // () => void
} = useBluetooth();
```

**Connection flow:**
1. Call `scan()` to start searching
2. App filters for devices advertising `SERVICE_UUID`
3. Auto-connects when device name matches "Surface Color Detector"
4. Subscribes to characteristic notifications
5. Parses incoming data: `"255,128,64,ORANGE"` → `{ r: 255, g: 128, b: 64 }`

**UUIDs (must match ESP32):**
```typescript
SERVICE_UUID        = "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
CHARACTERISTIC_UUID = "beb5483e-36e1-4688-b7f5-ea07361b26a8"
```

---

## Mock Mode

When BLE isn't available (Expo Go, web, simulator), the app automatically enables mock mode:

- Simulates connection after 2 seconds
- Generates random colors every 2 seconds
- Full UI remains functional

Useful for design iteration without hardware.

---

## State Management

Uses Zustand with persist middleware:

```typescript
const { lists, selectedListId, addColor, createList } = useColorStore();
```

**Store structure:**
```typescript
{
  lists: [
    {
      id: "uuid",
      name: "My Colors",
      colors: [
        { id: "uuid", r: 255, g: 128, b: 64, createdAt: timestamp }
      ]
    }
  ],
  selectedListId: "uuid" | null
}
```

Data persists across app restarts via AsyncStorage.

---

## Theming

Access semantic colors with the `useColor` hook:

```typescript
const bg = useColor("background");
const text = useColor("text");
const primary = useColor("primary");
```

Available tokens: `background`, `card`, `text`, `border`, `primary`, `muted`, `mutedForeground`, `red`, `green`

The theme responds to system preferences and can be toggled manually.

---

## Scripts

| Command | Description |
|---------|-------------|
| `npm start` | Start Expo dev server |
| `npm run ios` | Build and run on iOS |
| `npm run android` | Build and run on Android |
| `npm run lint` | Run ESLint |

---

## Troubleshooting

**"Device not found"**
- ESP32 powered on?
- Serial monitor shows "Waiting for connections..."?
- Phone Bluetooth enabled?

**Connection drops frequently**
- Move closer to device
- Check for BLE interference from other devices
- Verify ESP32 power supply is stable

**iOS build fails**
```bash
cd ios
pod install --repo-update
cd ..
npx expo run:ios
```

**Android permission denied**
Android 12+ requires runtime permissions for BLE. The app requests them, but if denied:
1. Open Settings → Apps → Surface Color Detector
2. Enable Location and Nearby Devices permissions

**Expo Go shows errors**
BLE requires native code. Use `npx expo run:ios` or `npx expo run:android` to create a dev build.

import { useColor } from "@/hooks/useColor";
import React, { useCallback, useEffect, useRef } from "react";
import { StyleProp, ViewStyle } from "react-native";
import { Gesture, GestureDetector } from "react-native-gesture-handler";
import Animated, {
  runOnJS,
  useAnimatedStyle,
  useSharedValue,
  withSpring,
  WithSpringConfig,
} from "react-native-reanimated";

// ============================================
// Types
// ============================================

export type JoystickDirection =
  | "center"
  | "up"
  | "down"
  | "left"
  | "right"
  | "up-left"
  | "up-right"
  | "down-left"
  | "down-right";

export type JoystickPosition = {
  /** Raw X position (-maxDistance to maxDistance) */
  x: number;
  /** Raw Y position (-maxDistance to maxDistance) */
  y: number;
  /** Normalized X (-1 to 1) */
  normalizedX: number;
  /** Normalized Y (-1 to 1, positive is up) */
  normalizedY: number;
  /** Distance from center (0 to 1) */
  distance: number;
  /** Angle in radians */
  angle: number;
  /** Angle in degrees (0-360, 0 is right, 90 is up) */
  angleDegrees: number;
  /** Current direction */
  direction: JoystickDirection;
};

export type JoystickMoveEvent = JoystickPosition & {
  /** Timestamp of the event */
  timestamp: number;
};

export type JoystickProps = {
  /** Size of the joystick base (default: 200) */
  size?: number;
  /** Size of the knob (default: 80) */
  knobSize?: number;
  /** Whether the joystick is disabled (default: false) */
  disabled?: boolean;
  /** Whether to show crosshair lines (default: true) */
  showCrosshair?: boolean;
  /** Whether to allow knob to extend outside boundary (default: false) */
  allowOvershoot?: boolean;
  /** Overshoot percentage (default: 0.1 = 10%) */
  overshootPercent?: number;
  /** Spring configuration for return animation */
  springConfig?: WithSpringConfig;
  /** Throttle interval for onMove events in ms (default: 0 = no throttle) */
  throttleMs?: number;
  /** Dead zone in the center (0-1, default: 0.1) */
  deadZone?: number;
  /** Called when joystick starts moving */
  onStart?: () => void;
  /** Called continuously while moving */
  onMove?: (event: JoystickMoveEvent) => void;
  /** Called when joystick is released */
  onEnd?: (event: JoystickMoveEvent) => void;
  /** Custom style for the container */
  style?: StyleProp<ViewStyle>;
  /** Custom style for the base */
  baseStyle?: StyleProp<ViewStyle>;
  /** Custom style for the knob */
  knobStyle?: StyleProp<ViewStyle>;
  /** Custom base color */
  baseColor?: string;
  /** Custom knob color */
  knobColor?: string;
  /** Custom border color */
  borderColor?: string;
  /** Custom crosshair color */
  crosshairColor?: string;
};

// ============================================
// Helper Functions
// ============================================

function getDirection(
  normalizedX: number,
  normalizedY: number,
  deadZone: number
): JoystickDirection {
  const absX = Math.abs(normalizedX);
  const absY = Math.abs(normalizedY);

  if (absX < deadZone && absY < deadZone) return "center";

  const isUp = normalizedY > deadZone;
  const isDown = normalizedY < -deadZone;
  const isRight = normalizedX > deadZone;
  const isLeft = normalizedX < -deadZone;

  if (isUp && isRight) return "up-right";
  if (isUp && isLeft) return "up-left";
  if (isDown && isRight) return "down-right";
  if (isDown && isLeft) return "down-left";
  if (isUp) return "up";
  if (isDown) return "down";
  if (isRight) return "right";
  if (isLeft) return "left";

  return "center";
}

function calculatePosition(
  x: number,
  y: number,
  maxDistance: number,
  deadZone: number
): JoystickPosition {
  const normalizedX = x / maxDistance;
  const normalizedY = -y / maxDistance; // Invert Y so positive is up

  const distance = Math.min(1, Math.sqrt(normalizedX ** 2 + normalizedY ** 2));
  const angle = Math.atan2(normalizedY, normalizedX);
  const angleDegrees = ((angle * 180) / Math.PI + 360) % 360;

  // Apply dead zone
  const effectiveNormalizedX =
    Math.abs(normalizedX) < deadZone ? 0 : normalizedX;
  const effectiveNormalizedY =
    Math.abs(normalizedY) < deadZone ? 0 : normalizedY;

  return {
    x,
    y,
    normalizedX: effectiveNormalizedX,
    normalizedY: effectiveNormalizedY,
    distance,
    angle,
    angleDegrees,
    direction: getDirection(normalizedX, normalizedY, deadZone),
  };
}

// ============================================
// Component
// ============================================

export function Joystick({
  size = 200,
  knobSize = 80,
  disabled = false,
  showCrosshair = true,
  allowOvershoot = false,
  overshootPercent = 0.1,
  springConfig = { damping: 15, stiffness: 150 },
  throttleMs = 0,
  deadZone = 0.1,
  onStart,
  onMove,
  onEnd,
  style,
  baseStyle,
  knobStyle: customKnobStyle,
  baseColor,
  knobColor,
  borderColor: customBorderColor,
  crosshairColor,
}: JoystickProps) {
  // Colors
  const primary = useColor("primary");
  const border = useColor("border");
  const textMuted = useColor("textMuted");

  const effectiveBaseColor = baseColor ?? border + "30";
  const effectiveKnobColor = knobColor ?? (disabled ? textMuted : primary);
  const effectiveBorderColor =
    customBorderColor ?? (disabled ? border : primary);
  const effectiveCrosshairColor = crosshairColor ?? border;

  // Calculate max distance
  const baseMaxDistance = size / 2 - knobSize / 2;
  const maxDistance = allowOvershoot
    ? baseMaxDistance * (1 + overshootPercent)
    : baseMaxDistance;

  // Shared values
  const translateX = useSharedValue(0);
  const translateY = useSharedValue(0);

  // Throttle ref
  const lastMoveTime = useRef(0);

  // Create move event
  const createMoveEvent = useCallback(
    (x: number, y: number): JoystickMoveEvent => ({
      ...calculatePosition(x, y, maxDistance, deadZone),
      timestamp: Date.now(),
    }),
    [maxDistance, deadZone]
  );

  // Handlers
  const handleStart = useCallback(() => {
    onStart?.();
  }, [onStart]);

  const handleMove = useCallback(
    (x: number, y: number) => {
      if (!onMove) return;

      const now = Date.now();
      if (throttleMs > 0 && now - lastMoveTime.current < throttleMs) {
        return;
      }
      lastMoveTime.current = now;

      onMove(createMoveEvent(x, y));
    },
    [onMove, throttleMs, createMoveEvent]
  );

  const handleEnd = useCallback(() => {
    onEnd?.(createMoveEvent(0, 0));
  }, [onEnd, createMoveEvent]);

  // Pan gesture
  const panGesture = Gesture.Pan()
    .enabled(!disabled)
    .onBegin(() => {
      "worklet";
      runOnJS(handleStart)();
    })
    .onUpdate((event) => {
      "worklet";
      let x = event.translationX;
      let y = event.translationY;

      // Limit to circle boundary
      const distance = Math.sqrt(x * x + y * y);
      if (distance > maxDistance) {
        x = (x / distance) * maxDistance;
        y = (y / distance) * maxDistance;
      }

      translateX.value = x;
      translateY.value = y;

      runOnJS(handleMove)(x, y);
    })
    .onEnd(() => {
      "worklet";
      translateX.value = withSpring(0, springConfig);
      translateY.value = withSpring(0, springConfig);

      runOnJS(handleEnd)();
    });

  // Animated styles
  const knobAnimatedStyle = useAnimatedStyle(() => ({
    transform: [
      { translateX: translateX.value },
      { translateY: translateY.value },
    ],
  }));

  // Reset position when disabled changes
  useEffect(() => {
    if (disabled) {
      translateX.value = withSpring(0, springConfig);
      translateY.value = withSpring(0, springConfig);
    }
  }, [disabled, translateX, translateY, springConfig]);

  return (
    <GestureDetector gesture={panGesture}>
      <Animated.View
        style={[
          {
            width: size,
            height: size,
            borderRadius: size / 2,
            backgroundColor: effectiveBaseColor,
            borderWidth: 2,
            borderColor: effectiveBorderColor,
            alignItems: "center",
            justifyContent: "center",
            opacity: disabled ? 0.5 : 1,
          },
          baseStyle,
          style,
        ]}
      >
        {/* Crosshair lines */}
        {showCrosshair && (
          <>
            <Animated.View
              style={{
                position: "absolute",
                width: 1,
                height: size - 40,
                backgroundColor: effectiveCrosshairColor,
              }}
            />
            <Animated.View
              style={{
                position: "absolute",
                width: size - 40,
                height: 1,
                backgroundColor: effectiveCrosshairColor,
              }}
            />
          </>
        )}

        {/* Knob */}
        <Animated.View
          style={[
            {
              width: knobSize,
              height: knobSize,
              borderRadius: knobSize / 2,
              backgroundColor: effectiveKnobColor,
              shadowColor: "#000",
              shadowOffset: { width: 0, height: 2 },
              shadowOpacity: 0.25,
              shadowRadius: 3.84,
              elevation: 5,
            },
            knobAnimatedStyle,
            customKnobStyle,
          ]}
        />
      </Animated.View>
    </GestureDetector>
  );
}

// ============================================
// Utility Hooks
// ============================================

/**
 * Convert joystick position to differential motor values (left/right)
 * Used for robots with two independent motors
 */
export function useMotorValues(position: JoystickPosition | null): {
  left: number;
  right: number;
} {
  if (!position) return { left: 0, right: 0 };

  const { normalizedX, normalizedY } = position;

  // Differential drive calculation
  let left = normalizedY + normalizedX;
  let right = normalizedY - normalizedX;

  // Clamp values
  left = Math.max(-1, Math.min(1, left));
  right = Math.max(-1, Math.min(1, right));

  return {
    left: Math.round(left * 100),
    right: Math.round(right * 100),
  };
}

/** @deprecated Use useMotorValues instead */
export const useTankDrive = useMotorValues;

/**
 * Convert joystick position to speed/turn values
 * Used for arcade-style control
 */
export function useArcadeDrive(position: JoystickPosition | null): {
  speed: number;
  turn: number;
} {
  if (!position) return { speed: 0, turn: 0 };

  return {
    speed: Math.round(position.normalizedY * 100),
    turn: Math.round(position.normalizedX * 100),
  };
}

export default Joystick;

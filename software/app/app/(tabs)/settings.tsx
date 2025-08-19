import React, { useState } from "react";
import {
  View,
  Text,
  TouchableOpacity,
  Switch,
  Alert,
  Animated,
  ScrollView,
} from "react-native";
import { useTheme } from "../../contexts/ThemeContext";

export default function SettingsScreen() {
  const { theme, setPrimaryColor, updateTheme } = useTheme();
  const [notifications, setNotifications] = useState(true);
  const [autoConnect, setAutoConnect] = useState(true);
  const [powerSaving, setPowerSaving] = useState(false);
  const fadeAnim = new Animated.Value(0);

  React.useEffect(() => {
    Animated.timing(fadeAnim, {
      toValue: 1,
      duration: 500,
      useNativeDriver: true,
    }).start();
  }, []);

  const handleThemeChange = (color: "orange" | "purple") => {
    setPrimaryColor(color);
    Alert.alert("Theme Updated", `Theme changed to ${color} mode!`);
  };

  const handlePowerSavingChange = (value: boolean) => {
    setPowerSaving(value);
    updateTheme({ secondaryColor: value ? "#1a1a1a" : "#2a2a2a" });
  };

  const settingsSections = [
    {
      title: "Appearance",
      items: [
        {
          title: "Primary Color",
          type: "theme-selector",
          value: theme.primary,
        },
        {
          title: "Dark Mode",
          type: "switch",
          value: true,
          disabled: true,
        },
      ],
    },
    {
      title: "Notifications",
      items: [
        {
          title: "Push Notifications",
          type: "switch",
          value: notifications,
          onValueChange: setNotifications,
        },
        {
          title: "Lantern Alerts",
          type: "switch",
          value: true,
        },
        {
          title: "Friend Requests",
          type: "switch",
          value: true,
        },
      ],
    },
    {
      title: "Lantern Settings",
      items: [
        {
          title: "Auto-connect to Lanterns",
          type: "switch",
          value: autoConnect,
          onValueChange: setAutoConnect,
        },
        {
          title: "Power Saving Mode",
          type: "switch",
          value: powerSaving,
          onValueChange: handlePowerSavingChange,
        },
        {
          title: "Default Brightness",
          type: "slider",
          value: "80%",
        },
      ],
    },
    {
      title: "Privacy & Security",
      items: [
        {
          title: "Location Services",
          type: "switch",
          value: false,
        },
        {
          title: "Analytics",
          type: "switch",
          value: true,
        },
        {
          title: "Crash Reports",
          type: "switch",
          value: true,
        },
      ],
    },
  ];

  const renderSettingItem = (item: any, index: number) => {
    switch (item.type) {
      case "switch":
        return (
          <View
            key={index}
            className="flex-row items-center justify-between py-4"
          >
            <Text className="text-white text-lg flex-1">{item.title}</Text>
            <Switch
              value={item.value}
              onValueChange={item.onValueChange}
              disabled={item.disabled}
              trackColor={{ false: "#3a3a3a", true: theme.primaryColor }}
              thumbColor={item.value ? "#ffffff" : "#666666"}
            />
          </View>
        );

      case "theme-selector":
        return (
          <View key={index} className="py-4">
            <Text className="text-white text-lg mb-3">{item.title}</Text>
            <View className="flex-row space-x-4">
              <TouchableOpacity
                className={`px-4 py-2 rounded-lg border-2 ${
                  theme.primary === "orange"
                    ? "border-orange-accent"
                    : "border-gray-600"
                }`}
                onPress={() => handleThemeChange("orange")}
              >
                <View className="flex-row items-center">
                  <View className="w-4 h-4 rounded-full bg-orange-accent mr-2" />
                  <Text className="text-white">Orange</Text>
                </View>
              </TouchableOpacity>

              <TouchableOpacity
                className={`px-4 py-2 rounded-lg border-2 ${
                  theme.primary === "purple"
                    ? "border-purple-accent"
                    : "border-gray-600"
                }`}
                onPress={() => handleThemeChange("purple")}
              >
                <View className="flex-row items-center">
                  <View className="w-4 h-4 rounded-full bg-purple-accent mr-2" />
                  <Text className="text-white">Purple</Text>
                </View>
              </TouchableOpacity>
            </View>
          </View>
        );

      case "slider":
        return (
          <View
            key={index}
            className="flex-row items-center justify-between py-4"
          >
            <Text className="text-white text-lg">{item.title}</Text>
            <Text className="text-gray-400">{item.value}</Text>
          </View>
        );

      default:
        return (
          <View key={index} className="py-4">
            <Text className="text-white text-lg">{item.title}</Text>
          </View>
        );
    }
  };

  return (
    <ScrollView className="flex-1 bg-dark-grey">
      {/* Header */}
      <View className="pt-12 pb-6 px-6 bg-dark-grey-light">
        <Animated.View style={{ opacity: fadeAnim }}>
          <Text className="text-3xl font-bold text-white mb-2">Settings</Text>
          <Text className="text-gray-400">Customize your Luma experience</Text>
        </Animated.View>
      </View>

      {/* Settings Sections */}
      <View className="px-6">
        <Animated.View style={{ opacity: fadeAnim }}>
          {settingsSections.map((section, sectionIndex) => (
            <View key={sectionIndex} className="mb-6">
              <Text className="text-xl font-semibold text-white mb-4 px-2">
                {section.title}
              </Text>
              <View className="bg-dark-grey-light rounded-2xl p-4">
                {section.items.map((item, itemIndex) =>
                  renderSettingItem(item, itemIndex)
                )}
              </View>
            </View>
          ))}
        </Animated.View>
      </View>

      {/* Version Info */}
      <View className="px-6 mb-8">
        <Animated.View style={{ opacity: fadeAnim }}>
          <View className="bg-dark-grey-light rounded-2xl p-4 items-center">
            <Text className="text-gray-400 text-sm">Luma v1.0.0</Text>
            <Text className="text-gray-400 text-xs mt-1">
              Illuminate Your World
            </Text>
          </View>
        </Animated.View>
      </View>
    </ScrollView>
  );
}

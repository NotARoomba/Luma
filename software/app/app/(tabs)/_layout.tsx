import React, { useState, useEffect } from "react";
import { View, Text, TouchableOpacity, Animated } from "react-native";
import { Tabs } from "expo-router";
import { useTheme } from "../../contexts/ThemeContext";
import { useAuth } from "../../contexts/AuthContext";

const tabItems = [
  { name: "index", title: "Lanterns", icon: "💡" },
  { name: "wifi-config", title: "WiFi", icon: "📶" },
  { name: "profile", title: "Profile", icon: "👤" },
  { name: "settings", title: "Settings", icon: "⚙️" },
];

export default function TabLayout() {
  const { theme } = useTheme();
  const { user } = useAuth();
  const [activeTab, setActiveTab] = useState(0);
  const [indicatorPos, setIndicatorPos] = useState(0);
  const [indicatorWidth, setIndicatorWidth] = useState(0);

  useEffect(() => {
    // Calculate initial indicator position
    const tabWidth = 100; // Approximate tab width
    setIndicatorWidth(tabWidth);
    setIndicatorPos(0);
  }, []);

  const handleTabPress = (index: number) => {
    setActiveTab(index);
    const tabWidth = 100; // Approximate tab width
    setIndicatorPos(index * tabWidth);
  };

  if (!user) {
    return null; // Redirect to auth
  }

  return (
    <View className="flex-1 bg-dark-grey">
      <Tabs
        screenOptions={{
          headerShown: false,
          tabBarStyle: {
            backgroundColor: "transparent",
            borderTopWidth: 0,
            elevation: 0,
            shadowOpacity: 0,
            height: 80,
            paddingBottom: 20,
          },
          tabBarShowLabel: false,
        }}
      >
        <Tabs.Screen
          name="index"
          options={{
            title: "Lanterns",
            tabBarIcon: ({ focused }) => (
              <TouchableOpacity
                onPress={() => handleTabPress(0)}
                className="items-center justify-center w-20 h-12"
              >
                <Text className="text-2xl">{tabItems[0].icon}</Text>
                <Text
                  className={`text-xs mt-1 ${
                    focused ? "text-white" : "text-gray-400"
                  }`}
                >
                  {tabItems[0].title}
                </Text>
              </TouchableOpacity>
            ),
          }}
        />
        <Tabs.Screen
          name="wifi-config"
          options={{
            title: "WiFi",
            tabBarIcon: ({ focused }) => (
              <TouchableOpacity
                onPress={() => handleTabPress(1)}
                className="items-center justify-center w-20 h-12"
              >
                <Text className="text-2xl">{tabItems[1].icon}</Text>
                <Text
                  className={`text-xs mt-1 ${
                    focused ? "text-white" : "text-gray-400"
                  }`}
                >
                  {tabItems[1].title}
                </Text>
              </TouchableOpacity>
            ),
          }}
        />
        <Tabs.Screen
          name="profile"
          options={{
            title: "Profile",
            tabBarIcon: ({ focused }) => (
              <TouchableOpacity
                onPress={() => handleTabPress(2)}
                className="items-center justify-center w-20 h-12"
              >
                <Text className="text-2xl">{tabItems[2].icon}</Text>
                <Text
                  className={`text-xs mt-1 ${
                    focused ? "text-white" : "text-gray-400"
                  }`}
                >
                  {tabItems[2].title}
                </Text>
              </TouchableOpacity>
            ),
          }}
        />
        <Tabs.Screen
          name="settings"
          options={{
            title: "Settings",
            tabBarIcon: ({ focused }) => (
              <TouchableOpacity
                onPress={() => handleTabPress(3)}
                className="items-center justify-center w-20 h-12"
              >
                <Text className="text-2xl">{tabItems[3].icon}</Text>
                <Text
                  className={`text-xs mt-1 ${
                    focused ? "text-white" : "text-gray-400"
                  }`}
                >
                  {tabItems[3].title}
                </Text>
              </TouchableOpacity>
            ),
          }}
        />
      </Tabs>

      {/* Custom Bottom Navigation Indicator */}
      <View className="absolute bottom-0 left-0 right-0 h-20 bg-dark-grey-light rounded-t-3xl">
        <View className="flex-row justify-around items-center h-full px-4">
          {tabItems.map((item, index) => (
            <TouchableOpacity
              key={item.name}
              onPress={() => handleTabPress(index)}
              className="items-center justify-center w-20 h-12"
            >
              <Text className="text-2xl">{item.icon}</Text>
              <Text
                className={`text-xs mt-1 ${
                  activeTab === index ? "text-white" : "text-gray-400"
                }`}
              >
                {item.title}
              </Text>
            </TouchableOpacity>
          ))}
        </View>

        {/* Animated Indicator */}
        <Animated.View
          className="absolute bottom-0 h-1 rounded-full"
          style={{
            backgroundColor: theme.primaryColor,
            width: indicatorWidth,
            left: indicatorPos + 20, // Adjust for padding
            transform: [{ translateX: 0 }],
          }}
        />
      </View>
    </View>
  );
}

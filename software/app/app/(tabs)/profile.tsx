import React, { useState } from "react";
import {
  View,
  Text,
  TouchableOpacity,
  Alert,
  Animated,
  ScrollView,
} from "react-native";
import { useTheme } from "../../contexts/ThemeContext";
import { useAuth } from "../../contexts/AuthContext";

export default function ProfileScreen() {
  const { theme } = useTheme();
  const { user, logout } = useAuth();
  const [loading, setLoading] = useState(false);
  const fadeAnim = new Animated.Value(0);

  React.useEffect(() => {
    Animated.timing(fadeAnim, {
      toValue: 1,
      duration: 500,
      useNativeDriver: true,
    }).start();
  }, []);

  const handleLogout = async () => {
    Alert.alert("Logout", "Are you sure you want to logout?", [
      {
        text: "Cancel",
        style: "cancel",
      },
      {
        text: "Logout",
        style: "destructive",
        onPress: async () => {
          setLoading(true);
          try {
            await logout();
          } catch (error) {
            Alert.alert("Error", "Failed to logout");
          } finally {
            setLoading(false);
          }
        },
      },
    ]);
  };

  const menuItems = [
    {
      title: "Account Settings",
      icon: "⚙️",
      onPress: () => Alert.alert("Account Settings", "Coming soon!"),
    },
    {
      title: "Privacy & Security",
      icon: "🔒",
      onPress: () => Alert.alert("Privacy & Security", "Coming soon!"),
    },
    {
      title: "Notifications",
      icon: "🔔",
      onPress: () => Alert.alert("Notifications", "Coming soon!"),
    },
    {
      title: "Help & Support",
      icon: "❓",
      onPress: () => Alert.alert("Help & Support", "Coming soon!"),
    },
    {
      title: "About Luma",
      icon: "ℹ️",
      onPress: () =>
        Alert.alert("About Luma", "Version 1.0.0\nIlluminate Your World"),
    },
  ];

  return (
    <ScrollView className="flex-1 bg-dark-grey">
      {/* Header */}
      <View className="pt-12 pb-8 px-6 bg-dark-grey-light">
        <Animated.View style={{ opacity: fadeAnim }}>
          <View className="items-center mb-6">
            <View
              className="w-24 h-24 rounded-full mb-4"
              style={{ backgroundColor: theme.primaryColor }}
            >
              <View className="flex-1 justify-center items-center">
                <Text className="text-4xl">👤</Text>
              </View>
            </View>
            <Text className="text-2xl font-bold text-white mb-2">
              {user?.displayName || "User"}
            </Text>
            <Text className="text-gray-400">{user?.email}</Text>
          </View>
        </Animated.View>
      </View>

      {/* Stats */}
      <View className="px-6 mb-6">
        <Animated.View style={{ opacity: fadeAnim }}>
          <View className="bg-dark-grey-light rounded-2xl p-4">
            <Text className="text-lg font-semibold text-white mb-4">
              Your Stats
            </Text>
            <View className="flex-row justify-around">
              <View className="items-center">
                <Text className="text-2xl font-bold text-white">5</Text>
                <Text className="text-gray-400 text-sm">Lanterns</Text>
              </View>
              <View className="items-center">
                <Text className="text-2xl font-bold text-white">12</Text>
                <Text className="text-gray-400 text-sm">Friends</Text>
              </View>
              <View className="items-center">
                <Text className="text-2xl font-bold text-white">89</Text>
                <Text className="text-gray-400 text-sm">Hours</Text>
              </View>
            </View>
          </View>
        </Animated.View>
      </View>

      {/* Menu Items */}
      <View className="px-6 mb-6">
        <Animated.View style={{ opacity: fadeAnim }}>
          {menuItems.map((item, index) => (
            <TouchableOpacity
              key={index}
              className="bg-dark-grey-light rounded-xl p-4 mb-3 flex-row items-center"
              onPress={item.onPress}
            >
              <Text className="text-2xl mr-4">{item.icon}</Text>
              <Text className="text-white text-lg flex-1">{item.title}</Text>
              <Text className="text-gray-400">›</Text>
            </TouchableOpacity>
          ))}
        </Animated.View>
      </View>

      {/* Logout Button */}
      <View className="px-6 mb-8">
        <Animated.View style={{ opacity: fadeAnim }}>
          <TouchableOpacity
            className="bg-red-600 rounded-xl p-4 items-center"
            onPress={handleLogout}
            disabled={loading}
          >
            <Text className="text-white text-lg font-semibold">
              {loading ? "Logging out..." : "Logout"}
            </Text>
          </TouchableOpacity>
        </Animated.View>
      </View>
    </ScrollView>
  );
}

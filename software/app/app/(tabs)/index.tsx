import React, { useState, useEffect } from "react";
import {
  View,
  Text,
  TouchableOpacity,
  FlatList,
  Alert,
  Animated,
} from "react-native";
import { useTheme } from "../../contexts/ThemeContext";
import { useAuth } from "../../contexts/AuthContext";
import { Lantern } from "../../types";

export default function LanternsScreen() {
  const { theme } = useTheme();
  const { user } = useAuth();
  const [lanterns, setLanterns] = useState<Lantern[]>([]);
  const [loading, setLoading] = useState(false);
  const fadeAnim = new Animated.Value(0);

  useEffect(() => {
    Animated.timing(fadeAnim, {
      toValue: 1,
      duration: 500,
      useNativeDriver: true,
    }).start();

    // Load user's lanterns from Firebase
    loadLanterns();
  }, []);

  const loadLanterns = async () => {
    // TODO: Implement Firebase loading
    // For now, using mock data
    const mockLanterns: Lantern[] = [
      {
        id: "1",
        name: "Living Room Lamp",
        uuid: "uuid-1",
        ownerId: user?.uid || "",
        color: "#ff6b35",
        brightness: 80,
        isConnected: true,
        isOnline: true,
        lastSeen: new Date(),
        friends: [],
        settings: {
          autoConnect: true,
          brightness: 80,
          colorMode: "solid",
          transitionSpeed: 1,
          powerSaving: false,
        },
        createdAt: new Date(),
        updatedAt: new Date(),
      },
    ];
    setLanterns(mockLanterns);
  };

  const addNewLantern = () => {
    Alert.alert("Add New Lantern", "Choose how to add a lantern", [
      {
        text: "Scan Bluetooth",
        onPress: () => scanBluetooth(),
      },
      {
        text: "Scan QR Code",
        onPress: () => scanQRCode(),
      },
      {
        text: "Enter UUID",
        onPress: () => enterUUID(),
      },
      {
        text: "Cancel",
        style: "cancel",
      },
    ]);
  };

  const scanBluetooth = () => {
    // TODO: Implement Bluetooth scanning
    Alert.alert("Bluetooth Scanning", "This feature will be implemented soon!");
  };

  const scanQRCode = () => {
    // TODO: Implement QR code scanning
    Alert.alert("QR Code Scanning", "This feature will be implemented soon!");
  };

  const enterUUID = () => {
    // TODO: Implement manual UUID entry
    Alert.alert("Manual Entry", "This feature will be implemented soon!");
  };

  const renderLantern = ({ item }: { item: Lantern }) => (
    <Animated.View
      style={{ opacity: fadeAnim }}
      className="bg-dark-grey-light rounded-2xl p-4 mb-4 mx-4"
    >
      <View className="flex-row items-center justify-between">
        <View className="flex-1">
          <Text className="text-xl font-bold text-white mb-2">{item.name}</Text>
          <View className="flex-row items-center space-x-4">
            <View className="flex-row items-center">
              <View
                className={`w-3 h-3 rounded-full mr-2 ${
                  item.isOnline ? "bg-green-500" : "bg-red-500"
                }`}
              />
              <Text className="text-gray-400">
                {item.isOnline ? "Online" : "Offline"}
              </Text>
            </View>
            <Text className="text-gray-400">
              Brightness: {item.brightness}%
            </Text>
          </View>
        </View>

        <View
          className="w-16 h-16 rounded-full mr-4"
          style={{ backgroundColor: item.color }}
        >
          <View className="flex-1 justify-center items-center">
            <Text className="text-2xl">💡</Text>
          </View>
        </View>
      </View>

      <View className="flex-row justify-between mt-4">
        <TouchableOpacity
          className="bg-dark-grey-lighter px-4 py-2 rounded-lg"
          onPress={() => editLantern(item)}
        >
          <Text className="text-white font-semibold">Edit</Text>
        </TouchableOpacity>

        <TouchableOpacity
          className="bg-dark-grey-lighter px-4 py-2 rounded-lg"
          onPress={() => controlLantern(item)}
        >
          <Text className="text-white font-semibold">Control</Text>
        </TouchableOpacity>

        <TouchableOpacity
          className="bg-dark-grey-lighter px-4 py-2 rounded-lg"
          onPress={() => shareLantern(item)}
        >
          <Text className="text-white font-semibold">Share</Text>
        </TouchableOpacity>
      </View>
    </Animated.View>
  );

  const editLantern = (lantern: Lantern) => {
    // TODO: Navigate to edit screen
    Alert.alert("Edit Lantern", `Edit ${lantern.name}`);
  };

  const controlLantern = (lantern: Lantern) => {
    // TODO: Navigate to control screen
    Alert.alert("Control Lantern", `Control ${lantern.name}`);
  };

  const shareLantern = (lantern: Lantern) => {
    // TODO: Show sharing options
    Alert.alert("Share Lantern", `Share ${lantern.name} with friends`);
  };

  return (
    <View className="flex-1 bg-dark-grey">
      {/* Header */}
      <View className="pt-12 pb-6 px-6 bg-dark-grey-light">
        <Text className="text-3xl font-bold text-white mb-2">My Lanterns</Text>
        <Text className="text-gray-400">
          {lanterns.length} lantern{lanterns.length !== 1 ? "s" : ""} connected
        </Text>
      </View>

      {/* Lantern List */}
      {lanterns.length > 0 ? (
        <FlatList
          data={lanterns}
          renderItem={renderLantern}
          keyExtractor={(item) => item.id}
          contentContainerStyle={{ paddingBottom: 100 }}
          showsVerticalScrollIndicator={false}
        />
      ) : (
        <View className="flex-1 justify-center items-center px-8">
          <View
            className="w-32 h-32 rounded-full mb-6 opacity-50"
            style={{ backgroundColor: theme.primaryColor }}
          >
            <View className="flex-1 justify-center items-center">
              <Text className="text-6xl">💡</Text>
            </View>
          </View>
          <Text className="text-2xl font-bold text-white mb-4 text-center">
            No Lanterns Yet
          </Text>
          <Text className="text-gray-400 text-center mb-8 text-lg">
            Add your first lantern to get started with smart lighting control
          </Text>
        </View>
      )}

      {/* Add Lantern Button */}
      <TouchableOpacity
        className="absolute bottom-24 right-6 w-16 h-16 rounded-full items-center justify-center shadow-lg"
        style={{ backgroundColor: theme.primaryColor }}
        onPress={addNewLantern}
      >
        <Text className="text-3xl text-white">+</Text>
      </TouchableOpacity>
    </View>
  );
}

import React, { useState } from "react";
import {
  View,
  Text,
  TextInput,
  TouchableOpacity,
  Alert,
  Animated,
  ScrollView,
} from "react-native";
import { useTheme } from "../../contexts/ThemeContext";

export default function WifiConfigScreen() {
  const { theme } = useTheme();
  const [ssid, setSsid] = useState("");
  const [password, setPassword] = useState("");
  const [loading, setLoading] = useState(false);
  const fadeAnim = new Animated.Value(0);

  React.useEffect(() => {
    Animated.timing(fadeAnim, {
      toValue: 1,
      duration: 500,
      useNativeDriver: true,
    }).start();
  }, []);

  const handleWifiUpdate = async () => {
    if (!ssid || !password) {
      Alert.alert("Error", "Please fill in both SSID and password");
      return;
    }

    setLoading(true);
    try {
      // TODO: Send WiFi credentials to ESP32 via Bluetooth
      const wifiConfig = {
        type: "wifi_config",
        wifi_ssid: ssid,
        wifi_password: password,
        timestamp: Date.now(),
      };

      // This would be sent via Bluetooth to the ESP32
      console.log("Sending WiFi config:", wifiConfig);

      Alert.alert(
        "WiFi Configuration Sent",
        "The lantern will attempt to connect to the new WiFi network. This may take a few moments.",
        [
          {
            text: "OK",
            onPress: () => {
              setSsid("");
              setPassword("");
            },
          },
        ]
      );
    } catch (error) {
      Alert.alert("Error", "Failed to send WiFi configuration");
    } finally {
      setLoading(false);
    }
  };

  const scanForNetworks = () => {
    // TODO: Implement WiFi network scanning
    Alert.alert(
      "WiFi Scanning",
      "This feature will scan for available WiFi networks"
    );
  };

  return (
    <ScrollView className="flex-1 bg-dark-grey">
      {/* Header */}
      <View className="pt-12 pb-6 px-6 bg-dark-grey-light">
        <Animated.View style={{ opacity: fadeAnim }}>
          <Text className="text-3xl font-bold text-white mb-2">
            WiFi Configuration
          </Text>
          <Text className="text-gray-400">
            Update your lantern's WiFi connection
          </Text>
        </Animated.View>
      </View>

      {/* Current Status */}
      <View className="px-6 mb-6">
        <Animated.View style={{ opacity: fadeAnim }}>
          <View className="bg-dark-grey-light rounded-2xl p-4">
            <Text className="text-lg font-semibold text-white mb-4">
              Current Status
            </Text>
            <View className="space-y-2">
              <View className="flex-row items-center">
                <View className="w-3 h-3 rounded-full bg-green-500 mr-3" />
                <Text className="text-white">Connected to WiFi</Text>
              </View>
              <Text className="text-gray-400 ml-6">Network: Home_WiFi_5G</Text>
              <Text className="text-gray-400 ml-6">Signal: Strong</Text>
              <Text className="text-gray-400 ml-6">IP: 192.168.1.100</Text>
            </View>
          </View>
        </Animated.View>
      </View>

      {/* WiFi Configuration Form */}
      <View className="px-6 mb-6">
        <Animated.View style={{ opacity: fadeAnim }}>
          <View className="bg-dark-grey-light rounded-2xl p-4">
            <Text className="text-lg font-semibold text-white mb-4">
              New WiFi Network
            </Text>

            <View className="space-y-4">
              <View>
                <Text className="text-white text-sm mb-2">
                  Network Name (SSID)
                </Text>
                <TextInput
                  className="bg-dark-grey-lighter rounded-lg px-4 py-3 text-white border border-gray-600"
                  placeholder="Enter WiFi network name"
                  placeholderTextColor="#666"
                  value={ssid}
                  onChangeText={setSsid}
                  autoCapitalize="none"
                />
              </View>

              <View>
                <Text className="text-white text-sm mb-2">Password</Text>
                <TextInput
                  className="bg-dark-grey-lighter rounded-lg px-4 py-3 text-white border border-gray-600"
                  placeholder="Enter WiFi password"
                  placeholderTextColor="#666"
                  value={password}
                  onChangeText={setPassword}
                  secureTextEntry
                />
              </View>

              <TouchableOpacity
                className="bg-blue-600 rounded-lg py-3 items-center mb-3"
                onPress={scanForNetworks}
              >
                <Text className="text-white font-semibold">
                  Scan for Networks
                </Text>
              </TouchableOpacity>

              <TouchableOpacity
                className="py-3 rounded-lg items-center"
                style={{ backgroundColor: theme.primaryColor }}
                onPress={handleWifiUpdate}
                disabled={loading}
              >
                <Text className="text-white font-semibold text-lg">
                  {loading ? "Updating..." : "Update WiFi Configuration"}
                </Text>
              </TouchableOpacity>
            </View>
          </View>
        </Animated.View>
      </View>

      {/* Instructions */}
      <View className="px-6 mb-8">
        <Animated.View style={{ opacity: fadeAnim }}>
          <View className="bg-dark-grey-light rounded-2xl p-4">
            <Text className="text-lg font-semibold text-white mb-4">
              Instructions
            </Text>
            <View className="space-y-2">
              <Text className="text-gray-400 text-sm">
                1. Make sure your lantern is in Bluetooth range
              </Text>
              <Text className="text-gray-400 text-sm">
                2. Enter the WiFi network name and password
              </Text>
              <Text className="text-gray-400 text-sm">
                3. Tap "Update WiFi Configuration"
              </Text>
              <Text className="text-gray-400 text-sm">
                4. The lantern will attempt to connect to the new network
              </Text>
              <Text className="text-gray-400 text-sm">
                5. You can monitor the connection status in the main app
              </Text>
            </View>
          </View>
        </Animated.View>
      </View>
    </ScrollView>
  );
}

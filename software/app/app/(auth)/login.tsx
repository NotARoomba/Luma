import { router } from "expo-router";
import React, { useState } from "react";
import { Alert, Text, TextInput, TouchableOpacity, View } from "react-native";
import { useAuth } from "../../contexts/AuthContext";
import { useTheme } from "../../contexts/ThemeContext";

export default function LoginScreen() {
  const [email, setEmail] = useState("");
  const [password, setPassword] = useState("");
  const [loading, setLoading] = useState(false);
  const { signIn } = useAuth();
  const { theme } = useTheme();

  const handleLogin = async () => {
    if (!email || !password) {
      Alert.alert("Error", "Please fill in all fields");
      return;
    }

    setLoading(true);
    try {
      await signIn(email, password);
      router.replace("/(tabs)");
    } catch (error: any) {
      Alert.alert("Login Failed", error.message);
    } finally {
      setLoading(false);
    }
  };

  return (
    <View className="flex-1 bg-dark-grey justify-center px-8">
      <View className="items-center mb-12">
        <View
          className="w-24 h-24 rounded-full mb-6"
          style={{ backgroundColor: theme.primaryColor }}
        >
          <View className="flex-1 justify-center items-center">
            <Text className="text-4xl">💡</Text>
          </View>
        </View>
        <Text className="text-3xl font-bold text-white mb-2">Welcome Back</Text>
        <Text className="text-gray-400 text-center">
          Sign in to control your lanterns
        </Text>
      </View>

      <View className="space-y-4">
        <View>
          <Text className="text-white text-sm mb-2">Email</Text>
          <TextInput
            className="bg-dark-grey-light rounded-lg px-4 py-3 text-white border border-gray-600"
            placeholder="Enter your email"
            placeholderTextColor="#666"
            value={email}
            onChangeText={setEmail}
            keyboardType="email-address"
            autoCapitalize="none"
          />
        </View>

        <View>
          <Text className="text-white text-sm mb-2">Password</Text>
          <TextInput
            className="bg-dark-grey-light rounded-lg px-4 py-3 text-white border border-gray-600"
            placeholder="Enter your password"
            placeholderTextColor="#666"
            value={password}
            onChangeText={setPassword}
            secureTextEntry
          />
        </View>

        <TouchableOpacity
          className="py-3 rounded-lg mt-6"
          style={{ backgroundColor: theme.primaryColor }}
          onPress={handleLogin}
          disabled={loading}
        >
          <Text className="text-white text-center font-semibold text-lg">
            {loading ? "Signing In..." : "Sign In"}
          </Text>
        </TouchableOpacity>

        <View className="flex-row justify-center mt-4">
          <Text className="text-gray-400">Don't have an account? </Text>
          <TouchableOpacity onPress={() => router.push("/(auth)/signup")}>
            <Text
              className="font-semibold"
              style={{ color: theme.primaryColor }}
            >
              Sign Up
            </Text>
          </TouchableOpacity>
        </View>

        <View className="flex-row justify-center mt-2">
          <TouchableOpacity
            onPress={() => router.push("/(auth)/reset-password")}
          >
            <Text
              className="text-gray-400 underline"
              style={{ color: theme.primaryColor }}
            >
              Forgot Password?
            </Text>
          </TouchableOpacity>
        </View>
      </View>
    </View>
  );
}

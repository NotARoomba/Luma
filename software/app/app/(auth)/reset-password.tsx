import React, { useState } from "react";
import {
  View,
  Text,
  TextInput,
  TouchableOpacity,
  Alert,
  Animated,
} from "react-native";
import { useAuth } from "../../contexts/AuthContext";
import { useTheme } from "../../contexts/ThemeContext";

export default function ResetPasswordScreen() {
  const [email, setEmail] = useState("");
  const [loading, setLoading] = useState(false);
  const [sent, setSent] = useState(false);
  const { resetPassword } = useAuth();
  const { theme } = useTheme();

  const handleResetPassword = async () => {
    if (!email) {
      Alert.alert("Error", "Please enter your email address");
      return;
    }

    setLoading(true);
    try {
      await resetPassword(email);
      setSent(true);
      Alert.alert("Success", "Password reset email sent! Check your inbox.");
    } catch (error: any) {
      Alert.alert("Reset Failed", error.message);
    } finally {
      setLoading(false);
    }
  };

  if (sent) {
    return (
      <View className="flex-1 bg-dark-grey justify-center px-8">
        <View className="items-center mb-12">
          <View
            className="w-24 h-24 rounded-full mb-6"
            style={{ backgroundColor: theme.primaryColor }}
          >
            <View className="flex-1 justify-center items-center">
              <Text className="text-4xl">📧</Text>
            </View>
          </View>
          <Text className="text-3xl font-bold text-white mb-2">
            Check Your Email
          </Text>
          <Text className="text-gray-400 text-center text-lg">
            We've sent a password reset link to {email}
          </Text>
        </View>

        <TouchableOpacity
          className="py-3 rounded-lg mt-6"
          style={{ backgroundColor: theme.primaryColor }}
          onPress={() => setSent(false)}
        >
          <Text className="text-white text-center font-semibold text-lg">
            Try Another Email
          </Text>
        </TouchableOpacity>
      </View>
    );
  }

  return (
    <View className="flex-1 bg-dark-grey justify-center px-8">
      <View className="items-center mb-12">
        <View
          className="w-24 h-24 rounded-full mb-6"
          style={{ backgroundColor: theme.primaryColor }}
        >
          <View className="flex-1 justify-center items-center">
            <Text className="text-4xl">🔐</Text>
          </View>
        </View>
        <Text className="text-3xl font-bold text-white mb-2">
          Reset Password
        </Text>
        <Text className="text-gray-400 text-center">
          Enter your email to receive a reset link
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

        <TouchableOpacity
          className="py-3 rounded-lg mt-6"
          style={{ backgroundColor: theme.primaryColor }}
          onPress={handleResetPassword}
          disabled={loading}
        >
          <Text className="text-white text-center font-semibold text-lg">
            {loading ? "Sending..." : "Send Reset Link"}
          </Text>
        </TouchableOpacity>

        <View className="flex-row justify-center mt-4">
          <Text className="text-gray-400">Remember your password? </Text>
          <TouchableOpacity>
            <Text
              className="font-semibold"
              style={{ color: theme.primaryColor }}
            >
              Sign In
            </Text>
          </TouchableOpacity>
        </View>
      </View>
    </View>
  );
}

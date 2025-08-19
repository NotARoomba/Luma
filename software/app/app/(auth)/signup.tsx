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

export default function SignupScreen() {
  const [email, setEmail] = useState("");
  const [password, setPassword] = useState("");
  const [confirmPassword, setConfirmPassword] = useState("");
  const [loading, setLoading] = useState(false);
  const { signUp } = useAuth();
  const { theme } = useTheme();

  const handleSignup = async () => {
    if (!email || !password || !confirmPassword) {
      Alert.alert("Error", "Please fill in all fields");
      return;
    }

    if (password !== confirmPassword) {
      Alert.alert("Error", "Passwords do not match");
      return;
    }

    if (password.length < 6) {
      Alert.alert("Error", "Password must be at least 6 characters");
      return;
    }

    setLoading(true);
    try {
      await signUp(email, password);
      Alert.alert("Success", "Account created successfully! Please sign in.");
    } catch (error: any) {
      Alert.alert("Signup Failed", error.message);
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
        <Text className="text-3xl font-bold text-white mb-2">
          Create Account
        </Text>
        <Text className="text-gray-400 text-center">
          Join Luma to control your lanterns
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

        <View>
          <Text className="text-white text-sm mb-2">Confirm Password</Text>
          <TextInput
            className="bg-dark-grey-light rounded-lg px-4 py-3 text-white border border-gray-600"
            placeholder="Confirm your password"
            placeholderTextColor="#666"
            value={confirmPassword}
            onChangeText={setConfirmPassword}
            secureTextEntry
          />
        </View>

        <TouchableOpacity
          className="py-3 rounded-lg mt-6"
          style={{ backgroundColor: theme.primaryColor }}
          onPress={handleSignup}
          disabled={loading}
        >
          <Text className="text-white text-center font-semibold text-lg">
            {loading ? "Creating Account..." : "Create Account"}
          </Text>
        </TouchableOpacity>

        <View className="flex-row justify-center mt-4">
          <Text className="text-gray-400">Already have an account? </Text>
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

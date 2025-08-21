import { useSession } from "@/hooks/useAuth";
import { useRouter } from "expo-router";
import { ArrowLeft, Eye, EyeOff, Lock, Mail } from "lucide-react-native";
import * as React from "react";
import { useState } from "react";
import {
  Alert,
  KeyboardAvoidingView,
  Platform,
  ScrollView,
  Text,
  TextInput,
  TouchableOpacity,
  View,
} from "react-native";

export default function LoginScreen() {
  const [email, setEmail] = useState("");
  const [password, setPassword] = useState("");
  const [showPassword, setShowPassword] = useState(false);
  const [loading, setLoading] = useState(false);
  const { signIn } = useSession();
  const router = useRouter();

  const handleLogin = async () => {
    if (!email || !password) {
      Alert.alert("Error", "Please fill in all fields");
      return;
    }

    setLoading(true);
    try {
      await signIn(email, password);
      // Navigation will be handled automatically by the protected routes
    } catch (error: any) {
      Alert.alert(
        "Login Failed",
        error.message || "An error occurred during login"
      );
    } finally {
      setLoading(false);
    }
  };

  return (
    <KeyboardAvoidingView
      behavior={Platform.OS === "ios" ? "padding" : "height"}
      className="flex-1"
    >
      <ScrollView
        contentContainerStyle={{ flexGrow: 1 }}
        className="flex-1"
        showsVerticalScrollIndicator={false}
      >
        <View className="flex-1 bg-night-500 px-6 pt-16">
          {/* Header */}
          <TouchableOpacity
            onPress={() => router.back()}
            className="flex-row items-center mb-8"
          >
            <ArrowLeft size={24} color="#fff8e6" />
            <Text className="text-cosmic_latte-500 ml-2 text-lg font-medium">
              Back
            </Text>
          </TouchableOpacity>

          {/* Title */}
          <View className="mb-12">
            <Text className="text-cosmic_latte-500 text-4xl font-bold mb-2">
              Welcome Back
            </Text>
            <Text className="text-gunmetal-400 text-lg">
              Sign in to your account
            </Text>
          </View>

          {/* Form */}
          <View className="space-y-6">
            {/* Email Input */}
            <View>
              <Text className="text-cosmic_latte-500 text-sm font-medium mb-2">
                Email
              </Text>
              <View className="relative">
                <Mail
                  size={20}
                  color="#5f81a7"
                  className="absolute left-4 top-3 z-10"
                />
                <TextInput
                  value={email}
                  onChangeText={setEmail}
                  placeholder="Enter your email"
                  placeholderTextColor="#5f81a7"
                  keyboardType="email-address"
                  autoCapitalize="none"
                  autoCorrect={false}
                  className="bg-gunmetal-300 border border-gunmetal-400 rounded-2xl px-12 py-4 text-cosmic_latte-500 text-base"
                />
              </View>
            </View>

            {/* Password Input */}
            <View>
              <Text className="text-cosmic_latte-500 text-sm font-medium mb-2">
                Password
              </Text>
              <View className="relative">
                <Lock
                  size={20}
                  color="#5f81a7"
                  className="absolute left-4 top-3 z-10"
                />
                <TextInput
                  value={password}
                  onChangeText={setPassword}
                  placeholder="Enter your password"
                  placeholderTextColor="#5f81a7"
                  secureTextEntry={!showPassword}
                  autoCapitalize="none"
                  autoCorrect={false}
                  className="bg-gunmetal-300 border border-gunmetal-400 rounded-2xl px-12 py-4 text-cosmic_latte-500 text-base pr-12"
                />
                <TouchableOpacity
                  onPress={() => setShowPassword(!showPassword)}
                  className="absolute right-4 top-3"
                >
                  {showPassword ? (
                    <EyeOff size={20} color="#5f81a7" />
                  ) : (
                    <Eye size={20} color="#5f81a7" />
                  )}
                </TouchableOpacity>
              </View>
            </View>

            {/* Forgot Password */}
            <TouchableOpacity
              onPress={() => router.push("/(auth)/forgot-password")}
              className="self-end"
            >
              <Text className="text-princeton_orange-500 text-sm font-medium">
                Forgot Password?
              </Text>
            </TouchableOpacity>

            {/* Login Button */}
            <TouchableOpacity
              onPress={handleLogin}
              disabled={loading}
              className={`bg-princeton_orange-500 rounded-2xl py-4 mt-6 ${
                loading ? "opacity-50" : ""
              }`}
            >
              <Text className="text-night-500 text-center text-lg font-semibold">
                {loading ? "Signing In..." : "Sign In"}
              </Text>
            </TouchableOpacity>

            {/* Sign Up Link */}
            <View className="flex-row justify-center mt-8">
              <Text className="text-gunmetal-400 text-base">
                Don't have an account?{" "}
              </Text>
              <TouchableOpacity onPress={() => router.push("/(auth)/signup")}>
                <Text className="text-amber-500 text-base font-semibold">
                  Sign Up
                </Text>
              </TouchableOpacity>
            </View>
          </View>
        </View>
      </ScrollView>
    </KeyboardAvoidingView>
  );
}

import { useSession } from "@/hooks/useAuth";
import { useRouter } from "expo-router";
import { ArrowLeft, Eye, EyeOff, Lock, Mail, User } from "lucide-react-native";
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

export default function SignupScreen() {
  const [name, setName] = useState("");
  const [email, setEmail] = useState("");
  const [password, setPassword] = useState("");
  const [confirmPassword, setConfirmPassword] = useState("");
  const [showPassword, setShowPassword] = useState(false);
  const [showConfirmPassword, setShowConfirmPassword] = useState(false);
  const [loading, setLoading] = useState(false);
  const { signUp } = useSession();
  const router = useRouter();

  const handleSignup = async () => {
    if (!name || !email || !password || !confirmPassword) {
      Alert.alert("Error", "Please fill in all fields");
      return;
    }

    if (password !== confirmPassword) {
      Alert.alert("Error", "Passwords do not match");
      return;
    }

    if (password.length < 6) {
      Alert.alert("Error", "Password must be at least 6 characters long");
      return;
    }

    setLoading(true);
    try {
      await signUp(email, password);
      Alert.alert("Success", "Account created successfully!", [
        {
          text: "OK",
          onPress: () => {
            // Navigation will be handled automatically by the protected routes
          },
        },
      ]);
    } catch (error: any) {
      Alert.alert(
        "Signup Failed",
        error.message || "An error occurred during signup"
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
        <View className="flex-1 bg-[#0f0f0f] px-6 pt-16">
          {/* Header */}
          <TouchableOpacity
            onPress={() => router.back()}
            className="flex-row items-center mb-8"
          >
            <ArrowLeft size={24} color="white" />
            <Text className="text-white ml-2 text-lg font-medium">Back</Text>
          </TouchableOpacity>

          {/* Title */}
          <View className="mb-12">
            <Text className="text-white text-4xl font-bold mb-2">
              Create Account
            </Text>
            <Text className="text-gray-300 text-lg">Join us today</Text>
          </View>

          {/* Form */}
          <View className="space-y-6">
            {/* Name Input */}
            <View>
              <Text className="text-white text-sm font-medium mb-2">
                Full Name
              </Text>
              <View className="relative">
                <User
                  size={20}
                  color="#f97316"
                  className="absolute left-4 top-3 z-10"
                />
                <TextInput
                  value={name}
                  onChangeText={setName}
                  placeholder="Enter your full name"
                  placeholderTextColor="#6b7280"
                  autoCapitalize="words"
                  autoCorrect={false}
                  className="bg-[#1a1a1a] border border-gray-700 rounded-2xl px-12 py-4 text-white text-base"
                />
              </View>
            </View>

            {/* Email Input */}
            <View>
              <Text className="text-white text-sm font-medium mb-2">Email</Text>
              <View className="relative">
                <Mail
                  size={20}
                  color="#f97316"
                  className="absolute left-4 top-3 z-10"
                />
                <TextInput
                  value={email}
                  onChangeText={setEmail}
                  placeholder="Enter your email"
                  placeholderTextColor="#6b7280"
                  keyboardType="email-address"
                  autoCapitalize="none"
                  autoCorrect={false}
                  className="bg-[#1a1a1a] border border-gray-700 rounded-2xl px-12 py-4 text-white text-base"
                />
              </View>
            </View>

            {/* Password Input */}
            <View>
              <Text className="text-white text-sm font-medium mb-2">
                Password
              </Text>
              <View className="relative">
                <Lock
                  size={20}
                  color="#f97316"
                  className="absolute left-4 top-3 z-10"
                />
                <TextInput
                  value={password}
                  onChangeText={setPassword}
                  placeholder="Create a password"
                  placeholderTextColor="#6b7280"
                  secureTextEntry={!showPassword}
                  autoCapitalize="none"
                  autoCorrect={false}
                  className="bg-[#1a1a1a] border border-gray-700 rounded-2xl px-12 py-4 text-white text-base pr-12"
                />
                <TouchableOpacity
                  onPress={() => setShowPassword(!showPassword)}
                  className="absolute right-4 top-3"
                >
                  {showPassword ? (
                    <EyeOff size={20} color="#6b7280" />
                  ) : (
                    <Eye size={20} color="#6b7280" />
                  )}
                </TouchableOpacity>
              </View>
            </View>

            {/* Confirm Password Input */}
            <View>
              <Text className="text-white text-sm font-medium mb-2">
                Confirm Password
              </Text>
              <View className="relative">
                <Lock
                  size={20}
                  color="#f97316"
                  className="absolute left-4 top-3 z-10"
                />
                <TextInput
                  value={confirmPassword}
                  onChangeText={setConfirmPassword}
                  placeholder="Confirm your password"
                  placeholderTextColor="#6b7280"
                  secureTextEntry={!showConfirmPassword}
                  autoCapitalize="none"
                  autoCorrect={false}
                  className="bg-[#1a1a1a] border border-gray-700 rounded-2xl px-12 py-4 text-white text-base pr-12"
                />
                <TouchableOpacity
                  onPress={() => setShowConfirmPassword(!showConfirmPassword)}
                  className="absolute right-4 top-3"
                >
                  {showConfirmPassword ? (
                    <EyeOff size={20} color="#6b7280" />
                  ) : (
                    <Eye size={20} color="#6b7280" />
                  )}
                </TouchableOpacity>
              </View>
            </View>

            {/* Sign Up Button */}
            <TouchableOpacity
              onPress={handleSignup}
              disabled={loading}
              className={`bg-orange-500 rounded-2xl py-4 mt-6 ${
                loading ? "opacity-50" : ""
              }`}
            >
              <Text className="text-white text-center text-lg font-semibold">
                {loading ? "Creating Account..." : "Create Account"}
              </Text>
            </TouchableOpacity>

            {/* Sign In Link */}
            <View className="flex-row justify-center mt-8">
              <Text className="text-gray-400 text-base">
                Already have an account?{" "}
              </Text>
              <TouchableOpacity onPress={() => router.push("/(auth)/login")}>
                <Text className="text-orange-500 text-base font-semibold">
                  Sign In
                </Text>
              </TouchableOpacity>
            </View>
          </View>
        </View>
      </ScrollView>
    </KeyboardAvoidingView>
  );
}

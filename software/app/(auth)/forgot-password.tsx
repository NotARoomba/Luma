import { useSession } from "@/hooks/useAuth";
import { useRouter } from "expo-router";
import { ArrowLeft, Mail, Send } from "lucide-react-native";
import * as React from "react";
import { useState } from "react";
import {
  Alert,
  KeyboardAvoidingView,
  Platform,
  Text,
  TextInput,
  TouchableOpacity,
  View,
} from "react-native";

export default function ForgotPasswordScreen() {
  const [email, setEmail] = useState("");
  const [loading, setLoading] = useState(false);
  const [emailSent, setEmailSent] = useState(false);
  const { resetPassword } = useSession();
  const router = useRouter();

  const handleResetPassword = async () => {
    if (!email) {
      Alert.alert("Error", "Please enter your email address");
      return;
    }

    setLoading(true);
    try {
      await resetPassword(email);
      setEmailSent(true);
    } catch (error: any) {
      Alert.alert(
        "Reset Failed",
        error.message || "An error occurred while resetting password"
      );
    } finally {
      setLoading(false);
    }
  };

  if (emailSent) {
    return (
      <View className="flex-1 bg-[#0f0f0f] px-6 pt-16">
        <TouchableOpacity
          onPress={() => router.back()}
          className="flex-row items-center mb-8"
        >
          <ArrowLeft size={24} color="white" />
          <Text className="text-white ml-2 text-lg font-medium">Back</Text>
        </TouchableOpacity>

        <View className="flex-1 justify-center items-center">
          <View className="bg-[#1a1a1a] rounded-3xl p-8 items-center border border-gray-700">
            <Mail size={64} color="#f97316" className="mb-6" />
            <Text className="text-white text-2xl font-bold text-center mb-4">
              Check Your Email
            </Text>
            <Text className="text-gray-300 text-center text-lg mb-6 leading-6">
              We've sent a password reset link to{" "}
              <Text className="text-white font-semibold">{email}</Text>
            </Text>
            <Text className="text-gray-400 text-center text-base mb-8 leading-6">
              Click the link in the email to reset your password. If you don't
              see it, check your spam folder.
            </Text>

            <TouchableOpacity
              onPress={() => router.replace("/(auth)/login")}
              className="bg-orange-500 rounded-2xl py-4 px-8"
            >
              <Text className="text-white text-center text-lg font-semibold">
                Back to Login
              </Text>
            </TouchableOpacity>
          </View>
        </View>
      </View>
    );
  }

  return (
    <KeyboardAvoidingView
      behavior={Platform.OS === "ios" ? "padding" : "height"}
      className="flex-1"
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
            Reset Password
          </Text>
          <Text className="text-gray-300 text-lg">
            Enter your email to receive a reset link
          </Text>
        </View>

        {/* Form */}
        <View className="space-y-6">
          {/* Email Input */}
          <View>
            <Text className="text-white text-sm font-medium mb-2">
              Email Address
            </Text>
            <View className="relative">
              <Mail
                size={20}
                color="#f97316"
                className="absolute left-4 top-3 z-10"
              />
              <TextInput
                value={email}
                onChangeText={setEmail}
                placeholder="Enter your email address"
                placeholderTextColor="#6b7280"
                keyboardType="email-address"
                autoCapitalize="none"
                autoCorrect={false}
                className="bg-[#1a1a1a] border border-gray-700 rounded-2xl px-12 py-4 text-white text-base"
              />
            </View>
          </View>

          {/* Reset Button */}
          <TouchableOpacity
            onPress={handleResetPassword}
            disabled={loading}
            className={`bg-orange-500 rounded-2xl py-4 mt-6 flex-row items-center justify-center ${
              loading ? "opacity-50" : ""
            }`}
          >
            <Send size={20} color="white" className="mr-2" />
            <Text className="text-white text-center text-lg font-semibold">
              {loading ? "Sending..." : "Send Reset Link"}
            </Text>
          </TouchableOpacity>

          {/* Back to Login */}
          <TouchableOpacity
            onPress={() => router.push("/(auth)/login")}
            className="self-center mt-8"
          >
            <Text className="text-orange-500 text-base font-medium">
              Back to Login
            </Text>
          </TouchableOpacity>
        </View>
      </View>
    </KeyboardAvoidingView>
  );
}

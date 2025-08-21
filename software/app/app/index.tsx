import { useRouter } from "expo-router";
import { LogIn, Sparkles, UserPlus } from "lucide-react-native";
import * as React from "react";
import { Text, TouchableOpacity, View } from "react-native";

export default function Index() {
  const router = useRouter();

  return (
    <View className="flex-1 bg-night-500">
      {/* Background Pattern */}
      <View className="absolute top-0 right-0 w-32 h-32 bg-princeton_orange-500 opacity-10 rounded-full -mr-16 -mt-16" />
      <View className="absolute bottom-20 left-0 w-24 h-24 bg-amber-500 opacity-10 rounded-full -ml-12" />

      {/* Main Content */}
      <View className="flex-1 px-6 pt-20 pb-10">
        {/* Header */}
        <View className="items-center mb-16">
          <View className="bg-gradient-to-br from-princeton_orange-500 to-amber-500 w-24 h-24 rounded-3xl items-center justify-center mb-6">
            <Sparkles size={48} color="#0f0f0f" />
          </View>
          <Text className="text-cosmic_latte-500 text-4xl font-bold text-center mb-3">
            Welcome to Luma
          </Text>
          <Text className="text-gunmetal-400 text-lg text-center leading-6">
            Your gateway to amazing experiences
          </Text>
        </View>

        {/* Authentication Buttons */}
        <View className="space-y-4 mb-12">
          <TouchableOpacity
            onPress={() => router.push("/(auth)/login")}
            className="bg-princeton_orange-500 rounded-2xl py-4 flex-row items-center justify-center"
          >
            <LogIn size={24} color="#0f0f0f" className="mr-3" />
            <Text className="text-night-500 text-center text-lg font-semibold">
              Sign In
            </Text>
          </TouchableOpacity>

          <TouchableOpacity
            onPress={() => router.push("/(auth)/signup")}
            className="bg-gunmetal-300 border-2 border-amber-500 rounded-2xl py-4 flex-row items-center justify-center"
          >
            <UserPlus size={24} color="#febf1c" className="mr-3" />
            <Text className="text-night-500 text-center text-lg font-semibold">
              Create Account
            </Text>
          </TouchableOpacity>
        </View>

        {/* Features Preview */}
        <View className="space-y-4">
          <View className="bg-gunmetal-300 p-4 rounded-2xl">
            <Text className="text-cosmic_latte-500 text-sm font-medium text-gunmetal-400 mb-1">
              🔐 Secure Authentication
            </Text>
            <Text className="text-cosmic_latte-500 text-base">
              Firebase-powered login with email verification
            </Text>
          </View>

          <View className="bg-gunmetal-300 p-4 rounded-2xl">
            <Text className="text-cosmic_latte-500 text-sm font-medium text-gunmetal-400 mb-1">
              🎨 Beautiful Design
            </Text>
            <Text className="text-cosmic_latte-500 text-base">
              iOS-inspired UI with custom color palette
            </Text>
          </View>

          <View className="bg-gunmetal-300 p-4 rounded-2xl">
            <Text className="text-cosmic_latte-500 text-sm font-medium text-gunmetal-400 mb-1">
              📱 Cross-Platform
            </Text>
            <Text className="text-cosmic_latte-500 text-base">
              Built with React Native and Expo
            </Text>
          </View>
        </View>

        {/* Footer */}
        <View className="mt-auto items-center">
          <Text className="text-gunmetal-400 text-sm text-center">
            Built with ❤️ using modern technologies
          </Text>
        </View>
      </View>
    </View>
  );
}

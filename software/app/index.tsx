import { useRouter } from "expo-router";
import { LogIn, UserPlus } from "lucide-react-native";
import * as React from "react";
import { Image, Text, TouchableOpacity, View } from "react-native";

export default function Index() {
  const router = useRouter();

  return (
    <View className="flex-1 bg-[#0f0f0f]">
      {/* Main Content */}
      <View className="flex-1 px-8 pt-24 pb-12">
        {/* Header */}
        <View className="items-center mb-20"></View>

        {/* Middle Content */}
        <View className="flex-1 items-center justify-center px-6">
          <View className="w-24 h-24 items-center justify-center mb-12">
            <Image
              source={require("../assets/images/lantern.png")}
              className="w-24 h-24"
              resizeMode="contain"
            />
          </View>
          <Text className="text-white text-4xl font-bold text-center mb-4">
            Welcome to <Text className="text-orange-500">Luma</Text>
          </Text>
          <Text className="text-gray-300 text-lg text-center leading-7 max-w-xs">
            Your smart Bluetooth lantern companion
          </Text>
        </View>

        {/* Authentication Buttons */}
        <View className="flex flex-col space-y-8 justify-center px-4 pb-8">
          <TouchableOpacity
            onPress={() => router.push("/(auth)/login")}
            className="bg-[#1a1a1a] border-2 border-orange-500 rounded-3xl py-5 flex-row items-center justify-center shadow-lg"
            style={{
              shadowColor: "#f97316",
              shadowOffset: { width: 0, height: 4 },
              shadowOpacity: 0.3,
              shadowRadius: 8,
              elevation: 8,
            }}
          >
            <LogIn size={24} color="white" className="mr-4" />
            <Text className="text-white text-center text-lg font-semibold">
              Sign In
            </Text>
          </TouchableOpacity>

          <TouchableOpacity
            onPress={() => router.push("/(auth)/signup")}
            className="bg-[#1a1a1a] border-2 border-orange-500 rounded-3xl py-5 flex-row items-center justify-center shadow-lg"
            style={{
              shadowColor: "#f97316",
              shadowOffset: { width: 0, height: 4 },
              shadowOpacity: 0.3,
              shadowRadius: 8,
              elevation: 8,
            }}
          >
            <UserPlus size={24} color="white" className="mr-4" />
            <Text className="text-white text-center text-lg font-semibold">
              Create Account
            </Text>
          </TouchableOpacity>
        </View>

        {/* Footer */}
        <View className="mt-auto items-center pt-8">
          <Text className="text-gray-400 text-sm text-center">
            Built with ❤️ by <Text className="text-orange-500">NotARoomba</Text>
          </Text>
        </View>
      </View>
    </View>
  );
}

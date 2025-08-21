import auth from "@react-native-firebase/auth";
import { useRouter } from "expo-router";
import { Bell, LogOut, Settings, User } from "lucide-react-native";
import * as React from "react";
import { ScrollView, Text, TouchableOpacity, View } from "react-native";

export default function HomeScreen() {
  const user = auth().currentUser;
  const router = useRouter();

  const handleSignOut = async () => {
    try {
      await auth().signOut();
      router.replace("/");
    } catch (error) {
      console.error("Error signing out:", error);
    }
  };

  return (
    <View className="flex-1 bg-night-500">
      {/* Header */}
      <View className="bg-gunmetal-500 pt-16 pb-6 px-6 rounded-b-3xl">
        <View className="flex-row items-center justify-between mb-6">
          <View>
            <Text className="text-cosmic_latte-500 text-2xl font-bold">
              Welcome back!
            </Text>
            <Text className="text-gunmetal-400 text-base mt-1">
              {user?.email}
            </Text>
          </View>
          <TouchableOpacity
            onPress={handleSignOut}
            className="bg-princeton_orange-500 p-3 rounded-2xl"
          >
            <LogOut size={24} color="#0f0f0f" />
          </TouchableOpacity>
        </View>
      </View>

      {/* Content */}
      <ScrollView
        className="flex-1 px-6 pt-8"
        showsVerticalScrollIndicator={false}
      >
        {/* Quick Actions */}
        <View className="mb-8">
          <Text className="text-cosmic_latte-500 text-xl font-semibold mb-4">
            Quick Actions
          </Text>
          <View className="flex-row space-x-4">
            <TouchableOpacity className="bg-gunmetal-300 p-6 rounded-2xl flex-1 items-center">
              <User size={32} color="#fe9525" />
              <Text className="text-cosmic_latte-500 text-sm font-medium mt-2">
                Profile
              </Text>
            </TouchableOpacity>
            <TouchableOpacity className="bg-gunmetal-300 p-6 rounded-2xl flex-1 items-center">
              <Settings size={32} color="#febf1c" />
              <Text className="text-cosmic_latte-500 text-sm font-medium mt-2">
                Settings
              </Text>
            </TouchableOpacity>
            <TouchableOpacity className="bg-gunmetal-300 p-6 rounded-2xl flex-1 items-center">
              <Bell size={32} color="#5f81a7" />
              <Text className="text-cosmic_latte-500 text-sm font-medium mt-2">
                Notifications
              </Text>
            </TouchableOpacity>
          </View>
        </View>

        {/* Welcome Card */}
        <View className="bg-gradient-to-br from-princeton_orange-500 to-amber-500 p-6 rounded-3xl mb-8">
          <Text className="text-night-500 text-2xl font-bold mb-2">
            🎉 Authentication Successful!
          </Text>
          <Text className="text-night-500 text-base opacity-90">
            You're now logged into your account. This is a simple home screen
            that you can customize with your app's content.
          </Text>
        </View>

        {/* Stats Cards */}
        <View className="mb-8">
          <Text className="text-cosmic_latte-500 text-xl font-semibold mb-4">
            Your Stats
          </Text>
          <View className="space-y-4">
            <View className="bg-gunmetal-300 p-4 rounded-2xl">
              <Text className="text-cosmic_latte-500 text-sm text-gunmetal-400">
                Account Status
              </Text>
              <Text className="text-cosmic_latte-500 text-lg font-semibold">
                Active
              </Text>
            </View>
            <View className="bg-gunmetal-300 p-4 rounded-2xl">
              <Text className="text-cosmic_latte-500 text-sm text-gunmetal-400">
                Member Since
              </Text>
              <Text className="text-cosmic_latte-500 text-lg font-semibold">
                {user?.metadata?.creationTime
                  ? new Date(user.metadata.creationTime).toLocaleDateString()
                  : "Today"}
              </Text>
            </View>
          </View>
        </View>

        {/* Next Steps */}
        <View className="bg-gunmetal-300 p-6 rounded-2xl mb-8">
          <Text className="text-cosmic_latte-500 text-lg font-semibold mb-3">
            Next Steps
          </Text>
          <Text className="text-gunmetal-400 text-base leading-6">
            • Customize this home screen with your app's features{"\n"}• Add
            navigation to other parts of your app{"\n"}• Implement user profile
            management{"\n"}• Add more authentication features as needed
          </Text>
        </View>
      </ScrollView>
    </View>
  );
}

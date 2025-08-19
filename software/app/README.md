# Luma - LED Lantern Control App

A comprehensive mobile application for controlling smart LED lanterns with Firebase authentication, Bluetooth connectivity, and social features.

## Features

### 🔐 Authentication

- User registration and login with Firebase
- Password reset functionality
- Secure user management

### 💡 Lantern Control

- Add new lanterns via Bluetooth scanning
- Control LED colors and brightness
- Multiple lighting modes (solid, rainbow, breathing, strobe)
- Real-time updates via Firebase

### 👥 Social Features

- Share lanterns with friends via QR codes or UUIDs
- Granular permission system for shared access
- Friend management and invitations

### 🎨 Customization

- Dark theme with orange/purple accent options
- Animated transitions and smooth interactions
- Customizable app preferences

### 📱 Modern UI/UX

- NativeWind styling with TailwindCSS
- Smooth animations and transitions
- Responsive design for all screen sizes
- Custom bottom navigation inspired by modern design patterns

## Tech Stack

### Frontend

- **React Native** with Expo SDK 53
- **NativeWind** for styling (TailwindCSS)
- **TypeScript** for type safety
- **Framer Motion** for animations

### Backend

- **Firebase Authentication** for user management
- **Firestore** for real-time database
- **Firebase Cloud Functions** for server-side logic

### Hardware

- **ESP32** microcontroller
- **WS2812B LED strips** for lighting
- **Bluetooth Low Energy** for device discovery
- **WiFi** for internet connectivity

## Setup Instructions

### Prerequisites

- Node.js 18+ and npm
- Expo CLI (`npm install -g @expo/cli`)
- Android Studio / Xcode for mobile development
- ESP-IDF for firmware development

### 1. Clone and Install Dependencies

```bash
git clone <repository-url>
cd app
npm install
```

### 2. Firebase Setup

1. Create a new Firebase project at [console.firebase.google.com](https://console.firebase.google.com)
2. Enable Authentication (Email/Password)
3. Create a Firestore database
4. Update `config/firebase.ts` with your Firebase credentials:

```typescript
const firebaseConfig = {
  apiKey: "your-api-key",
  authDomain: "your-project.firebaseapp.com",
  projectId: "your-project-id",
  storageBucket: "your-project.appspot.com",
  messagingSenderId: "your-sender-id",
  appId: "your-app-id",
};
```

### 3. Configure NativeWind

The app is already configured with NativeWind. Make sure you have:

- `tailwind.config.js` for custom colors and animations
- `babel.config.js` with NativeWind plugin
- `nativewind-env.d.ts` for TypeScript support

### 4. Run the App

```bash
# Start the development server
npm start

# Run on iOS simulator
npm run ios

# Run on Android emulator
npm run android

# Run on web
npm run web
```

## Firmware Setup

### 1. Install ESP-IDF

Follow the [ESP-IDF installation guide](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/get-started/index.html)

### 2. Configure WiFi and Firebase

Update the firmware configuration in `firmware/main/main.c`:

```c
#define WIFI_SSID "YOUR_WIFI_SSID"
#define WIFI_PASS "YOUR_WIFI_PASSWORD"
#define FIREBASE_HOST "your-project.firebaseio.com"
#define FIREBASE_AUTH "your-firebase-secret"
```

### 3. Build and Flash

```bash
cd firmware
idf.py build
idf.py flash monitor
```

## App Structure

```
app/
├── app/                    # Expo Router screens
│   ├── (auth)/            # Authentication screens
│   │   ├── login.tsx      # Login screen
│   │   ├── signup.tsx     # Signup screen
│   │   └── reset-password.tsx # Password reset
│   ├── (tabs)/            # Main app tabs
│   │   ├── index.tsx      # Lanterns list
│   │   ├── profile.tsx    # User profile
│   │   └── settings.tsx   # App settings
│   └── _layout.tsx        # Root layout
├── components/             # Reusable components
├── contexts/               # React contexts
│   ├── AuthContext.tsx    # Authentication state
│   └── ThemeContext.tsx   # Theme management
├── types/                  # TypeScript definitions
├── config/                 # Configuration files
└── assets/                 # Images and fonts
```

## Key Components

### Authentication Flow

- Splash screen with animated logo
- Login/signup with Firebase
- Password reset via email
- Automatic navigation based on auth state

### Lantern Management

- Bluetooth device discovery
- QR code scanning for sharing
- Real-time color and brightness control
- Permission-based access control

### Theme System

- Dark mode with customizable accents
- Orange and purple theme options
- Persistent theme storage
- Smooth theme transitions

## Contributing

1. Fork the repository
2. Create a feature branch
3. Make your changes
4. Add tests if applicable
5. Submit a pull request

## License

This project is licensed under the MIT License - see the LICENSE file for details.

## Support

For support and questions:

- Create an issue on GitHub
- Check the documentation
- Join our community Discord

---

**Illuminate Your World** ✨

export interface User {
  id: string;
  email: string;
  displayName?: string;
  photoURL?: string;
  createdAt: Date;
}

export interface Lantern {
  id: string;
  name: string;
  uuid: string;
  ownerId: string;
  color: string;
  brightness: number;
  isConnected: boolean;
  isOnline: boolean;
  lastSeen: Date;
  friends: LanternFriend[];
  settings: LanternSettings;
  createdAt: Date;
  updatedAt: Date;
}

export interface LanternFriend {
  id: string;
  userId: string;
  displayName: string;
  permissions: FriendPermissions;
  addedAt: Date;
}

export interface FriendPermissions {
  canControl: boolean;
  canChangeColor: boolean;
  canAddFriends: boolean;
  canRemoveFriends: boolean;
}

export interface LanternSettings {
  autoConnect: boolean;
  brightness: number;
  colorMode: 'solid' | 'rainbow' | 'breathing' | 'strobe';
  transitionSpeed: number;
  powerSaving: boolean;
}

export interface BluetoothDevice {
  id: string;
  name: string;
  rssi: number;
  isConnectable: boolean;
  manufacturerData?: string;
}

export interface Theme {
  primary: 'orange' | 'purple';
  primaryColor: string;
  secondaryColor: string;
  accentColor: string;
}

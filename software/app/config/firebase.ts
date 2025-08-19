import { initializeApp } from 'firebase/app';
import { getAuth } from 'firebase/auth';
import { getFirestore } from 'firebase/firestore';

const firebaseConfig = {
  apiKey: process.env.FIREBASE_API_KEY,
  authDomain: process.env.FIREBASE_AUTH_DOMAIN || "luma-application.firebaseapp.com",
  projectId: process.env.FIREBASE_PROJECT_ID || "luma-application",
  storageBucket: process.env.FIREBASE_STORAGE_BUCKET || "luma-application.firebasestorage.app",
  messagingSenderId: process.env.FIREBASE_MESSAGING_SENDER_ID || "944314074710",
  appId: process.env.FIREBASE_APP_ID || "1:944314074710:web:8de444d36cc42efb8fddf7"
};

// Initialize Firebase
const app = initializeApp(firebaseConfig);

// Initialize Firebase Authentication and get a reference to the service
export const auth = getAuth(app);

// Initialize Cloud Firestore and get a reference to the service
export const db = getFirestore(app);

export default app;

import React, { createContext, useContext, useState, useEffect } from "react";
import AsyncStorage from "@react-native-async-storage/async-storage";
import { Theme } from "../types";

interface ThemeContextType {
  theme: Theme;
  setPrimaryColor: (color: "orange" | "purple") => void;
  updateTheme: (newTheme: Partial<Theme>) => void;
}

const defaultTheme: Theme = {
  primary: "orange",
  primaryColor: "#ff6b35",
  secondaryColor: "#2a2a2a",
  accentColor: "#ffd700",
};

const ThemeContext = createContext<ThemeContextType | undefined>(undefined);

export const useTheme = () => {
  const context = useContext(ThemeContext);
  if (context === undefined) {
    throw new Error("useTheme must be used within a ThemeProvider");
  }
  return context;
};

export const ThemeProvider: React.FC<{ children: React.ReactNode }> = ({
  children,
}) => {
  const [theme, setTheme] = useState<Theme>(defaultTheme);

  useEffect(() => {
    loadTheme();
  }, []);

  const loadTheme = async () => {
    try {
      const savedTheme = await AsyncStorage.getItem("theme");
      if (savedTheme) {
        setTheme(JSON.parse(savedTheme));
      }
    } catch (error) {
      console.error("Error loading theme:", error);
    }
  };

  const setPrimaryColor = (color: "orange" | "purple") => {
    const newTheme: Theme = {
      ...theme,
      primary: color,
      primaryColor: color === "orange" ? "#ff6b35" : "#8b5cf6",
      accentColor: color === "orange" ? "#ffd700" : "#9370db",
    };
    setTheme(newTheme);
    saveTheme(newTheme);
  };

  const updateTheme = (newTheme: Partial<Theme>) => {
    const updatedTheme = { ...theme, ...newTheme };
    setTheme(updatedTheme);
    saveTheme(updatedTheme);
  };

  const saveTheme = async (newTheme: Theme) => {
    try {
      await AsyncStorage.setItem("theme", JSON.stringify(newTheme));
    } catch (error) {
      console.error("Error saving theme:", error);
    }
  };

  const value = {
    theme,
    setPrimaryColor,
    updateTheme,
  };

  return (
    <ThemeContext.Provider value={value}>{children}</ThemeContext.Provider>
  );
};

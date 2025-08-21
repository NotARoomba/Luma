/** @type {import('tailwindcss').Config} */
module.exports = {
  content: [],
  presets: [require("nativewind/preset")],
  theme: {
    extend: {
      colors: { 'princeton_orange': { DEFAULT: '#fe9525', 100: '#3a1e00', 200: '#743c01', 300: '#ae5a01', 400: '#e77801', 500: '#fe9525', 600: '#feaa50', 700: '#febf7c', 800: '#ffd5a8', 900: '#ffead3' }, 'amber': { DEFAULT: '#febf1c', 100: '#382800', 200: '#705001', 300: '#a77801', 400: '#dfa001', 500: '#febf1c', 600: '#fecb48', 700: '#fed876', 800: '#ffe5a4', 900: '#fff2d1' }, 'gunmetal': { DEFAULT: '#222f3e', 100: '#070a0d', 200: '#0e1319', 300: '#151d26', 400: '#1c2632', 500: '#222f3e', 600: '#405874', 700: '#5f81a7', 800: '#94abc4', 900: '#cad5e2' }, 'night': { DEFAULT: '#0f0f0f', 100: '#030303', 200: '#060606', 300: '#090909', 400: '#0c0c0c', 500: '#0f0f0f', 600: '#3f3f3f', 700: '#6f6f6f', 800: '#9f9f9f', 900: '#cfcfcf' }, 'cosmic_latte': { DEFAULT: '#fff8e6', 100: '#614500', 200: '#c28b00', 300: '#ffc124', 400: '#ffdc85', 500: '#fff8e6', 600: '#fff9eb', 700: '#fffbf0', 800: '#fffcf5', 900: '#fffefa' } }
    },
  },
  plugins: [],
}


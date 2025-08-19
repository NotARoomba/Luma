/** @type {import('tailwindcss').Config} */
module.exports = {
  content: ["./app/**/*.{js,jsx,ts,tsx}", "./components/**/*.{js,jsx,ts,tsx}"],
  theme: {
    extend: {
      colors: {
        'dark-grey': '#1a1a1a',
        'dark-grey-light': '#2a2a2a',
        'dark-grey-lighter': '#3a3a3a',
        'orange-accent': '#ff6b35',
        'orange-accent-light': '#ff8a5c',
        'purple-accent': '#8b5cf6',
        'purple-accent-light': '#a78bfa',
        'lantern-glow': '#ffd700',
        'lantern-glow-orange': '#ff8c00',
        'lantern-glow-purple': '#9370db',
      },
      animation: {
        'fade-in': 'fadeIn 0.5s ease-in-out',
        'slide-up': 'slideUp 0.3s ease-out',
        'pulse-glow': 'pulseGlow 2s ease-in-out infinite',
      },
      keyframes: {
        fadeIn: {
          '0%': { opacity: '0' },
          '100%': { opacity: '1' },
        },
        slideUp: {
          '0%': { transform: 'translateY(20px)', opacity: '0' },
          '100%': { transform: 'translateY(0)', opacity: '1' },
        },
        pulseGlow: {
          '0%, 100%': { opacity: '1' },
          '50%': { opacity: '0.7' },
        },
      },
    },
  },
  plugins: [],
}

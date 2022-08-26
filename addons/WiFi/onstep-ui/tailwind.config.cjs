/** @type {import('tailwindcss').Config} */
module.exports = {
  content: ["./index.html", "./src/**/*.{vue,js,ts,jsx,tsx}"],
  separator: "_",
  theme: {
    extend: {},
  },
  plugins: [require("@tailwindcss/forms")],
};

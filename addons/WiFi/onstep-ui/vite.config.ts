import { inlineAssets, generateHeaderFile } from "./build/plugins";
import { defineConfig, loadEnv } from "vite";
import vue from "@vitejs/plugin-vue";
import analyze from "rollup-plugin-analyzer";

export default defineConfig(({ mode }) => ({
  plugins: [
    vue(),
    inlineAssets,
    generateHeaderFile,
    analyze({
      summaryOnly: true,
    }),
  ],
  server: {
    proxy: {
      // OnStep responses do not have CORS enabled - we can use Vite to proxy
      // the requests to OnStep.
      "^/api/.*": {
        target: loadEnv(mode, process.cwd()).VITE_ONSTEP_URL,
        rewrite: (path) => path.replace(/^\/api/, ""),
        changeOrigin: true,
      },
    },
  },
}));

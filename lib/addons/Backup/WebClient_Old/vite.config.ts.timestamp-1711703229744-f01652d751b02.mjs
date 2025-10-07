// vite.config.ts
import path from "path";
import { splitVendorChunkPlugin } from 'vite'
import { defineConfig } from "file:///E:/KERJA/2024/Personal/ALS_App/als-app/node_modules/vite/dist/node/index.js";
import vue from "file:///E:/KERJA/2024/Personal/ALS_App/als-app/node_modules/@vitejs/plugin-vue/dist/index.mjs";
import tailwind from "file:///E:/KERJA/2024/Personal/ALS_App/als-app/node_modules/tailwindcss/lib/index.js";
import autoprefixer from "file:///E:/KERJA/2024/Personal/ALS_App/als-app/node_modules/autoprefixer/lib/autoprefixer.js";
var __vite_injected_original_dirname = "E:\\KERJA\\2024\\Personal\\ALS_App\\als-app";
var vite_config_default = defineConfig({
  css: {
    postcss: {
      plugins: [tailwind(), autoprefixer()]
    }
  },
  plugins: [vue()],
  resolve: {
    alias: {
      "@": path.resolve(__vite_injected_original_dirname, "./src")
    }
  }
});
export {
  vite_config_default as default
};
//# sourceMappingURL=data:application/json;base64,ewogICJ2ZXJzaW9uIjogMywKICAic291cmNlcyI6IFsidml0ZS5jb25maWcudHMiXSwKICAic291cmNlc0NvbnRlbnQiOiBbImNvbnN0IF9fdml0ZV9pbmplY3RlZF9vcmlnaW5hbF9kaXJuYW1lID0gXCJFOlxcXFxLRVJKQVxcXFwyMDI0XFxcXFBlcnNvbmFsXFxcXEFMU19BcHBcXFxcYWxzLWFwcFwiO2NvbnN0IF9fdml0ZV9pbmplY3RlZF9vcmlnaW5hbF9maWxlbmFtZSA9IFwiRTpcXFxcS0VSSkFcXFxcMjAyNFxcXFxQZXJzb25hbFxcXFxBTFNfQXBwXFxcXGFscy1hcHBcXFxcdml0ZS5jb25maWcudHNcIjtjb25zdCBfX3ZpdGVfaW5qZWN0ZWRfb3JpZ2luYWxfaW1wb3J0X21ldGFfdXJsID0gXCJmaWxlOi8vL0U6L0tFUkpBLzIwMjQvUGVyc29uYWwvQUxTX0FwcC9hbHMtYXBwL3ZpdGUuY29uZmlnLnRzXCI7aW1wb3J0IHBhdGggZnJvbSBcInBhdGhcIlxuaW1wb3J0IHsgZGVmaW5lQ29uZmlnIH0gZnJvbSAndml0ZSdcbmltcG9ydCB2dWUgZnJvbSAnQHZpdGVqcy9wbHVnaW4tdnVlJ1xuXG5pbXBvcnQgdGFpbHdpbmQgZnJvbSBcInRhaWx3aW5kY3NzXCJcbmltcG9ydCBhdXRvcHJlZml4ZXIgZnJvbSBcImF1dG9wcmVmaXhlclwiXG5cbi8vIGh0dHBzOi8vdml0ZWpzLmRldi9jb25maWcvXG5leHBvcnQgZGVmYXVsdCBkZWZpbmVDb25maWcoe1xuICBjc3M6IHtcbiAgICBwb3N0Y3NzOiB7XG4gICAgICBwbHVnaW5zOiBbdGFpbHdpbmQoKSwgYXV0b3ByZWZpeGVyKCldLFxuICAgIH0sXG4gIH0sXG4gIHBsdWdpbnM6IFt2dWUoKV0sXG4gIHJlc29sdmU6IHtcbiAgICBhbGlhczoge1xuICAgICAgXCJAXCI6IHBhdGgucmVzb2x2ZShfX2Rpcm5hbWUsIFwiLi9zcmNcIiksXG4gICAgfSxcbiAgfSxcbn0pXG4iXSwKICAibWFwcGluZ3MiOiAiO0FBQWdULE9BQU8sVUFBVTtBQUNqVSxTQUFTLG9CQUFvQjtBQUM3QixPQUFPLFNBQVM7QUFFaEIsT0FBTyxjQUFjO0FBQ3JCLE9BQU8sa0JBQWtCO0FBTHpCLElBQU0sbUNBQW1DO0FBUXpDLElBQU8sc0JBQVEsYUFBYTtBQUFBLEVBQzFCLEtBQUs7QUFBQSxJQUNILFNBQVM7QUFBQSxNQUNQLFNBQVMsQ0FBQyxTQUFTLEdBQUcsYUFBYSxDQUFDO0FBQUEsSUFDdEM7QUFBQSxFQUNGO0FBQUEsRUFDQSxTQUFTLENBQUMsSUFBSSxDQUFDO0FBQUEsRUFDZixTQUFTO0FBQUEsSUFDUCxPQUFPO0FBQUEsTUFDTCxLQUFLLEtBQUssUUFBUSxrQ0FBVyxPQUFPO0FBQUEsSUFDdEM7QUFBQSxFQUNGO0FBQ0YsQ0FBQzsiLAogICJuYW1lcyI6IFtdCn0K

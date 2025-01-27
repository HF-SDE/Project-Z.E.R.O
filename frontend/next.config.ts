import type { NextConfig } from "next";

const nextConfig: NextConfig = {
  reactStrictMode: true,
  output: "standalone",
  env: {
    SERVER_API_URL: process.env.SERVER_API_URL ?? "http://reverse-proxy/api/",
    CLIENT_API_URL: process.env.CLIENT_API_URL ?? "http://localhost/api/",
  },
};

console.log("SERVER URL", process.env.SERVER_API_URL);
console.log("CLIENT URL", process.env.CLIENT_API_URL);

if (!process.env.SERVER_API_URL) {
  console.error("Missing SERVER_API_URL from environment variables");
}
if (!process.env.CLIENT_API_URL) {
  console.error("Missing CLIENT_API_URL from environment variables");
}

export default nextConfig;

import axios from "axios";
import { RequestInit } from "next/dist/server/web/spec-extension/request";
const backendAxios = axios.create({
  baseURL: process.env.CLIENT_API_URL,
  withCredentials: true,
});

export async function backendCall(
  url: string,
  method: RequestInit["method"],
  headers?: RequestInit["headers"],
  data?: any
): Promise<string> {
  const baseUrl = process.env.SERVER_API_URL;
  if (!baseUrl) {
    console.error("Error: Environment variable URL is undefined.");
    throw new Error("Backend URL is not configured.");
  }

  console.log("Fetching from:", baseUrl + url);

  const authHeader = backendAxios.defaults.headers.common.Authorization;
  const headersWithAuth = {
    ...headers,
    "Content-Type": "application/json",
    ...(authHeader && { Authorization: authHeader?.toString() || "" }),
  };

  try {
    const res = await fetch(baseUrl + url, {
      credentials: "include",
      method,
      body: data ? JSON.stringify(data) : undefined,
      headers: headersWithAuth,
      priority: "high",
      mode: "same-origin",
    });

    if (!res.ok) {
      console.error("HTTP error! Status:", res.status);
    }

    return await res.json();
  } catch (error) {
    console.error("Fetch failed:", error);
    throw error;
  }
}

export default backendAxios;

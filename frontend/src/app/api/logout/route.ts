import { cookies } from "next/headers";
import { NextResponse } from "next/server";

import { getCookies } from "@/helpers/cookies";

import axios from "axios";

const logoutInstance = axios.create({
  baseURL: process.env.BACKEND_URL,
  withCredentials: true,
  validateStatus: (status) => status < 500,
});

export async function POST() {
  const allCookies = await cookies();

  const token = await getCookies("token");

  if (!token) return new NextResponse(null, { status: 400 });

  const refreshResponse = await logoutInstance.get("/refreshToken", {
    headers: { Authorization: `Bearer ${token}` },
  });

  if (refreshResponse.status !== 200) {
    console.log("refreshResponse error", refreshResponse.data);

    allCookies.delete("token");

    return new NextResponse(null, { status: 400 });
  }

  const resfreshToken = refreshResponse.data.data.refreshToken.token;

  const response = await logoutInstance.post("/logout", {
    token: resfreshToken,
  });

  allCookies.delete("token");

  if (response.status === 200) {
    axios.defaults.headers.Authorization = null;

    return new NextResponse(JSON.stringify({ success: true }), { status: 200 });
  }

  return new NextResponse(null, { status: 400 });
}

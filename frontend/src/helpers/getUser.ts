"use server";

import { CurrentUser } from "@/types/user";

import { jwtDecode } from "jwt-decode";

import { getCookies } from "./cookies";

export async function getUser(preToken?: string) {
  const token = preToken || (await getCookies("token"));

  if (token) return jwtDecode<CurrentUser>(token);
}

"use server";

import { cookies } from "next/headers";

export async function getCookies(name: string) {
  const cookieStore = await cookies();
  const cookie = cookieStore.get(name) || { value: "" };

  return cookie.value;
}

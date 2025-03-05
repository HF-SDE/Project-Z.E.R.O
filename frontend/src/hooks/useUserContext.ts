"use client";

import { use } from "react";

import { UserContext } from "@/contexts/user";

export function useUserContext() {
  return use(UserContext);
}

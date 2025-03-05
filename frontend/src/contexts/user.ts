"use client";

import { createContext } from "react";

import { CurrentUser } from "@/types/user";

interface UserContextType {
  user: CurrentUser | undefined;
  setUser: (user: CurrentUser) => void;
}

export const UserContext = createContext<UserContextType>({
  user: undefined,
  setUser: () => {},
});

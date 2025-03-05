"use client";

import { useState } from "react";

import { UserContext } from "@/contexts/user";
import { CurrentUser } from "@/types/user";

interface UserProviderProps {
  children: React.ReactNode;
  initUser: CurrentUser;
}

export default function UserProvider({
  children,
  initUser,
}: UserProviderProps) {
  const [user, setUser] = useState<CurrentUser>(initUser);

  return <UserContext value={{ user, setUser }}>{children}</UserContext>;
}

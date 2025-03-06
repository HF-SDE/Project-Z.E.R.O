"use client";

import useRealtimeLogout from "@/hooks/useRealtimeLogout";
import { useUserContext } from "@/hooks/useUserContext";

export default function RealtimeLogout() {
  const { user } = useUserContext();
  useRealtimeLogout(user);

  return null;
}

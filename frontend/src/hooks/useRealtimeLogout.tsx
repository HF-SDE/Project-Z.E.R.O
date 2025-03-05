import { useEffect } from "react";

import { CurrentUser } from "@/types/user";

import axios from "axios";

export default function useRealtimeLogout(
  currentUser: CurrentUser | undefined,
) {
  useEffect(() => {
    if (!currentUser) return;

    const timeUntilExpire = currentUser.exp * 1000 - Date.now();

    const expireTimeout = setTimeout(async () => {
      await axios.post("/api/logout");
      window.location.reload();
    }, timeUntilExpire + 1000);

    return () => {
      clearTimeout(expireTimeout);
    };
  }, [currentUser]);
}

import { useEffect, useState } from "react";

import { client } from "@/axios";
import { User } from "@/types/user";

interface WithError {
  users: undefined;
  error: string;
  loading: false;
}

interface WithoutError {
  users: User[];
  error: undefined;
  loading: false;
}

interface Loading {
  users: undefined;
  error: undefined;
  loading: true;
}

type UseUsersState = WithError | WithoutError | Loading;

export function useUsers(): UseUsersState {
  const [state, setState] = useState<UseUsersState>({
    users: undefined,
    error: undefined,
    loading: true,
  });

  async function getUsers() {
    const { data, status } = await client.get("/api/users").catch((err) => {
      return {
        data: { message: err.response.data.message },
        status: err.status,
      };
    });

    if (status !== 200) {
      setState({ error: data.message, loading: false } as WithError);
      return;
    }

    setState({ users: data.data, loading: false } as WithoutError);
  }

  useEffect(() => {
    getUsers();
  }, []);

  return state;
}

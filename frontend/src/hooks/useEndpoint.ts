import { useEffect, useState } from "react";

import { client } from "@/axios";

interface WithError {
  state: undefined;
  error: string;
  loading: false;
}

interface WithoutError<T> {
  state: T;
  error: undefined;
  loading: false;
}

interface Loading {
  state: undefined;
  error: undefined;
  loading: true;
}

type UseEndpointState<T> = WithError | WithoutError<T> | Loading;

export function useEndpoint<T>(url: string): UseEndpointState<T> {
  const [state, setState] = useState<UseEndpointState<T>>({
    state: undefined,
    error: undefined,
    loading: true,
  });

  async function getData() {
    const { data, status } = await client.get(url).catch((err) => {
      return {
        data: { message: err.response.data.message },
        status: err.status,
      };
    });

    if (status !== 200) {
      setState({ error: data.message, loading: false } as WithError);
      return;
    }

    setState({ state: data.data, loading: false } as WithoutError<T>);
  }

  useEffect(() => {
    getData();
  }, []);

  return state;
}

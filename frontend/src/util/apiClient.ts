import { cookies } from "next/headers";

import { getCookies } from "@/helpers/cookies";

import axios from "axios";

const baseURL = process.env.BACKEND_URL;
const apiClient = axios.create({
  baseURL: baseURL,
  validateStatus: (status) => status >= 200 && status < 500,
});
const localApiClient = axios.create({ baseURL: baseURL });

apiClient.interceptors.request.use(
  async (config) => {
    const cookieStore = await cookies();

    try {
      if (config.url === "/refreshToken" || config.url === "/accessToken") {
        return config;
      }

      const authHeader = await getAuthHeader();
      if (authHeader) {
        config.headers.Authorization = authHeader;
      }
    } catch {
      cookieStore.delete("token");
      // router.replace("/login");
    }

    return config;
  },
  (error) => {
    return Promise.reject(error);
  },
);

apiClient.interceptors.response.use(
  async (response) => response,
  async (error) => {
    const cookieStore = await cookies();

    try {
      if (
        error.response &&
        (error.response.status === 401 || error.response.status === 403)
      ) {
        const originalRequest = error.config;

        if (!originalRequest._retry) {
          originalRequest._retry = true;

          const authHeader = await getAuthHeader();
          if (authHeader) {
            originalRequest.headers["Authorization"] = authHeader;
          }
        }

        cookieStore.delete("token");
        // router.replace("/login");
      }
    } catch (error) {
      console.error("Error during response handling:", error);
    }

    return Promise.reject(error);
  },
);

async function getAuthHeader(): Promise<string | undefined> {
  const token = await getCookies("token");

  if (token) {
    if (isTokenExpired(token)) {
      const newToken = await getNewAccessToken(token);
      return getAuthHeaderFormat(newToken);
    } else {
      return getAuthHeaderFormat(token);
    }
  } else {
    return undefined;
  }
}

const isTokenExpired = (token: string): boolean => {
  try {
    const payloadBase64 = token.split(".")[1];
    const decodedPayload = JSON.parse(atob(payloadBase64));

    if (!decodedPayload.exp) {
      throw new Error("Token does not have an exp field");
    }

    const currentTime = Math.floor(Date.now() / 1000);
    return decodedPayload.exp < currentTime;
  } catch {
    return true;
  }
};

const getNewAccessToken = async (
  expiredToken: string,
): Promise<string | undefined> => {
  const cookieStore = await cookies();

  try {
    const refreshResponse = await localApiClient.get("/refreshToken", {
      headers: { Authorization: getAuthHeaderFormat(expiredToken) },
    });

    const refreshToken = refreshResponse.data?.data?.refreshToken?.token;
    if (!refreshToken) throw new Error("Failed to retrieve refresh token");

    const accessResponse = await localApiClient.post("/accessToken", {
      token: refreshToken,
    });

    const accessToken = accessResponse.data?.data?.accessToken?.token;
    if (!accessToken) throw new Error("Failed to retrieve access token");

    cookieStore.set("token", accessToken);

    return accessToken;
  } catch {
    throw new Error("Error while refreshing access token");
  }
};

const getAuthHeaderFormat = (token: string | undefined): string | undefined => {
  return token ? `Bearer ${token}` : undefined;
};

export default apiClient;

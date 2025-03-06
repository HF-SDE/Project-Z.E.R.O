"use client";

import { useRouter } from "next/navigation";

import { LoginForm } from "@/components/login-form";

import axios from "axios";
import { toast } from "sonner";

export default function Page() {
  const router = useRouter();

  async function handleSubmit(e: React.FormEvent<HTMLFormElement>) {
    e.preventDefault();

    const formData = new FormData(e.currentTarget);

    const username = formData.get("username") as string;
    const password = formData.get("password") as string;

    const data = {
      username,
      password: Buffer.from(password).toString("base64"),
    };

    const loginPromise = axios.post("api/login", data);

    toast.promise(loginPromise, {
      loading: "Logging in...",
      success: () => {
        router.push("/dashboard");
        return "Logged in";
      },
      error: "Failed to login",
    });
  }

  return (
    <div className="flex min-h-svh w-full items-center justify-center p-6 md:p-10">
      <div className="w-full max-w-sm">
        <LoginForm handleSubmit={handleSubmit} />
      </div>
    </div>
  );
}

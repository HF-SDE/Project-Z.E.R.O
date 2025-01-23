"use client";
import { Button } from "@components/ui/button";
import {
  Card,
  CardDescription,
  CardHeader,
  CardTitle,
} from "@components/ui/card";
import { Input } from "@components/ui/input";
import { Label } from "@components/ui/label";
import { useUser } from "@hooks/useUser";
import { cn } from "@lib/utils";
import { loginUser } from "core/redux/actions/user.actions";
import { usePathname, useRouter } from "next/navigation";
import {
  ComponentPropsWithoutRef,
  FormEvent,
  useEffect,
  useState,
} from "react";
import { useDispatch } from "react-redux";

export function LoginForm({
  className,
  ...props
}: ComponentPropsWithoutRef<"div">) {
  const [state, setState] = useState({
    username: "",
    password: "",
  });

  const user = useUser();
  const dispatch = useDispatch();
  const router = useRouter();
  const pathname = usePathname()

  useEffect(() => {
    if (user.AUTHENTICATED) {
      if (pathname.at(0) === "/") {
        router.push(pathname.toString());
      }
      router.push("/dashboard");
    } else {
      console.warn("User is not authenticated");
    }
  }, [user.AUTHENTICATED]);

  /**
   * Handle the login form submission
   * @param {FormEvent<HTMLFormElement>} event
   */
  async function onLogin(event: FormEvent<HTMLFormElement>) {
    event.preventDefault();
    dispatch(loginUser(state.username, state.password));
  }

  return (
    <Card className={cn("flex flex-col gap-6", className)} {...props}>
      <CardHeader>
        <CardTitle className="text-2xl">Login</CardTitle>
        <CardDescription>
          Enter your email below to login to your account
        </CardDescription>
      </CardHeader>
      <form className="flex flex-col gap-6 p-6 pt-0" onSubmit={onLogin}>
        <fieldset className="grid gap-2">
          <Label htmlFor="username">username</Label>
          <Input
            id="username"
            type="text"
            required
            onChange={(e) => setState({ ...state, username: e.target.value })}
          />
        </fieldset>
        <fieldset className="grid gap-2">
          <div className="flex items-center">
            <Label htmlFor="password">Password</Label>
            <p className="ml-auto inline-block text-sm underline-offset-4 cursor-pointer hover:underline">
              Forgot your password?
            </p>
          </div>
          <Input
            id="password"
            type="password"
            required
            onChange={(e) => setState({ ...state, password: e.target.value })}
          />
        </fieldset>
        <Button type="submit" className="w-full">
          Login
        </Button>
      </form>
    </Card>
  );
}

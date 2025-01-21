import { Button } from "@components/ui/button";
import {
  Card,
  CardDescription,
  CardHeader,
  CardTitle,
} from "@components/ui/card";
import { Input } from "@components/ui/input";
import { Label } from "@components/ui/label";
import { cn } from "@lib/utils";

export function LoginForm({
  className,
  ...props
}: React.ComponentPropsWithoutRef<"div">) {
  return (
    <Card className={cn("flex flex-col gap-6", className)} {...props}>
      <CardHeader>
        <CardTitle className="text-2xl">Login</CardTitle>
        <CardDescription>
          Enter your email below to login to your accoun
        </CardDescription>
      </CardHeader>
      <form className="flex flex-col gap-6 p-6 pt-0">
        <fieldset className="grid gap-2">
          <Label htmlFor="email">Email</Label>
          <Input id="email" type="email" placeholder="m@example.com" required />
        </fieldset>
        <fieldset className="grid gap-2">
          <div className="flex items-center">
            <Label htmlFor="password">Password</Label>
            <p className="ml-auto inline-block text-sm underline-offset-4 cursor-pointer hover:underline">
              Forgot your password?
            </p>
          </div>
          <Input id="password" type="password" required />
        </fieldset>
        <Button type="submit" className="w-full">
          Login
        </Button>
      </form>
    </Card>
  );
}

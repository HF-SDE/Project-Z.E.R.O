import { useState } from "react";

import { Button } from "@/components/ui/button";
import {
  Dialog,
  DialogContent,
  DialogDescription,
  DialogHeader,
  DialogTitle,
  DialogTrigger,
} from "@/components/ui/dialog";
import { Input } from "@/components/ui/input";
import { Label } from "@/components/ui/label";

import { User } from "@/types/user";

import axios, { AxiosError } from "axios";
import { toast } from "sonner";

export type NewUser = Omit<User, "id" | "createdAt" | "updatedAt"> & {
  password: string;
  confirmPassword?: string;
};

interface NewUserDialogProps {
  onSubmit?: (newUser: User) => void;
}

interface ErrorResponse {
  status: string;
  message: string;
}

export default function NewUserDialog({
  onSubmit = () => {},
}: NewUserDialogProps) {
  const [open, setOpen] = useState(false);

  function handleSubmit(e: React.FormEvent<HTMLFormElement>) {
    e.preventDefault();

    const formData = new FormData(e.currentTarget);

    const newUser = Object.fromEntries(formData) as unknown as NewUser;

    if (newUser.password !== newUser.confirmPassword) {
      return toast.warning("Passwords do not match");
    }

    delete newUser.confirmPassword;
    newUser.password = Buffer.from(newUser.password).toString("base64");

    toast.promise(axios.post("/api/users", newUser), {
      loading: "Creating user...",
      success: () => {
        setOpen(false);

        onSubmit({
          id: Math.random().toString(),
          name: newUser.name,
          username: newUser.username,
          email: newUser.email,
          initials: newUser.initials,
          active: true,
          createdAt: new Date().toISOString(),
          updatedAt: new Date().toISOString(),
        });
        return "User created";
      },
      error: (err: AxiosError<ErrorResponse>) => err.response?.data?.message,
    });
  }

  return (
    <Dialog open={open} onOpenChange={setOpen}>
      <DialogTrigger>
        <Button asChild>
          <div>New user</div>
        </Button>
      </DialogTrigger>
      <DialogContent>
        <DialogHeader>
          <DialogTitle>New User</DialogTitle>
          <DialogDescription></DialogDescription>
        </DialogHeader>

        <form className="flex flex-col gap-6" onSubmit={handleSubmit}>
          <section className="flex gap-4">
            <div>
              <Label htmlFor="name">Name</Label>
              <Input name="name" maxLength={20} size={28} />
            </div>

            <div>
              <Label htmlFor="username">Username</Label>
              <Input name="username" maxLength={20} size={28} />
            </div>
          </section>

          <section className="flex gap-4">
            <div>
              <Label htmlFor="initials">Initials</Label>
              <Input name="initials" maxLength={6} size={8} />
            </div>

            <div>
              <Label htmlFor="email">Email</Label>
              <Input name="email" maxLength={35} size={48} />
            </div>
          </section>

          <section className="flex gap-4">
            <div>
              <Label htmlFor="password">Password</Label>
              <Input name="password" type="password" maxLength={20} size={28} />
            </div>

            <div>
              <Label htmlFor="confirmPassword">Confirm password</Label>
              <Input
                name="confirmPassword"
                type="password"
                maxLength={20}
                size={28}
              />
            </div>
          </section>

          <section className="flex justify-end">
            <Button>Create</Button>
          </section>
        </form>
      </DialogContent>
    </Dialog>
  );
}

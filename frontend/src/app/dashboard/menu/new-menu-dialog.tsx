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

import { Menu } from "@/types/menu";

import axios, { AxiosError } from "axios";
import { toast } from "sonner";

export type NewMenu = Omit<Menu, "id" | "createdAt" | "updatedAt"> & {
  password: string;
  confirmPassword?: string;
};

interface NewMenuDialogProps {
  onSubmit?: (newMenu: Menu) => void;
}

interface ErrorResponse {
  status: string;
  message: string;
}

export default function NewUserDialog({
  onSubmit = () => {},
}: NewMenuDialogProps) {
  const [open, setOpen] = useState(false);

  function handleSubmit(e: React.FormEvent<HTMLFormElement>) {
    e.preventDefault();

    const formData = new FormData(e.currentTarget);

    const newMenu = Object.fromEntries(formData) as unknown as NewMenu;

    if (newMenu.password !== newMenu.confirmPassword) {
      return toast.warning("Passwords do not match");
    }

    delete newMenu.confirmPassword;
    newMenu.password = Buffer.from(newMenu.password).toString("base64");

    toast.promise(axios.post("/api/users", newMenu), {
      loading: "Creating user...",
      success: () => {
        setOpen(false);

        onSubmit({
          id: Math.random().toString(),
          name: newMenu.name,
          price: newMenu.price,
          active: true,
          RawMaterial_MenuItems: [],
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

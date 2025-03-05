import { use, useEffect, useState } from "react";

import { Button } from "@/components/ui/button";
import { Checkbox } from "@/components/ui/checkbox";
import {
  Dialog,
  DialogContent,
  DialogDescription,
  DialogHeader,
  DialogTitle,
  DialogTrigger,
} from "@/components/ui/dialog";
import { Label } from "@/components/ui/label";

import { useUserContext } from "@/hooks/useUserContext";

import { client } from "@/axios";
import { Permission, PermissionGroup } from "@/types/permission";
import { User, UserPermission } from "@/types/user";

interface PermissionsDialogProps {
  user: User;
  onSave?: (userPermissions: UserPermission[]) => void;
}

const permissionPromise = client.get<{ data: PermissionGroup[] }>(
  "/api/permissionGroups",
  { validateStatus: (status) => status < 500 },
);

export default function PermissionsDialog({
  user,
  onSave = () => {},
}: PermissionsDialogProps) {
  const { user: currentUser } = useUserContext();

  const allPermissions = use(permissionPromise).data?.data;

  const [editablePermissions, setEditablePermissions] = useState<
    UserPermission[]
  >(user.UserPermissions || []);

  function handleCheckedChange(
    checked: boolean | string,
    permission: Permission,
  ) {
    if (typeof checked === "string") return;
    if (!currentUser) return;
    if (!allPermissions) return;

    const findPermission = allPermissions
      .flatMap((group) => group.Permissions)
      .find((perm) => perm.id === permission.id);

    if (!findPermission) return;

    const userPermission: UserPermission = {
      permissionId: findPermission.id,
      code: findPermission.code,
      description: findPermission.description,
      assignedBy: currentUser.sub,
    };

    setEditablePermissions((prev) => {
      if (checked) return [...prev, userPermission];

      return prev.filter((perm) => perm.permissionId !== permission.id);
    });
  }

  function handleSave(open: boolean) {
    if (open) return;
    if (editablePermissions === user.UserPermissions) return;
    onSave(editablePermissions);
  }

  useEffect(() => {
    setEditablePermissions(user.UserPermissions || []);
  }, [user.UserPermissions]);

  return (
    <Dialog onOpenChange={handleSave}>
      <DialogTrigger asChild>
        <Button variant="link">Open</Button>
      </DialogTrigger>
      <DialogContent className="w-96 max-w-none md:w-[50rem]">
        <DialogHeader>
          <DialogTitle>{user.name}</DialogTitle>
          <DialogDescription>Adjust permissions</DialogDescription>
        </DialogHeader>
        <ul className="grid grid-cols-1 gap-4 gap-x-40 md:grid-cols-2">
          {allPermissions?.map((permissionGroup) => {
            return (
              <li key={permissionGroup.id}>
                <h3 className="flex text-lg font-semibold">
                  {permissionGroup.name}
                </h3>
                <ul className="flex flex-col gap-1">
                  {permissionGroup.Permissions.map((permission) => {
                    const userPermission = editablePermissions?.find(
                      (userPerm) => userPerm.permissionId === permission.id,
                    );

                    return (
                      <li
                        key={permission.id}
                        className="flex items-center justify-between gap-2"
                      >
                        <Label htmlFor={permission.code}>
                          {permission.description}
                        </Label>
                        <Checkbox
                          id={permission.code}
                          name={permission.code}
                          checked={userPermission !== undefined}
                          onCheckedChange={(checked) =>
                            handleCheckedChange(checked, permission)
                          }
                        />
                      </li>
                    );
                  })}
                </ul>
              </li>
            );
          })}
        </ul>
      </DialogContent>
    </Dialog>
  );
}

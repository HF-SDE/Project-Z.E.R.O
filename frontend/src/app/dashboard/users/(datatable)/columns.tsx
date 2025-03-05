"use client";

import {
  ActionButtons,
  ButtonActions,
} from "@/components/datatable/action-buttons";
import { IsVisible } from "@/components/is-visible";

import { getHasPermission } from "@/helpers/getHasPermission";

import { CurrentUser, User, UserPermission } from "@/types/user";

import { ColumnDef } from "@tanstack/react-table";
import { Check, X } from "lucide-react";

import PermissionsDialog from "./permissions-dialog";

export function CreateColumns(
  user: CurrentUser,
  editing: User[],
  getButtonActions?: (user: User) => ButtonActions,
  savePermissions?: (id: string, permissions: UserPermission[]) => void,
): ColumnDef<User>[] {
  const hasUpdatePermission = getHasPermission(
    "administrator:users:update",
    user,
  );

  const columns: ColumnDef<User>[] = [
    { accessorKey: "initials", header: "Initials" },
    { accessorKey: "name", header: "Name" },
    { accessorKey: "email", header: "Email" },
    { accessorKey: "username", header: "Username" },
    {
      accessorKey: "active",
      header: "Is Active",
      cell: ({ row }) => (row.original.active ? <Check /> : <X />),
      meta: { editable: false },
    },
    {
      accessorKey: "permissions",
      header: "Permissions",
      cell: ({ row, column }) => {
        return (
          <IsVisible
            value={hasUpdatePermission}
            toggleVisibility={column.toggleVisibility}
          >
            <PermissionsDialog
              user={row.original}
              onSave={(permissions) =>
                savePermissions?.(row.original.id, permissions)
              }
            />
          </IsVisible>
        );
      },
      meta: { editable: false },
    },
    {
      header: "Actions",
      cell: ({ row, column }) => {
        const buttonActions = getButtonActions?.(row.original);

        return (
          <IsVisible
            value={hasUpdatePermission && Boolean(buttonActions)}
            toggleVisibility={column.toggleVisibility}
          >
            <ActionButtons
              data={row.original}
              editMode={editing.includes(row.original)}
              buttonActions={buttonActions}
              deletePermission={hasUpdatePermission}
              updatePermission={hasUpdatePermission}
            />
          </IsVisible>
        );
      },
      meta: { editable: false },
    },
  ];

  return columns;
}

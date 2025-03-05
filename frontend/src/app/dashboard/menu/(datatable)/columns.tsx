"use client";

import {
  ActionButtons,
  ButtonActions,
} from "@/components/datatable/action-buttons";
import { IsVisible } from "@/components/is-visible";

import { getHasPermission } from "@/helpers/getHasPermission";

import { Menu } from "@/types/menu";
import { CurrentUser } from "@/types/user";

import { ColumnDef } from "@tanstack/react-table";
import { Check, X } from "lucide-react";

export function CreateColumns(
  user: CurrentUser,
  editing: Menu[],
  getButtonActions?: (menu: Menu) => ButtonActions,
): ColumnDef<Menu>[] {
  const hasUpdatePermission = getHasPermission("menu:update", user);
  const hasDeletePermission = getHasPermission("menu:delete", user);

  const showActions = hasUpdatePermission || hasDeletePermission;

  const columns: ColumnDef<Menu>[] = [
    { accessorKey: "name", header: "Name" },
    { accessorKey: "price", header: "Price" },
    {
      accessorKey: "active",
      header: "Is Active",
      cell: ({ row }) => (row.original.active ? <Check /> : <X />),
      meta: { editable: false },
    },
    {
      header: "Actions",
      cell: ({ row, column }) => {
        const buttonActions = getButtonActions?.(row.original);

        return (
          <IsVisible
            value={showActions && Boolean(buttonActions)}
            toggleVisibility={column.toggleVisibility}
          >
            <ActionButtons
              data={row.original}
              editMode={editing.includes(row.original)}
              deletePermission={hasDeletePermission}
              updatePermission={hasUpdatePermission}
              buttonActions={buttonActions}
            />
          </IsVisible>
        );
      },
      meta: { editable: false },
    },
  ];

  return columns;
}

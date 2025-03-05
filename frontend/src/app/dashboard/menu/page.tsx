"use client";

import { useEffect, useState } from "react";

import CheckPermission from "@/components/check-permission";
import { DataTable } from "@/components/datatable/data-table";

import { useEndpoint } from "@/hooks/useEndpoint";
import { useUserContext } from "@/hooks/useUserContext";

import { client } from "@/axios";
import { type Menu } from "@/types/menu";

import { toast } from "sonner";

import { CreateColumns } from "./(datatable)/columns";
import NewMenuDialog from "./new-menu-dialog";

export default function Menu() {
  const { state, loading, error } = useEndpoint<Menu[]>("/api/menu");
  const { user } = useUserContext();

  const [editing, setEditing] = useState<Menu[]>([]);
  const [newMenu, setNewMenu] = useState<Menu | null>(null);

  const [originalData, setOriginalData] = useState<Menu[]>([]);

  function toggleEditing(menu: Menu, set?: boolean) {
    if (set === true || !editing.includes(menu)) setEditing([...editing, menu]);
    else setEditing(editing.filter(({ id }) => id !== menu.id));
  }

  function getButtonActions(menu: Menu) {
    return {
      edit: () => toggleEditing(menu, true),
      cancel: () => toggleEditing(menu, false),
      delete: () => deleteMenu(menu),
      save: () => saveMenu(menu),
    };
  }

  function deleteMenu(menu: Menu) {
    toast.promise(client.delete("/api/menu?id=" + menu.id), {
      loading: menu.active ? "Deactivating..." : "Reactivating...",
      success: () => {
        setOriginalData((prev) =>
          prev.map((data) =>
            data.id === menu.id ? { ...menu, active: !menu.active } : data,
          ),
        );
        return menu.active ? "Deactivated!" : "Reactivated!";
      },
      error: "Failed to delete.",
    });
  }

  function saveMenu(menu: Menu) {
    const rowToSave = editing.find((i) => i.id === menu.id);
    if (!rowToSave) throw new Error("Row to save not found");

    const savePromise = client.patch("/api/menu", rowToSave);

    toast.promise(savePromise, {
      loading: "Saving...",
      success: () => {
        setOriginalData((prev) =>
          prev.map((data) => (data.id === menu.id ? rowToSave : data)),
        );
        toggleEditing(menu, false);
        return "Saved!";
      },
      error: "Failed to save.",
    });
  }

  useEffect(() => {
    if (state) {
      setOriginalData(state.sort((a, b) => a.name.localeCompare(b.name)));
    }
  }, [state]);

  useEffect(() => {
    if (newMenu) setOriginalData((prev) => [...prev, newMenu]);
  }, [newMenu]);

  if (loading) return <div>Loading...</div>;
  if (error ?? false) return <div>{error}</div>;
  if (!user) return <div>No user found</div>;

  return (
    <div className="mx-auto py-10 md:mx-10">
      <div className="absolute right-4 top-4 flex gap-4">
        <CheckPermission permissionsNeeded="menu:create">
          <NewMenuDialog onSubmit={setNewMenu} />
        </CheckPermission>
      </div>
      <DataTable
        columns={CreateColumns(user, editing, getButtonActions)}
        data={originalData}
        editing={editing}
        setEditing={setEditing}
      />
    </div>
  );
}

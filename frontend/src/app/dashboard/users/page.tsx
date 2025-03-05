"use client";

import { useEffect, useState } from "react";

import CheckPermission from "@/components/check-permission";
import { DataTable } from "@/components/datatable/data-table";
import { Button } from "@/components/ui/button";

import { useUserContext } from "@/hooks/useUserContext";
import { useUsers } from "@/hooks/useUsers";

import { client } from "@/axios";
import { User, UserPermission } from "@/types/user";

import { toast } from "sonner";

import { CreateColumns } from "./(datatable)/columns";
import NewUserDialog from "./new-user-dialog";

export default function Users() {
  const { users, loading, error } = useUsers();
  const { user } = useUserContext();

  const [showInactive, setShowInactive] = useState(false);
  const [editing, setEditing] = useState<User[]>([]);
  const [newUser, setNewUser] = useState<User | null>(null);
  const [originalData, setOriginalData] = useState<User[]>([]);
  const [tableData, setTableData] = useState(originalData);

  function toggleEditing(user: User, set?: boolean) {
    if (set === true || !editing.includes(user)) setEditing([...editing, user]);
    else setEditing(editing.filter(({ id }) => id !== user.id));
  }

  function getButtonActions(user: User) {
    return {
      edit: () => toggleEditing(user, true),
      cancel: () => toggleEditing(user, false),
      delete: () => deleteUser(user),
      save: () => saveUser(user),
    };
  }

  function deleteUser(user: User) {
    toast.promise(
      client.put("/api/users", { id: user.id, active: !user.active }),
      {
        loading: user.active ? "Deactivating..." : "Reactivating...",
        success: () => {
          setOriginalData((prev) =>
            prev.map((data) =>
              data.id === user.id ? { ...user, active: !user.active } : data,
            ),
          );
          return user.active ? "Deactivated!" : "Reactivated!";
        },
        error: "Failed to delete.",
      },
    );
  }

  function saveUser(user: User) {
    const rowToSave = editing.find((i) => i.id === user.id);
    if (!rowToSave) throw new Error("Row to save not found");

    const savePromise = client.put("/api/users", user);

    toast.promise(savePromise, {
      loading: "Saving...",
      success: () => {
        setOriginalData((prev) =>
          prev.map((data) => (data.id === user.id ? rowToSave : data)),
        );
        toggleEditing(user, false);
        return "Saved!";
      },
      error: "Failed to save.",
    });
  }

  async function savePermissions(id: string, permissions: UserPermission[]) {
    toast.promise(
      client.put("/api/users", { id, UserPermissions: permissions }),
      {
        loading: "Saving permissions...",
        success: () => {
          setOriginalData((prev) =>
            prev.map((user) =>
              user.id === id ? { ...user, UserPermissions: permissions } : user,
            ),
          );
          return "Permissions saved!";
        },
        error: (err) => err.response.data.message,
      },
    );
  }

  useEffect(() => {
    if (users) setOriginalData(users);
  }, [users]);

  useEffect(() => {
    if (newUser) setOriginalData((prev) => [...prev, newUser]);
  }, [newUser]);

  useEffect(() => {
    const newData = originalData.filter((u) =>
      showInactive ? true : u.active,
    );

    setTableData(
      newData.sort((a, b) =>
        a.name.toLowerCase() > b.name.toLowerCase() ? 1 : -1,
      ),
    );
  }, [showInactive, originalData]);

  if (loading) return <div>Loading...</div>;
  if (error ?? false) return <div>{error}</div>;
  if (!user) return <div>No user found</div>;

  return (
    <div className="mx-auto py-10 md:mx-10">
      <div className="absolute right-4 top-4 flex gap-4">
        <Button
          variant="secondary"
          onClick={() => setShowInactive((prev) => !prev)}
        >
          {showInactive ? "Hide inactive" : "Show inactive"}
        </Button>
        <CheckPermission permissionsNeeded="administrator:users:create">
          <NewUserDialog onSubmit={setNewUser} />
        </CheckPermission>
      </div>
      <DataTable
        columns={CreateColumns(
          user,
          editing,
          getButtonActions,
          savePermissions,
        )}
        data={tableData}
        editing={editing}
        setEditing={setEditing}
      />
    </div>
  );
}

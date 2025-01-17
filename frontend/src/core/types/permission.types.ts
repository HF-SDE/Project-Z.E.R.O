export interface Permission {
    id: number;
    code: string;
    name: string;
    description: string;
    assignedBy: string;
}

export interface PermissionGroup {
    id?: string;
    name: string;
    Permissions: Permission[]
}

export interface PermissionWithGroup extends Permission {
    PermissionGroup: Omit<PermissionGroup, "Permissions">;
}
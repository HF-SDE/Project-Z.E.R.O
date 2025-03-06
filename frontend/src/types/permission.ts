export interface Permission {
  id: string;
  code: string;
  description: string;
  createdAt: Date | string;
  updatedAt: Date | string;
  permissionGroupId: string;
}

export interface PermissionGroup {
  id: string;
  name: string;
  createdAt: Date | string;
  updatedAt: Date | string;
  Permissions: Permission[];
}

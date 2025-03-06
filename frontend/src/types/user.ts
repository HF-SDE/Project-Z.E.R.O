export interface CurrentUser {
  sub: string;
  jti: string;
  name: string;
  username: string;
  permissions?: string[];
  initials: string;
  iat: number;
  exp: number;
}

export type User = {
  id: string;
  initials: string;
  name: string;
  email: string;
  username: string;
  active: boolean;
  createdAt: string;
  updatedAt: string;
  UserPermissions?: UserPermission[];
};

export interface UserPermission {
  assignedBy: string;
  code: string;
  description: string;
  permissionId: string;
}

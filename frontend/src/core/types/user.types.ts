import { PermissionGroup } from "./permission.types";

export interface IUser {
    id?: number;
    name: string;
    email: string;
    active: boolean;
    createdAt?: string;
    updatedAt?: string;
}


export interface IUserWithPermGroups extends IUser {
    groups: Omit<PermissionGroup, "Permissions">[]
}

export interface Create {
    visible: boolean;
    name: string;
    email: string;
    password: string;
}
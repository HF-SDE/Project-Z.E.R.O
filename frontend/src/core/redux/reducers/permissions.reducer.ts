import { createSlice, PayloadAction } from "@reduxjs/toolkit";
import { Permission } from "@types";
import { AppState } from "../store";

export interface IPermissionState {
    PERMISSIONS: Permission[]
    LOADING: boolean;
}

export const permissionSlice = createSlice({
    name: "permission",
    initialState: {
        PERMISSIONS: [],
        LOADING: false
    } as IPermissionState,
    reducers: {
        PERMISSIONS_RECIEVED(state: IPermissionState, { payload }: PayloadAction<{ PERMISSIONS: Permission[] }>) {
            state.PERMISSIONS = payload.PERMISSIONS;
        },
    },
});

export const selectPermission = (state: AppState) => state[permissionSlice.name];
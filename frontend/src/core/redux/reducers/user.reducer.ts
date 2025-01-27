import { createSlice, PayloadAction } from "@reduxjs/toolkit";
import { AppState } from "../store";

export interface IUserState {
	ID: number | null;
	EMAIL: string;
	NAME: string;
	TOKEN: string;
	AUTHENTICATED: boolean;
	LOADING: boolean;
	ERROR: string;
}
const initialState: IUserState = {
	ID: null,
	EMAIL: "",
	NAME: "",
	TOKEN: "",
	AUTHENTICATED: false,
	LOADING: false,
	ERROR: "",
};

export const userSlice = createSlice({
    name: "user",

    initialState: {
		ID: null,
		EMAIL: "",
		NAME: "",
		TOKEN: "",
		AUTHENTICATED: false,
		LOADING: false,
		ERROR: "",
	} as IUserState,

    reducers: {
		USER_DATA_RECIEVED(state: IUserState, { payload }: PayloadAction<{ ID: number, EMAIL: string, NAME: string }>) {
			state.EMAIL = payload.EMAIL;
			state.ID = payload.ID;
			state.NAME = payload.NAME;
			state.LOADING = false;
		},
		ACCESS_TOKEN_RECIEVED(state: IUserState, { payload }: PayloadAction<{ TOKEN: string }>) {
			state.TOKEN = payload.TOKEN;
			state.AUTHENTICATED = true;
			state.LOADING = false;
		},
		LOGOUT(state: IUserState) {
			state.AUTHENTICATED = initialState.AUTHENTICATED;
			state.ID = initialState.ID;
			state.NAME = initialState.NAME;
			state.LOADING = initialState.LOADING;
			state.ERROR = initialState.ERROR;
			state.TOKEN = initialState.TOKEN;
			state.EMAIL = initialState.EMAIL;
		},
		AUTH_FAILED(state: IUserState, { payload }: PayloadAction<{ ERROR: string }>) {
			state.ERROR = payload.ERROR;
			state.LOADING = false;
		},
		AUTHENTICATING(state: IUserState) {
			state.LOADING = true;
		}
	},
});

export const selectUser = (state: AppState) => state[userSlice.name];

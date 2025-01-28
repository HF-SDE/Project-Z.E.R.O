import { ActionCreator } from "redux";
import backendAxios from "core/api/backendAxios";
import { permissionSlice } from "../reducers/permissions.reducer";
import { IUser } from "@types";
import { AppThunkAction } from "../store";
import { userSlice } from "../reducers/user.reducer";


/**
 * Logs in a user and retrieves their data and permissions
 *
 * @param {string} username 
 * @param {string} password 
 * @returns {(dispatch: any) => any} 
 */
export const loginUser: ActionCreator<AppThunkAction> = (username: string, password: string) => async (dispatch) => {
	let accessToken = "";
	try {

		dispatch(userSlice.actions.AUTHENTICATING());

		const authResponse = await backendAxios.post(`/login`, {
			username,
			password: btoa(password),
		});

		dispatch(
			userSlice.actions.ACCESS_TOKEN_RECIEVED({ TOKEN: `${authResponse.data.data.accessToken.authType} ${authResponse.data.data.accessToken.token}` })
		);
		backendAxios.defaults.headers.common.Authorization = `${authResponse.data.data.accessToken.authType} ${authResponse.data.data.accessToken.token}`;
		accessToken = `${authResponse.data.data.accessToken.authType} ${authResponse.data.data.accessToken.token}`;
	} catch (error: any) {
		if (error.response) {
			console.warn('Authentication failed', error.response);
		}

		dispatch(
			userSlice.actions.AUTH_FAILED({
				ERROR: "Username or password is wrong.",
			})
		);
		return;
	}

	try {

		const userResponse = await backendAxios.get("/profile", {
			headers: {
				Authorization: accessToken,
			},
		});

		dispatch(
			userSlice.actions.USER_DATA_RECIEVED({
				EMAIL: userResponse.data.data.email,
				NAME: userResponse.data.data.name,
				ID: userResponse.data.data.id,
			})
		);

		dispatch(
			permissionSlice.actions.PERMISSIONS_RECIEVED({
				PERMISSIONS: userResponse.data.data.permissions
			})
		);
		return;
	} catch (error: any) {
		console.warn("Login failed because we couldn't get user information", error.response);

		dispatch(
			userSlice.actions.AUTH_FAILED({
				ERROR: "Couldn't get user information, Contact our support",
			})
		);
		return;
	}
};

export const retrievedAccessToken: ActionCreator<AppThunkAction> = (accessToken: string) => async (dispatch) => {
	dispatch(userSlice.actions.ACCESS_TOKEN_RECIEVED({ TOKEN: accessToken }));
};

export const retrievedUserData: ActionCreator<AppThunkAction> = (userData: IUser) => async (dispatch) => {
	dispatch(
		userSlice.actions.USER_DATA_RECIEVED({
			ID: userData.id!,
			EMAIL: userData.email,
			NAME: userData.name
		})
	);
};

export const clearUserState: ActionCreator<AppThunkAction> = () => async (dispatch) => {
	dispatch(userSlice.actions.LOGOUT());
};
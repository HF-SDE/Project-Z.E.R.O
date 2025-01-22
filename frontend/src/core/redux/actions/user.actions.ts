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
		}, { withCredentials: true });

		dispatch(
			userSlice.actions.ACCESS_TOKEN_RECIEVED({ TOKEN: authResponse.data.accessToken })
		);
		backendAxios.defaults.headers.common.Authorization = authResponse.data.accessToken;
		accessToken = authResponse.data.accessToken;
	} catch (error: any) {
		if (error.response) {
			console.warn('Authentication failed', error.response);
		}

		dispatch(
			userSlice.actions.AUTH_FAILED({
				ERROR: "Username eller adgangskode var forkert.",
			})
		);
		return;
	}

	try {
		const userResponse = await backendAxios.get("user/fromauth", {
			headers: {
				Authorization: accessToken,
			},
		});

		dispatch(
			userSlice.actions.USER_DATA_RECIEVED({
				EMAIL: userResponse.data.user.email,
				NAME: userResponse.data.user.name,
				ID: userResponse.data.user.id,
			})
		);
	} catch (error: any) {
		console.warn("Login failed because we couldn't get user information", error.response);

		dispatch(
			userSlice.actions.AUTH_FAILED({
				ERROR: "Kunne ikke hente bruger information, Contact our support",
			})
		);
		return;
	}

	try {

		const permResponse = await backendAxios.get('permission/fromAuth', {
			headers: {
				Authorization: accessToken
			}
		});

		dispatch(
			permissionSlice.actions.PERMISSIONS_RECIEVED({
				PERMISSIONS: permResponse.data.data
			})
		);

	} catch (error: any) {
		console.warn("Login failed because we couldn't get users permissions", error.response);

		dispatch(
			userSlice.actions.AUTH_FAILED({
				ERROR: "Kunne ikke hente bruger rettigheder, Contact our support",
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
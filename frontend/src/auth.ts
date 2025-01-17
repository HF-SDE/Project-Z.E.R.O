import { serialize } from "cookie";
import { IncomingMessage, ServerResponse } from "node:http";
import backendAxios from "core/api/backendAxios";
import { retrievedAccessToken, retrievedUserData } from "core/redux/actions/user.actions";

interface AuthenticateResponse {
	authenticated: boolean;
	message: string;
	errorCode?: string; // An error code to find specific place for the error.
}

/**
 * Used for authenticateUser
 * @async
 * @param {IncomingMessage} req
 * @param {ServerResponse} res
 * @param {*} dispatch
 * @returns {Promise<AuthenticateResponse>}
 */
export async function authenticateUser(
	req: IncomingMessage,
	res: ServerResponse,
	dispatch: any
): Promise<AuthenticateResponse> {
	const refreshToken = getRefreshTokenFromReq(req);
	if (!refreshToken) {
		backendAxios.defaults.headers.common.Authorization = "";
		return {
			authenticated: false,
			message: "No refresh token was found in the browser.",
			errorCode: "#ATH01",
		};
	}

	try {
		const response = await backendAxios.get("auth/refresh", {
			headers: {
				cookie: serialize("refreshToken", refreshToken, {
					path: "/",
					httpOnly: true
				}),
			},
		});

		dispatch(retrievedAccessToken(response.data.accessToken));
		backendAxios.defaults.headers.common.Authorization = response.data.accessToken;

		const userInfo = await backendAxios.get("user/fromauth", {
			headers: {
				Authorization: response.data.accessToken,
			},
		});
		if (userInfo.status === 200) {
			dispatch(retrievedUserData(userInfo.data.user));
			res.setHeader("Authorization", response.data.accessToken);
			return {
				authenticated: true,
				message: "User has been authenticated",
			};
		}

		return {
			authenticated: false,
			message: "Getting user information has failed, unauthenticating the user.",
		};
	} catch (error: any) {
		console.warn('Critical error in authenticating', error);

		setRefreshTokenToRes(res, null, true);
		backendAxios.defaults.headers.common.Authorization = "";
		return {
			authenticated: false,
			message: error.response ? error.response.data.message : "Internal server error",
			errorCode: "#ATH02",
		};
	}
}

/**
 * Used for getting a RefreshToken
 * @param {*} req
 * @returns {*}
 */
export function getRefreshTokenFromReq(req: any) {
	if (!req.headers.cookie) return null;
	const refreshToken = req.headers.cookie.split(";").find((c: string) => c.trim().startsWith("refreshToken="));
	if (!refreshToken) return null;

	return refreshToken.split("=")[1];
}

/**
 * Used for setting RefreshToken
 * @param {ServerResponse} res
 * @param {(string | null)} refreshToken
 * @param {boolean} [remove=false]
 */
export function setRefreshTokenToRes(res: ServerResponse, refreshToken: string | null, remove = false) {
	if (remove) {
		res.setHeader("set-cookie", [
			serialize("refreshToken", "", {
				path: "/",
				maxAge: -1,
			}),
		]);
	} else {
		if (refreshToken !== null) {
			res.setHeader("set-cookie", [
				serialize("refreshToken", refreshToken, {
					path: "/",
					httpOnly: true,
				}),
			]);
		}
	}
}

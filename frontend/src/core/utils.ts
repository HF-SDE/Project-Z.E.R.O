import { NextRouter } from "next/router";
import { Popup, uiSlice } from "./redux/reducers/ui.reducer";
import { Status } from "./types/api.types";
import { Dispatch } from "@reduxjs/toolkit";
import { isAxiosError } from "axios";

type TPopup = Omit<Popup, "lifeSpanMs">

/**
 * Refreshes the current route - does not re-render all components or clear state, but it refreshes props and therefore SSR.
 * @param {NextRouter} router Router component from useRouter() hook
 */
export function refreshData(router: NextRouter) {
	router.replace(router.asPath);
}

interface APIErrorResponse {
	response: {
		data: {
			message: string,
			status: Status
		},
		status: number;
		statusText: string;
		request: {
			responseUrl: string;
		}
	}
}

/**
 *
 */
function isPopup(obj: any): obj is TPopup {
	const array = ["text", "title", "type"];
	return array.every(key => obj.hasOwnProperty(key));
}

interface HandleAPIResponseWithAll {
	popup: TPopup,
	dispatch: Dispatch<any>,
	lifeSpanMs?: number,
	error?: APIErrorResponse
}

export function HandleAPIResponse(popup: TPopup, dispatch: Dispatch<any>, error?: APIErrorResponse): void;
export function HandleAPIResponse(popup: TPopup, dispatch: Dispatch<any>, lifeSpanMs?: number, error?: APIErrorResponse): void;
/**
 * Used in api call returns
 * @param {Omit<Popup, "lifeSpanMs">} popup Is the object with title, text and type
 * @param {Dispatch<any>} dispatch useDispatch from redux
 * @param {APIErrorResponse} [error] [Optional]
 * @param {number} [lifeSpanMs] [Optional] - How long should notification being showed. Default value is "10000".
 * ```js
 * // It can be used like this
 * HandleAPIResponse({ title: 'Some title', text: 'Some text', type: 'error' }, dispatch, error);
 * 
 * // Or with lifeSpanMs like this
 * HandleAPIResponse({ title: 'Some title', text: 'Some text', type: 'error' }, dispatch, 5000, error);
 * ```
 */
export function HandleAPIResponse(..._arg: any): void {
	const rv: any = {};
	for (let i = 0; i < (_arg as any[]).length; ++i) {
		if (isPopup(_arg[i])) {
			rv.popup = _arg[i];
			continue;
		}
		if (Number(_arg[i])) {
			rv.lifeSpanMs = _arg[i];
			continue;
		}
		if (isAxiosError(_arg[i])) {
			rv.error = _arg[i];
			continue;
		}
		if (_arg[i] as Dispatch<any>) {
			rv.dispatch = _arg[i];
			continue;
		}
	}
	const { popup, dispatch, error, lifeSpanMs } = (rv as HandleAPIResponseWithAll);
	let concatinatedText = popup.text;
	if (error && error.response) {
		console.error(`[APIResponseHandler] API ${error.response.request.responseUrl} has failed`, error);
		concatinatedText += ` (${error.response.data.message})`;
	} else if (error && !error.response) {
		concatinatedText += ` (${error})`;
		console.error(`[APIResponseHandler] API ${error} has failed`, error);
	}
	dispatch(uiSlice.actions.ADD_POPUP({ popup: { lifeSpanMs: lifeSpanMs ?? 10000, text: concatinatedText, title: popup.title, type: popup.type } }));
}
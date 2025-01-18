import { ToastMessageVariant } from "@components/Toast/ToastMessage";
import { createSlice, PayloadAction } from "@reduxjs/toolkit";
import { AppState } from "../store";

export interface Popup {
	id?: string;
	title: string;
	text: string;
	type: ToastMessageVariant
	lifeSpanMs: number;
}

export interface IUIState {
	sidebarVisible: boolean;
	popups: Popup[]
}

export const uiSlice = createSlice({
	name: "ui",
	initialState: {
		sidebarVisible: true,
		popups: []
	} as IUIState,
	reducers: {
		TOGGLE_SIDEBAR(state: IUIState, { payload }: PayloadAction<{ state: boolean }>) {
			state.sidebarVisible = payload.state;
		},
		ADD_POPUP(state: IUIState, { payload }: PayloadAction<{ popup: Popup }>) {
			let id = "";
			if (payload.popup.id === undefined || !payload.popup.id) id = Math.floor(Math.random() * 10000).toString();
			else id = payload.popup.id;
			state.popups.push({
				...payload.popup,
				id: id
			});
		},
		REMOVE_POPUP(state: IUIState, { payload }: PayloadAction<{ popup: Popup }>) {
			const filtered = state.popups.filter(e => e.id !== payload.popup.id);
			state.popups = filtered;
		}
	},
});

export const selectUI = (state: AppState) => state[uiSlice.name];
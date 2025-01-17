import { AnyAction, combineReducers } from "redux";
import { ThunkAction } from "redux-thunk";
import { createWrapper } from "next-redux-wrapper";
import { configureStore } from "@reduxjs/toolkit";
import { FLUSH, PAUSE, PERSIST, persistReducer, PURGE, REGISTER, REHYDRATE } from "redux-persist";
import { uiSlice } from "./reducers/ui.reducer";
import { userSlice } from "./reducers/user.reducer";
import { permissionSlice } from "./reducers/permissions.reducer";
import storage from "redux-persist/lib/storage";

const persistConfig = {
	key: "iot",
	version: 1,
	storage,
	whitelist: ["ui", "permission"],
};

const rootReducer = combineReducers({
	[uiSlice.name]: uiSlice.reducer,
	[userSlice.name]: userSlice.reducer,
	[permissionSlice.name]: permissionSlice.reducer
});

const persistedReducer = persistReducer(persistConfig, rootReducer);

export const makeStore = () =>
	configureStore({
		reducer: persistedReducer,
		middleware: (getDefaultMiddleware) =>
			getDefaultMiddleware({
				serializableCheck: {
					ignoredActions: [FLUSH, REHYDRATE, PAUSE, PERSIST, PURGE, REGISTER],
				},
			}),
		devTools: true,
	});

export type AppStore = ReturnType<typeof makeStore>;
export type AppState = ReturnType<AppStore["getState"]>;
export type AppThunkAction<ReturnType = Promise<void>> = ThunkAction<ReturnType, AppState, unknown, AnyAction>;

export const wrapper = createWrapper<AppStore>(makeStore, { debug: false });

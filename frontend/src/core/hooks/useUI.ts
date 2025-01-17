import { useSelector } from "react-redux";
import { selectUI } from "../redux/reducers/ui.reducer";

/**
 * Used by redux
 * 
 * @returns 
 */
export function useUI() {
	const ui = useSelector(selectUI);
	return ui;
}

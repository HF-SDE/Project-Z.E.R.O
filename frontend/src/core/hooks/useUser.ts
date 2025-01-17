import { useSelector } from "react-redux";
import { selectUser } from "../redux/reducers/user.reducer";

/**
 * Used by redux
 * 
 * @returns 
 */
export function useUser() {
	const user = useSelector(selectUser);
	return user;
}

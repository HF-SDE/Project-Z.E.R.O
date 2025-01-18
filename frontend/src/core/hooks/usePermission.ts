import { selectPermission } from "core/redux/reducers/permissions.reducer";
import { useSelector } from "react-redux";

/**
 * Used for get the selectPermissions
 * 
 * @returns 
 */
export function usePermission() {
    return useSelector(selectPermission);
}

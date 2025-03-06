import { getHasPermission } from "@/helpers/getHasPermission";
import { useUserContext } from "@/hooks/useUserContext";

interface CheckPermissionProps {
  permissionsNeeded?: string | string[];
  children: React.ReactNode;
}

export default function CheckPermission({
  children,
  permissionsNeeded,
}: CheckPermissionProps) {
  const { user } = useUserContext();
  const hasPermission = getHasPermission(permissionsNeeded, user);
  return hasPermission ? children : null;
}

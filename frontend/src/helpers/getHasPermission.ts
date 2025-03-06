import { CurrentUser } from "@/types/user";

export function getHasPermission(
  permission?: string | string[],
  user?: CurrentUser,
): boolean {
  if (!permission || permission.length === 0) return true;

  if (!user) return false;

  const permissionsNeeded = Array.isArray(permission)
    ? permission
    : [permission];

  const hasPermission = user.permissions?.some((permission) =>
    permissionsNeeded.includes(permission),
  );

  return hasPermission ?? false;
}

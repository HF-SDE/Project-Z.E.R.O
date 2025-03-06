import { NextRequest, NextResponse } from "next/server";

import { getCookies } from "./helpers/cookies";
import { getUser } from "./helpers/getUser";

const protectedRoutes = [
  { route: "/dashboard", permissionsNeeded: ["administrator:dashboard:view"] },
];
const publicRoutes = ["/login"];

export default async function middleware(req: NextRequest) {
  const path = req.nextUrl.pathname;
  const isProtected = protectedRoutes.some(({ route }) =>
    path.startsWith(route),
  );
  const isPublic = publicRoutes.includes(path);
  const token = await getCookies("token");

  function redirect(url: string) {
    return NextResponse.redirect(new URL(url, req.nextUrl));
  }

  const hasPermission = await getHasPermission(path);

  if (isProtected && !token && !hasPermission) return redirect("/login");
  if (isPublic && token && (await getHasPermission("/dashboard")))
    return redirect("/dashboard");

  return NextResponse.next();
}

export const config = {
  matcher: ["/((?!api|_next/static|_next/image|.*\\.png$).*)"],
};

async function getHasPermission(path: string) {
  const currentUser = await getUser();

  if (!currentUser) return false;

  const protectedRoute = protectedRoutes.find(({ route }) =>
    path.startsWith(route),
  );

  if (!protectedRoute) return true;

  const hasPermission = currentUser.permissions?.some((permission) =>
    protectedRoute.permissionsNeeded.includes(permission),
  );

  return hasPermission;
}

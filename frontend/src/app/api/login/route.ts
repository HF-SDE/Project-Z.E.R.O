import { cookies } from "next/headers";
import { NextRequest, NextResponse } from "next/server";

import { getHasPermission } from "@/helpers/getHasPermission";
import { getUser } from "@/helpers/getUser";

import axios from "@/axios";

export async function POST(req: NextRequest) {
  const body = await req.json();
  const cookieStore = await cookies();

  const tokenResponse = await axios.post("/login", body).catch((error) => {
    return error.response;
  });

  if (tokenResponse.status === 200) {
    const token = tokenResponse.data.data.accessToken.token;

    const hasPermission = getHasPermission(
      "administrator:dashboard:login",
      await getUser(token),
    );

    if (!hasPermission) return new NextResponse("", { status: 403 });

    cookieStore.set("token", token);

    return new NextResponse(JSON.stringify(tokenResponse.data), {
      status: 200,
    });
  }

  return new NextResponse(JSON.stringify(tokenResponse.data), { status: 400 });
}

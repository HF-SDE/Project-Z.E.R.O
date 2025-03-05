import { NextResponse } from "next/server";

import apiClient from "@/util/apiClient";

export async function GET() {
  const { data, status } = await apiClient.get("manage/permissionGroups");

  const newReponse = NextResponse.json(data, { status });

  return newReponse;
}

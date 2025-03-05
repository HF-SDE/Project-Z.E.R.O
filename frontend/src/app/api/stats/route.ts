import { NextResponse } from "next/server";

import apiClient from "@/util/apiClient";

export async function GET() {
  const { data, status } = await apiClient.get("/stats");

  return new NextResponse(JSON.stringify(data), { status });
}

import { NextRequest, NextResponse } from "next/server";

import apiClient from "@/util/apiClient";

export async function GET() {
  const { data, status } = await apiClient.get("/menu");

  const newReponse = NextResponse.json(data, { status });

  return newReponse;
}

export async function DELETE(req: NextRequest) {
  const id = req.nextUrl.searchParams.get("id");

  const { data, status } = await apiClient.delete(`menu/${id}`);

  const newReponse = NextResponse.json(data, { status });

  return newReponse;
}

export async function PATCH(req: NextRequest) {
  const updatedMenu = await req.json();

  const id = updatedMenu.id;

  delete updatedMenu.id;

  const { data, status } = await apiClient.patch(`menu/${id}`, updatedMenu);

  const newReponse = NextResponse.json(data, { status });

  return newReponse;
}

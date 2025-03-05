import { NextRequest, NextResponse } from "next/server";

import apiClient from "@/util/apiClient";

export async function GET() {
  const { data, status } = await apiClient.get("manage/user");

  const newReponse = NextResponse.json(data, { status });

  return newReponse;
}

export async function POST(req: NextRequest) {
  const newUser = await req.json();

  const { data, status } = await apiClient.post("manage/user", newUser);

  const newReponse = new NextResponse(JSON.stringify(data), { status });

  return newReponse;
}

export async function PUT(req: NextRequest) {
  const updatedUser = await req.json();

  const id = updatedUser.id;

  delete updatedUser.id;

  const { data, status } = await apiClient.put(
    `manage/user/${id}`,
    updatedUser,
  );

  const newReponse = new NextResponse(JSON.stringify(data), { status });

  return newReponse;
}

export async function DELETE(req: NextRequest) {
  const id = req.nextUrl.searchParams.get("id");

  const { data, status } = await apiClient.delete(`manage/user/${id}`);

  const newReponse = new NextResponse(JSON.stringify(data), { status });

  return newReponse;
}

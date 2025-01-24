import { Card } from "@components/ui/card";
import { backendCall } from "core/api/backendAxios";
import { useEffect } from "react";

export default async function Device() {
  const res = await backendCall("device", "GET");

  return (
    <main>
      <Card className="p-4"></Card>
    </main>
  );
}

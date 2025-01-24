import { Card } from "@components/ui/card";
import { backendCall } from "core/api/backendAxios";
import { useEffect } from "react";

export default async function Device() {
  const data = await backendCall("device", "GET");

  return (
    <main>
      {data.data?.map((device) => {
        return (
          <Card key={device.id} className="p-4">
            <h2>{device.name}</h2>
            <p>{device.description}</p>
          </Card>
        );
      })}
    </main>
  );
}

"use client";

import * as React from "react";

import { Card, CardTitle } from "@/components/ui/card";
import { Button } from "@/components/ui/button";
import { Badge } from "@/components/ui/badge";
import { Separator } from "@/components/ui/separator";
import {
  Sheet,
  SheetContent,
  SheetDescription,
  SheetFooter,
  SheetHeader,
  SheetTitle,
} from "@/components/ui/sheet";
import { Input } from "@/components/ui/input";
import { Label } from "@/components/ui/label";
import {
  Select,
  SelectContent,
  SelectItem,
  SelectTrigger,
  SelectValue,
} from "@/components/ui/select";
import mqtt from "mqtt";

import { Data, mergeTopicIntoResult } from "./helper.function";
import { MiddleCard } from "./middle-card";

export type Id = string;

export type CreateKind = "location" | "zone" | "door" | "client" | "component";

function uid(prefix: string) {
  return `${prefix}_${Math.random().toString(16).slice(2, 10)}`;
}

export function useMqttClient() {
  return React.useMemo(() => {
    const clnt = mqtt
      .connect("ws://127.0.0.1:9001")
      .on("connect", () => console.log("Connected to MQTT broker"));

    return clnt;
  }, []);
}

interface ClientPageProps {
  preLocation?: string;
  preZone?: string;
  initData?: Data;
}

export default function SystemClientPage({
  preLocation,
  preZone,
  initData = { locations: [], clients: [] },
}: ClientPageProps) {
  const mqttClient = useMqttClient();

  const [data, setData] = React.useState<Data>(initData);

  const [locationId, setLocationId] = React.useState<Id | undefined>(
    preLocation
  );
  const [zoneId, setZoneId] = React.useState<Id | undefined>(preZone);

  function handleLocationChange(id: string) {
    setLocationId(id);
    setZoneId(undefined);

    window.history.replaceState(null, "", `?location=${id}`);
  }

  function handleZoneChange(id: string) {
    setZoneId(id);

    const params = new URLSearchParams(window.location.search);
    params.set("zone", id);
    window.history.replaceState(
      null,
      "",
      `${window.location.pathname}?${params.toString()}`
    );
  }

  React.useEffect(() => {
    const topic = "#";

    mqttClient.subscribe(topic);

    const onMessage = (fullTopic: string, message: Buffer) => {
      const text = message.toString();
      const payload = text ? JSON.parse(text) : null;

      setData((prev) => mergeTopicIntoResult(prev, fullTopic, payload));
    };

    mqttClient.on("message", onMessage);

    return () => {
      mqttClient.off("message", onMessage);
      mqttClient.unsubscribe(topic);
    };
  }, [mqttClient]);

  const [clientId, setClientId] = React.useState<Id>("");

  const [createOpen, setCreateOpen] = React.useState(false);
  const [createKind, setCreateKind] = React.useState<CreateKind>("location");
  const [name, setName] = React.useState("");

  function openCreate(kind: CreateKind) {
    setCreateKind(kind);
    setName("");
    setCreateOpen(true);
  }

  function canCreate() {
    if (!name.trim()) return false;
    if (createKind === "zone" && !locationId) return false;
    if ((createKind === "door" || createKind === "client") && !zoneId)
      return false;
    if (createKind === "component" && !clientId) return false;
    return true;
  }

  function submitCreate() {
    const n = name.trim();
    if (!n) return;

    let topic = "";

    if (createKind === "location") topic = `locations/${uid("loc")}/config`;

    if (createKind === "zone")
      topic = `locations/${locationId}/zones/${uid("zone")}/config`;

    if (createKind === "door")
      topic = `locations/${locationId}/zones/${zoneId}/doors/${uid(
        "door"
      )}/config`;

    if (createKind === "client")
      topic = `locations/${locationId}/zones/${zoneId}/clients/${uid(
        "dev"
      )}/config`;

    if (createKind === "component")
      topic = `clients/${clientId}/${"temperature"}/config`;

    mqttClient.publish(topic, JSON.stringify({ name: n }), { retain: true });

    setCreateOpen(false);
  }

  const location = data.locations.find((l) => l.id === locationId);
  const zone = location?.zones?.find((z) => z.id === zoneId);
  const client = data.clients?.find((c) => c.id === clientId);

  return (
    <div className="p-4">
      <div className="flex flex-col gap-2 sm:flex-row sm:items-center sm:justify-between">
        <div>
          <h1 className="text-xl font-semibold">System Overview</h1>
          <p className="text-sm text-muted-foreground">
            Pick a location + zone, then see what’s inside. Create new items
            from the buttons.
          </p>
        </div>

        <div className="flex gap-2">
          <Button variant="outline" onClick={() => openCreate("location")}>
            + Location
          </Button>
          <Button onClick={() => openCreate("zone")} disabled={!locationId}>
            + Zone
          </Button>
        </div>
      </div>

      <Separator className="my-4" />

      <div className="grid gap-4 grid-cols-4">
        {/* Left: very simple selectors */}
        <Card className="p-4">
          <div className="space-y-4">
            <div>
              <div className="mb-2 text-sm font-medium">Location</div>
              <Select
                value={locationId || ""}
                onValueChange={handleLocationChange}
              >
                <SelectTrigger>
                  <SelectValue placeholder="Select a location" />
                </SelectTrigger>
                <SelectContent>
                  {data.locations.map((l) => (
                    <SelectItem key={l.id} value={l.id}>
                      {l.config.name}
                    </SelectItem>
                  ))}
                </SelectContent>
              </Select>
            </div>

            <div>
              <div className="mb-2 text-sm font-medium">Zone</div>
              <Select
                value={zoneId || ""}
                onValueChange={handleZoneChange}
                disabled={!locationId || location?.zones.length === 0}
              >
                <SelectTrigger>
                  <SelectValue
                    placeholder={
                      !location
                        ? "Select a location first"
                        : location.zones.length
                        ? "Select a zone"
                        : "No zones yet"
                    }
                  />
                </SelectTrigger>
                <SelectContent>
                  {location?.zones.map((z) => (
                    <SelectItem key={z.id} value={z.id}>
                      {z.config.name}
                    </SelectItem>
                  ))}
                </SelectContent>
              </Select>

              <div className="mt-3 flex gap-2">
                <Button
                  variant="outline"
                  onClick={() => openCreate("door")}
                  disabled={!zoneId}
                >
                  + Door
                </Button>
                <Button
                  variant="outline"
                  onClick={() => openCreate("client")}
                  disabled={!zoneId}
                >
                  + Client
                </Button>
              </div>
            </div>

            <Separator />

            <div className="rounded-lg border p-3">
              <div className="text-sm font-medium">Current selection</div>
              <div className="mt-2 space-y-2 text-sm">
                <div className="flex items-center justify-between">
                  <span className="text-muted-foreground">Location</span>
                  <span className="font-medium">
                    {location?.config?.name ?? "—"}
                  </span>
                </div>
                <div className="flex items-center justify-between">
                  <span className="text-muted-foreground">Zone</span>
                  <span className="font-medium">
                    {zone?.config?.name ?? "—"}
                  </span>
                </div>
              </div>
              <div className="mt-3 flex flex-wrap gap-2">
                <Badge variant="secondary">{zone?.doors?.length} doors</Badge>
                <Badge variant="secondary">
                  {zone?.clients?.length} clients
                </Badge>
              </div>
            </div>
          </div>
        </Card>

        {/* Right: simple tabs */}
        <Card className="p-4 col-span-2">
          <MiddleCard
            locationId={locationId}
            zone={zone}
            client={client}
            setClientId={setClientId}
            mqttClient={mqttClient}
            openCreate={openCreate}
          />
        </Card>

        <Card className="p-4">
          <CardTitle className="mb-4">Unassigned Clients</CardTitle>

          <Separator className="mb-4" />

          {data.clients
            .filter((c) => c.locationId === null)
            .map((c) => (
              <div
                key={c.id}
                className="flex justify-between items-center rounded-md"
                onClick={() => setClientId(c.id)}
              >
                <p>{c.config.name || c.id}</p>

                <Button
                  variant="ghost"
                  size="sm"
                  onClick={(e) => {
                    e.stopPropagation();
                    
                    
                  }}
                >
                  Assign location
                </Button>
              </div>
            ))}
        </Card>
      </div>

      {/* Create sheet */}
      <Sheet open={createOpen} onOpenChange={setCreateOpen}>
        <SheetContent className="sm:max-w-[420px]">
          <SheetHeader>
            <SheetTitle>
              Create <span className="capitalize">{createKind}</span>
            </SheetTitle>
            <SheetDescription>
              Fill in the details below to create a new {createKind}.
            </SheetDescription>
          </SheetHeader>

          <div className="mt-4 space-y-4">
            <div className="rounded-lg border p-3 text-sm">
              <div className="font-medium">Context</div>
              <div className="mt-2 space-y-1 text-muted-foreground">
                <div>Location: {location?.config.name ?? "—"}</div>
                <div>Zone: {zone?.config.name ?? "—"}</div>
                <div>Client: {client?.config.name ?? "—"}</div>
              </div>
            </div>

            <div className="space-y-2">
              <Label>Name</Label>
              <Input
                value={name}
                onChange={(e) => setName(e.target.value)}
                placeholder="Enter a name…"
              />
            </div>

            <div className="text-xs text-muted-foreground">
              {createKind === "zone" &&
                !locationId &&
                "Select a location first."}
              {(createKind === "door" || createKind === "client") &&
                !zoneId &&
                "Select a zone first."}
              {createKind === "component" &&
                !clientId &&
                "Select a client first."}
            </div>
          </div>

          <SheetFooter className="mt-6">
            <Button variant="outline" onClick={() => setCreateOpen(false)}>
              Cancel
            </Button>
            <Button disabled={!canCreate()} onClick={submitCreate}>
              Create
            </Button>
          </SheetFooter>
        </SheetContent>
      </Sheet>
    </div>
  );
}

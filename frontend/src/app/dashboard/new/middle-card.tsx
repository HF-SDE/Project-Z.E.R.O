import { Button } from "@/components/ui/button";

import {
  Select,
  SelectContent,
  SelectItem,
  SelectTrigger,
  SelectValue,
} from "@/components/ui/select";
import { Tabs, TabsContent, TabsList, TabsTrigger } from "@/components/ui/tabs";
import { useState } from "react";
import {
  Dialog,
  DialogClose,
  DialogContent,
  DialogDescription,
  DialogTitle,
  DialogTrigger,
} from "@/components/ui/dialog";
import { CreateKind, Id } from "./client";
import mqtt from "mqtt";
import { Base, Client, Zone } from "./helper.function";
import { Label } from "@/components/ui/label";
import { Input } from "@/components/ui/input";

export function MiddleCard({
  locationId,
  zone,
  mqttClient,
  openCreate,
  setClientId,
  client,
}: {
  locationId?: Id;
  zone?: Zone;
  mqttClient: mqtt.MqttClient;
  openCreate: (kind: CreateKind) => void;
  setClientId: (id: string) => void;
  client?: Client;
}) {
  type TabType = "doors" | "clients" | "components";

  const [activeTab, setActiveTab] = useState<TabType>("doors");

  if (!locationId || !zone) {
    return (
      <div className="text-center text-sm text-muted-foreground">
        Select a location and zone to see details here.
      </div>
    );
  }

  return (
    <Tabs
      defaultValue="doors"
      onValueChange={(v) => setActiveTab(v as TabType)}
    >
      <div className="flex flex-col gap-3 sm:flex-row sm:items-center sm:justify-between">
        <TabsList>
          <TabsTrigger value="doors">Doors</TabsTrigger>
          <TabsTrigger value="clients">Clients</TabsTrigger>
          <TabsTrigger value="components">Components</TabsTrigger>
        </TabsList>

        <div className="flex gap-2">
          {activeTab == "doors" && (
            <Button
              variant="outline"
              onClick={() => openCreate("door")}
              disabled={!zone.id}
            >
              + Door
            </Button>
          )}

          {activeTab == "clients" && (
            <Button
              variant="outline"
              onClick={() => openCreate("client")}
              disabled={!zone.id}
            >
              + Client
            </Button>
          )}

          {activeTab == "components" && (
            <div className="flex items-center gap-2">
              <div className="text-sm text-muted-foreground">Client</div>
              <Select
                value={client?.id || ""}
                onValueChange={setClientId}
                disabled={zone?.clients.length === 0}
              >
                <SelectTrigger>
                  <SelectValue
                    placeholder={
                      zone?.clients.length ? "Select client" : "No clients"
                    }
                  />
                </SelectTrigger>
                <SelectContent>
                  {zone?.clients.map((d) => (
                    <SelectItem key={d.id} value={d.id}>
                      {d.config?.name}
                    </SelectItem>
                  ))}
                </SelectContent>
              </Select>

              <Button
                variant="outline"
                onClick={() => openCreate("component")}
                disabled={!client?.id}
              >
                + Component
              </Button>
            </div>
          )}
        </div>
      </div>

      <TabsContent value="doors" className="mt-4">
        <SimpleList
          title="Doors in this zone"
          items={zone?.doors ?? []}
          empty="No doors yet."
          kind="door"
          locationId={locationId}
          zoneId={zone.id}
          mqttClient={mqttClient}
        />
      </TabsContent>

      <TabsContent value="clients" className="mt-4">
        {zone?.clients && (
          <SimpleList
            title="Clients in this zone"
            items={zone.clients}
            empty="No clients yet."
            kind="client"
            locationId={locationId}
            zoneId={zone.id}
            mqttClient={mqttClient}
          />
        )}
      </TabsContent>

      <TabsContent value="components" className="mt-4">
        <div className="mt-3">
          <SimpleList
            title={`Components for ${client?.config.name ?? "—"}`}
            items={client?.components ?? []}
            empty="No components for this client."
            kind="component"
            locationId={locationId}
            zoneId={zone.id}
            mqttClient={mqttClient}
          />
        </div>
      </TabsContent>
    </Tabs>
  );
}

function SimpleList<T extends Base>({
  title,
  items,
  empty,
  kind,
  locationId,
  zoneId,
  mqttClient,
}: {
  title: string;
  items: T[];
  empty: string;
  kind: CreateKind;
  locationId: Id;
  zoneId?: Id;
  mqttClient: mqtt.MqttClient;
}) {
  function handleSubmit(e: React.FormEvent, id: string) {
    e.preventDefault();

    const formData = new FormData(e.target as HTMLFormElement);

    const newName = formData.get("name") as string;
    if (!newName) return;

    let topic = "";
    switch (kind) {
      case "door":
        topic = `locations/${locationId}/zones/${zoneId}/doors/${id}/config`;
        break;
      case "component":
        topic = `clients/${id}/${"temperature"}/config`;
        break;
      case "location":
        topic = `locations/${id}/config`;
        break;
      case "zone":
        topic = `locations/${locationId}/zones/${id}/config`;
        break;
      case "client":
        topic = `locations/${locationId}/zones/${zoneId}/clients/${id}/config`;
        break;
    }

    mqttClient.publish(topic, JSON.stringify({ name: newName }), {
      retain: true,
    });
  }

  function handleDelete(id: string) {
    let topic = "";
    switch (kind) {
      case "door":
        topic = `locations/${locationId}/zones/${zoneId}/doors/${id}/config`;
        break;
      case "component":
        topic = `clients/${id}/${"temperature"}/config`;
        break;
      case "location":
        topic = `locations/${id}/config`;
        break;
      case "zone":
        topic = `locations/${locationId}/zones/${id}/config`;
        break;
      case "client":
        topic = `locations/${locationId}/zones/${zoneId}/clients/${id}/config`;
        break;
    }

    mqttClient.publish(topic, "", { retain: true });
  }

  return (
    <div className="rounded-lg border">
      <div className="border-b p-3">
        <div className="text-sm font-medium flex items-center gap-1">
          <p>{title}</p>
          <span className="text-muted-foreground">({items.length})</span>
        </div>
      </div>
      <div className="divide-y">
        {items.length === 0 ? (
          <div className="p-4 text-sm text-muted-foreground">{empty}</div>
        ) : (
          items.map((it) => (
            <div key={it.id} className="flex items-center justify-between p-3">
              <div className="min-w-0">
                <div className="truncate text-sm font-medium">
                  {it.config?.name}
                </div>
                <div className="truncate text-xs text-muted-foreground">
                  {it.id}
                </div>
              </div>
              <div className="flex gap-2">
                <Dialog>
                  <DialogTrigger asChild>
                    <Button variant="outline" size="sm">
                      Configure
                    </Button>
                  </DialogTrigger>
                  <DialogContent>
                    <DialogTitle>Configure {it.config?.name}</DialogTitle>
                    <DialogDescription>
                      Settings and configuration options for{" "}
                      <b>{it.config?.name}</b> go here.
                    </DialogDescription>
                    <form onSubmit={(e) => handleSubmit(e, it.id)}>
                      <div className="mt-4 space-y-4">
                        <Label>Name</Label>
                        <Input
                          id="name"
                          name="name"
                          defaultValue={it.config?.name}
                          autoComplete="off"
                          className="mt-1"
                        />
                      </div>

                      <div className="mt-6 flex justify-end gap-2">
                        <DialogClose asChild>
                          <Button variant="outline">Cancel</Button>
                        </DialogClose>

                        <Button>Save</Button>
                      </div>
                    </form>
                  </DialogContent>
                </Dialog>
                <Button
                  variant="destructive"
                  size="sm"
                  onClick={() => handleDelete(it.id)}
                >
                  Delete
                </Button>
              </div>
            </div>
          ))
        )}
      </div>
    </div>
  );
}

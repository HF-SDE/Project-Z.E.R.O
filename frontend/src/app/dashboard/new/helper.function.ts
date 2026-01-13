import { Id } from "./client";

interface Config {
  name?: string;
  [key: string]: unknown;
}

export interface Base {
  id: Id;
  config: Config;
}

export type Location = Base & {
  zones: Zone[];
};

export type Zone = Base & {
  doors: Base[];
  clients: Client[];
};

export type Client = Base & {
  components: Base[];
};

export type FlatClient = Client & {
  locationId: Id | null;
};

export interface Data {
  locations: Location[];
  clients: FlatClient[];
}

export const mergeTopicIntoResult = (
  prev: Data,
  fullTopic: string,
  payload: unknown
): Data => {
  const result: Data = { ...prev };

  const parts = fullTopic.split("/").filter(Boolean);
  if (parts.length < 3) return result;

  const isDeletePayload = (p: unknown) => {
    if (p === null || p === undefined) return true;
    if (typeof p === "string") return p.trim() === "";
    // If you pass `{}` for retained-deletes, treat empty object as delete
    if (typeof p === "object") {
      try {
        return Object.keys(p as Record<string, unknown>).length === 0;
      } catch {
        return false;
      }
    }
    return false;
  };

  const removeById = <T extends { id: string }>(list: T[], id: string) =>
    list.filter((x) => x.id !== id);

  const isDelete = isDeletePayload(payload);
  /* ---------------- helpers ---------------- */

  const getLocation = (locId: string): Location => {
    let loc = result.locations.find((l) => l.id === locId);

    if (!loc) {
      loc = {
        id: locId,
        config: {},
        zones: [],
      };
      result.locations.push(loc);
    }

    loc.config ??= {};
    loc.zones ??= [];
    return loc;
  };

  const getZone = (loc: Location, zoneId: string): Zone => {
    let zone = loc.zones.find((z) => z.id === zoneId);

    if (!zone) {
      zone = {
        id: zoneId,
        config: {},
        doors: [],
        clients: [],
      };
      loc.zones.push(zone);
    }

    zone.config ??= {};
    zone.doors ??= [];
    zone.clients ??= [];
    return zone;
  };

  const getDoor = (zone: Zone, doorId: string): Base => {
    let door = zone.doors.find((d) => d.id === doorId);

    if (!door) {
      door = {
        id: doorId,
        config: {},
      };
      zone.doors.push(door);
    }

    door.config ??= {};
    return door;
  };

  const getClient = (zone: Zone, clientId: string): Client => {
    let client = zone.clients.find((c) => c.id === clientId);

    if (!client) {
      client = {
        id: clientId,
        config: {},
        components: [],
      };
      zone.clients.push(client);
    }

    client.config ??= {};
    client.components ??= [];
    return client;
  };

  const getFlatClient = (clientId: string): FlatClient => {
    let client = result.clients.find((c) => c.id === clientId);

    if (!client) {
      client = {
        id: clientId,
        config: {},
        components: [],
        locationId: null,
      };
      result.clients.push(client);
    }

    client.config ??= {};
    client.components ??= [];
    client.locationId ??= null;
    return client;
  };

  const getComponent = (client: Client, componentId: string): Base => {
    let comp = client.components.find((c) => c.id === componentId);

    if (!comp) {
      comp = {
        id: componentId,
        config: {},
      };
      client.components.push(comp);
    }

    comp.config ??= {};
    return comp;
  };

  /* ---------------- topic routing ---------------- */

  // 1) locations/{locId}/config
  if (parts[0] === "locations" && parts[2] === "config") {
    const locId = parts[1];

    if (isDelete) {
      // Remove location and any flat clients that belonged to its zones (if present)
      const existing = result.locations.find((l) => l.id === locId);
      const clientIds = new Set<string>();
      existing?.zones?.forEach((z) =>
        z.clients?.forEach((c) => clientIds.add(c.id))
      );

      result.locations = removeById(result.locations, locId);
      if (clientIds.size) {
        result.clients = result.clients.filter((c) => !clientIds.has(c.id));
      }
      return result;
    }

    const loc = getLocation(locId);
    Object.assign(loc.config, payload);
    return result;
  }

  // 2) locations/{locId}/zones/{zoneId}/config
  if (
    parts[0] === "locations" &&
    parts[2] === "zones" &&
    parts[4] === "config"
  ) {
    const locId = parts[1];
    const zoneId = parts[3];

    if (isDelete) {
      const loc = result.locations.find((l) => l.id === locId);
      if (!loc) return result;

      // collect client ids from this zone to also remove from flat list
      const zone = loc.zones?.find((z) => z.id === zoneId);
      const clientIds = new Set<string>();
      zone?.clients?.forEach((c) => clientIds.add(c.id));

      loc.zones = removeById(loc.zones ?? [], zoneId);
      if (clientIds.size) {
        result.clients = result.clients.filter((c) => !clientIds.has(c.id));
      }
      return result;
    }

    const loc = getLocation(locId);
    const zone = getZone(loc, zoneId);
    Object.assign(zone.config, payload);
    return result;
  }

  // 3) locations/{locId}/zones/{zoneId}/doors/{doorId}/config
  if (
    parts[0] === "locations" &&
    parts[2] === "zones" &&
    parts[4] === "doors" &&
    parts[6] === "config"
  ) {
    const locId = parts[1];
    const zoneId = parts[3];
    const doorId = parts[5];

    if (isDelete) {
      const loc = result.locations.find((l) => l.id === locId);
      const zone = loc?.zones?.find((z) => z.id === zoneId);
      if (!zone) return result;

      zone.doors = removeById(zone.doors ?? [], doorId);
      return result;
    }

    const loc = getLocation(locId);
    const zone = getZone(loc, zoneId);
    const door = getDoor(zone, doorId);
    Object.assign(door.config, payload);
    return result;
  }

  // 4) locations/{locId}/zones/{zoneId}/clients/{clientId}/config
  if (
    parts[0] === "locations" &&
    parts[2] === "zones" &&
    parts[4] === "clients" &&
    parts[6] === "config"
  ) {
    const locId = parts[1];
    const zoneId = parts[3];
    const clientId = parts[5];

    if (isDelete) {
      const loc = result.locations.find((l) => l.id === locId);
      const zone = loc?.zones?.find((z) => z.id === zoneId);
      if (!zone) return result;

      zone.clients = removeById(zone.clients ?? [], clientId);
      result.clients = removeById(result.clients, clientId);
      return result;
    }

    const loc = getLocation(locId);
    const zone = getZone(loc, zoneId);

    const client = getClient(zone, clientId);
    Object.assign(client.config, payload);

    // also keep flat list in sync
    const flat = getFlatClient(clientId);
    Object.assign(flat.config, payload);
    flat.locationId = locId;

    return result;
  }

  // 5) clients/{clientId}/{componentId}/config
  if (parts[0] === "clients" && parts[3] === "config") {
    const clientId = parts[1];
    const componentId = parts[2];

    if (isDelete) {
      const client = result.clients.find((c) => c.id === clientId);
      if (!client) return result;

      client.components = removeById(client.components ?? [], componentId);
      return result;
    }

    const client = getFlatClient(clientId);
    const comp = getComponent(client, componentId);
    Object.assign(comp.config, payload);
    return result;
  }

  // 6) clients/{clientId}/config
  if (parts[0] === "clients" && parts[2] === "config") {
    const clientId = parts[1];

    if (isDelete) {
      result.clients = removeById(result.clients, clientId);
      return result;
    }

    const client = getFlatClient(clientId);
    Object.assign(client.config, payload);
    return result;
  }

  return result;
};

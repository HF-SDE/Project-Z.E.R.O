import { $Enums } from "@prisma/client";

export interface MQTTData {
    deviceId: string;
    data: {
      value: number;
      identifier: $Enums.Identifier;
      name: string;
    }[];
  }
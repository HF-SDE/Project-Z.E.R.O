import { Device } from '@prisma/client';

interface SensorData {
  value: number;
  date: Date;
}

interface Sensor {
  name: string;
  identifier: string;
  data: SensorData[];
}

export interface StatsResponse extends Device {
  Sensors?: Sensor[];
}

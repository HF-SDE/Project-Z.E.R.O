import { APIResponse, IAPIResponse, Status } from '@api-types/general.types';
import { StatsResponse } from '@api-types/stats.types';
import prisma from '@prisma-instance';

/**
 * Service to get a user's profile
 * @param {string} accessToken - The access token for the user.
 * @returns {Promise<APIResponse<StatsResponse[]>>} A promise that resolves to an object containing the status, message and data of the user's profile.
 */
export async function getStats(
  deviceIds: string[] | undefined,
  sensors: string[] | undefined,
  startDatetime: Date | undefined,
  endDatetime: Date | undefined,
): Promise<APIResponse<StatsResponse[]>> {
  try {
    const foundDevices = await prisma.device.findMany({
      where: deviceIds ? { uuid: { in: deviceIds } } : undefined,
      include: {
        Location: true,
      },
      omit: {
        locationUuid: true,
      },
    });

    const start =
      startDatetime && !isNaN(startDatetime.getTime())
        ? startDatetime
        : new Date();
    const end =
      endDatetime && !isNaN(endDatetime.getTime())
        ? endDatetime
        : new Date(new Date().setDate(new Date().getDate() - 30));

    const timeseriesData = await prisma.timeseries.findMany({
      where: {
        deviceId: { in: foundDevices.map((d) => d.uuid) },
        createdAt: {
          gte: end, // Greater than or equal to start date
          lte: start, // Less than or equal to end date
        },
        name: sensors && sensors.length > 0 ? { in: sensors } : undefined, // If no sensors provided, fetch all
      },
    });

    console.log(start);
    console.log(end);
    console.log(sensors);

    const stats: StatsResponse[] = foundDevices.map((device) => {
      // Get all readings corresponding to the current device.
      const deviceReadings = timeseriesData.filter(
        (ts) => ts.deviceId === device.uuid,
      );

      // Group readings by sensor name.
      const sensorGroups: Record<
        string,
        {
          name: string;
          identifier: string;
          data: { value: number; identifier: string; date: Date }[];
        }
      > = {};

      deviceReadings.forEach((reading) => {
        if (!sensorGroups[reading.name]) {
          sensorGroups[reading.name] = {
            name: reading.name,
            identifier: reading.identifier,
            data: [],
          };
        }

        sensorGroups[reading.name].data.push({
          value: reading.value,
          date: reading.createdAt,
        });
      });

      // Convert grouped sensors into an array.
      const sensorsArray = Object.values(sensorGroups);

      return {
        ...device,
        // Map the Sensors property to "sensors" to match your desired JSON format.
        sensors: sensorsArray,
      };
    });

    return {
      status: Status.Found,
      message: 'Stat(s) found',
      data: stats,
    };
  } catch (error: unknown) {
    return {
      status: Status.Failed,
      message: 'Something went wrong on our end' + (error as Error).message,
    };
  }
}

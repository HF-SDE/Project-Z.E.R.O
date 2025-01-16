import Joi from 'joi';

import { APIResponse, IAPIResponse, Status } from '@api-types/general.types';
import prisma, { errorResponse } from '@prisma-instance';
import { Prisma } from '@prisma/client';
import { PrismaClientKnownRequestError } from '@prisma/client/runtime/library';

import { Validate } from './default.service';
import { UUID } from 'bson';
import argon2 from 'argon2';

interface IDeviceResponse {
  'api-key': string;
}

/**
 * Service to create a record
 * @async
 * @param {Prisma.DeviceCreateInput} data - The data to create a record with.
 * @param {Joi.ObjectSchema} schema - The schema to validate the data against.
 * @returns {Promise<IAPIResponse>} A promise that resolves to an object containing the record data, status, and message.
 */
export async function create(
  data: Prisma.DeviceCreateInput,
  schema: Joi.ObjectSchema,
): Promise<APIResponse<IDeviceResponse>> {
  const { err, validatedData } = Validate('device', data, schema) as {
    err: IAPIResponse;
    validatedData: Prisma.DeviceCreateInput;
  };
  if (err) return err;

  try {
    await prisma.device.create({
      data: { ...validatedData },
    });

    return {
      status: Status.Created,
      message: `Created new device`,
    };
  } catch (error) {
    const prismaError = error as Prisma.PrismaClientKnownRequestError;
    return errorResponse(prismaError, 'device', 'CreationFailed');
  }
}

/**
 * Service to reset the API key for a device
 * @param {string} deviceUuid - The UUID of the device to reset the API key for.
 * @returns {Promise<IAPIResponse<IDeviceResponse>>} A promise that resolves to an object containing the new API key, status, and message.
 */
export async function resetApiKey(
  deviceUuid: string,
): Promise<APIResponse<IDeviceResponse>> {
  try {
    console.log(`Resetting API key for device UUID: ${deviceUuid}`); // Debugging log
    const device = await prisma.device.findUnique({
      where: { uuid: deviceUuid }, // Query by 'uuid' instead of 'id'
    });

    if (!device) {
      console.log(`Device with UUID ${deviceUuid} not found`); // Debugging log
      return {
        data: null,
        status: Status.NotFound,
        message: 'Device not found',
      };
    }

    const newApiKey = new UUID().toString();
    const hashedNewApiKey = await argon2.hash(newApiKey);

    await prisma.device.update({
      where: { uuid: deviceUuid }, // Update by 'uuid' instead of 'id'
      data: { token: hashedNewApiKey },
    });

    return {
      data: { 'api-key': newApiKey },
      status: Status.Success,
      message: 'API key reset successfully',
    };
  } catch (err) {
    console.error('Error resetting API key:', err); // Debugging log
    if (err instanceof PrismaClientKnownRequestError) {
      return {
        data: null,
        status: Status.ApiKeyResetFailed,
        message: 'Prisma error: Failed to reset API key',
      };
    }
    return {
      data: null,
      status: Status.ApiKeyResetFailed,
      message: 'Failed to reset API key',
    };
  }
}

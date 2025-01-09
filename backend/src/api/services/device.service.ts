import argon2 from 'argon2';
import { UUID } from 'bson';
import Joi from 'joi';

import { APIResponse, IAPIResponse, Status } from '@api-types/general.types';
import prisma, { errorResponse } from '@prisma-instance';
import { Prisma } from '@prisma/client';

import { Validate } from './default.service';

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
    const randomUUID = new UUID();
    const hashedUUID = await argon2.hash(randomUUID.toString());

    await prisma.device.create({
      data: { ...validatedData, token: hashedUUID },
    });

    return {
      data: { 'api-key': randomUUID.toString() },
      status: Status.Created,
      message: `Created new device`,
    };
  } catch (error) {
    const prismaError = error as Prisma.PrismaClientKnownRequestError;
    return errorResponse(prismaError, 'device', 'CreationFailed');
  }
}

/* eslint-disable @typescript-eslint/no-unsafe-member-access */

/* eslint-disable @typescript-eslint/no-unsafe-call */

/* eslint-disable @typescript-eslint/no-unsafe-assignment */
import Joi from 'joi';

import { APIResponse, IAPIResponse, Status } from '@api-types/general.types';
import prisma, { errorResponse, prismaModels } from '@prisma-instance';
import { Prisma } from '@prisma/client';
import { capitalize } from '@utils/Utils';

/**
 * Service to get all records from a collection
 * @async
 * @param {prismaModels} prismaModel - The Prisma model to get the records from.
 * @param {Record<string, unknown>} config - The parameters to filter the records by.
 * @returns {Promise<APIResponse<any>>} A promise that resolves to an object containing the data, status, and message.
 */
export async function getAll(
  prismaModel: prismaModels,
  config: Record<string, unknown> = {},
  schema: Joi.ObjectSchema,
): Promise<APIResponse<any>> {
  const { err, prismaType, validatedData } = Validate(
    prismaModel,
    config.where,
    schema,
  );
  if (err) return err;

  config.where = validatedData;

  const results = await prismaType.findMany(config);

  return {
    data: results,
    status: Status.Found,
    message: `${capitalize(prismaModel)}(s) found`,
  };
}

/**
 * Service to create a record
 * @async
 * @param {prismaModels} prismaModel - The Prisma model to create the record with.
 * @param {any} data - The data to create a record with.
 * @param {Joi.ObjectSchema} schema - The schema to validate the data against.
 * @returns {Promise<IAPIResponse>} A promise that resolves to an object containing the record data, status, and message.
 */
export async function create(
  prismaModel: prismaModels,
  data: unknown,
  schema: Joi.ObjectSchema,
): Promise<IAPIResponse> {
  const { err, prismaType, validatedData } = Validate(
    prismaModel,
    data,
    schema,
  );
  if (err) return err;

  try {
    await prismaType.create({ data: validatedData });

    return {
      status: Status.Created,
      message: `Created new ${prismaModel}`,
    };
  } catch (error) {
    const prismaError = error as Prisma.PrismaClientKnownRequestError;
    return errorResponse(prismaError, prismaModel, 'CreationFailed');
  }
}

/**
 * Service to update a record
 * @async
 * @param {prismaModels} prismaModel - The Prisma model to update the record from.
 * @param {string} id - The id of the record to update.
 * @param {any} data - The data to update the record with.
 * @param {boolean} isPatch - Whether to update the record with a patch.
 * @param {Joi.ObjectSchema} schema - The schema to validate the data against.
 * @returns {Promise<APIResponse>} A promise that resolves to an object containing the record data, status, and message.
 */
export async function update(
  prismaModel: prismaModels,
  id: string,
  data: unknown,
  schema: Joi.ObjectSchema,
): Promise<APIResponse> {
  const { err, prismaType, validatedData } = Validate(
    prismaModel,
    data,
    schema,
  );
  if (err) return err;

  try {
    await prismaType.update({ where: { id }, data: validatedData });

    return {
      status: Status.Updated,
      message: `Updated ${prismaModel}`,
    };
  } catch (error) {
    const prismaError = error as Prisma.PrismaClientKnownRequestError;
    return errorResponse(prismaError, prismaModel, 'UpdateFailed');
  }
}

/**
 * Service to delete a record
 * @async
 * @param {prismaModels} prismaModel - The Prisma model to delete the record from.
 * @param {string} id - The id of the record to delete.
 * @returns {Promise<IAPIResponse>} A promise that resolves to an object containing the record data, status, and message.
 */
export async function deleteRecord(
  prismaModel: prismaModels,
  id: string,
): Promise<IAPIResponse> {
  const { err, prismaType } = Validate(prismaModel);
  if (err) return err;

  try {
    await prismaType.delete({ where: { id } });

    return {
      status: Status.Deleted,
      message: `Deleted ${prismaModel}`,
    };
  } catch (error) {
    const prismaError = error as Prisma.PrismaClientKnownRequestError;
    return errorResponse(prismaError, prismaModel, 'DeleteFailed');
  }
}

/**
 * Function to validate the data
 * @param {prismaModels} prismaModel - The Prisma model to validate the data against.
 * @param {unknown} data - The data to validate.
 * @param {Joi.ObjectSchema} schema - The schema to validate the data against.
 * @returns {IAPIResponse} An object containing the status and message.
 */
function Validate(
  prismaModel: prismaModels,
  data?: unknown,
  schema?: Joi.ObjectSchema,
): { err?: IAPIResponse; prismaType?: any; validatedData?: unknown } {
  if (!Object.prototype.hasOwnProperty.call(prisma, prismaModel)) {
    return {
      err: {
        status: Status.Failed,
        message: `Something went wrong on our end`,
      },
    };
  }
  // eslint-disable-next-line security/detect-object-injection
  const prismaType = prisma[prismaModel];

  if (!prismaType) {
    return {
      err: {
        status: Status.NotFound,
        message: `Database collection not found`,
      },
    };
  }

  if (schema) {
    const { value, error } = schema.validate(data);

    if (error) {
      return {
        err: {
          status: Status.InvalidDetails,
          message: error.message,
        },
      };
    }

    data = value;
  }

  return {
    err: undefined,
    prismaType,
    validatedData: data,
  };
}

import { Request, Response } from 'express';
import Joi from 'joi';

import { ExpressFunction } from '@api-types/general.types';
import { prismaModels } from '@prisma-instance';
import { Prisma } from '@prisma/client';
import * as deviceService from '@services/device.service';
import { getHttpStatusCode } from '@utils/Utils';

/**
 * Controller to create a record
 * @param {schema} schema - The schema to validate the create object.
 * @returns {ExpressFunction} The response object
 */
export function createDevice(schema: Joi.ObjectSchema): ExpressFunction {
  return async (req, res) => {
    const data = req.body as Prisma.DeviceCreateInput;

    const response = await deviceService.create(data, schema);

    res.status(getHttpStatusCode(response.status)).json(response).end();
  };
}

interface IGetAllConfig {
  model?: prismaModels;
  prismaConfig?: RequestConfig;
}

// eslint-disable-next-line jsdoc/require-jsdoc
function getModel(req: Request): prismaModels {
  return req.baseUrl.replace('/', '') as prismaModels;
}

/**
 * Controller to get devices
 * @param {schema} schema - The schema to validate the query object.
 * @param {IGetAllConfig | prismaModels} configOrModel - The Prisma model to get the records from.
 * @returns {ExpressFunction} The response object
 */
export function getDevices(
  schema: Joi.ObjectSchema,
  configOrModel?: IGetAllConfig | prismaModels,
): ExpressFunction {
  return async (req, res) => {
    let model: prismaModels | undefined;
    let prismaConfig: RequestConfig | undefined;

    if (typeof configOrModel === 'object') {
      model = configOrModel.model;
      prismaConfig = configOrModel.prismaConfig;
    } else model = configOrModel;

    model ??= getModel(req);

    const config = {
      where: req.query,
      ...prismaConfig,
    };

    const response = await deviceService.getAll(model, schema, config);

    res.status(getHttpStatusCode(response.status)).json(response).end();
  };
}
/**
 * Controller to reset the API key for a device
 * @returns {ExpressFunction} The response object with the new API key.
 */
export function resetApiKey(): ExpressFunction {
  return async (req, res) => {
    const deviceUuid = req.params.uuid;

    // Call the service function to reset the API key
    const response = await deviceService.resetApiKey(deviceUuid);

    res.status(getHttpStatusCode(response.status)).json(response).end();
  };
}

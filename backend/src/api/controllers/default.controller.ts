import { Request, Response } from 'express';
import Joi from 'joi';
import { ExtendedWebSocket, WSResponse, isWebSocket } from 'websocket-express';

import { ExpressFunction, Status } from '@api-types/general.types';
import { prismaModels } from '@prisma-instance';
import { UuidSchema } from '@schemas/general.schemas';
import * as DefaultService from '@services/default.service';
import { getHttpStatusCode } from '@utils/Utils';
import * as configWithoutType from '@utils/configs';

// eslint-disable-next-line jsdoc/require-jsdoc
function getModel(req: Request): prismaModels {
  return req.baseUrl.replace('/', '') as prismaModels;
}

type Config = Record<prismaModels, Record<string, unknown>>;
const config: Config = configWithoutType as Config;

/**
 * Controller to get all
 * @template T - The type of the data to be transformed.
 * @param {prismaModels} [model] - [Optional] - The Prisma model to get the records from.
 * @param {(data: T[]) => T[]} [transform] - [Optional] - The function to transform the data.
 * @returns {ExpressFunction} The response object
 */
export function getAll<T = unknown, D = unknown>(
  schema: Joi.ObjectSchema,
  model?: prismaModels,
  transform?: (data: T[] & D[]) => T[],
): ExpressFunction {
  return async (req, res: Response | WSResponse) => {
    let ws: ExtendedWebSocket;
    if (isWebSocket(res)) {
      ws = await res.accept();
    }
    if (!model) model = getModel(req);

    req.query.id ??= req.params.id;

    const { error } = UuidSchema.validate(req.query.id);

    if (error) {
      if (isWebSocket(res)) {
        res.sendError(
          getHttpStatusCode(Status.InvalidDetails),
          getHttpStatusCode(Status.wsInvalidDetails),
          JSON.stringify({
            status: Status.InvalidDetails,
            message: error.message,
          }),
        );
      } else {
        res
          .status(400)
          .json({ status: Status.InvalidDetails, message: error.message })
          .end();
      }
      return;
    }

    const modelConfig = req.config || {
      // eslint-disable-next-line security/detect-object-injection
      ...config[model],
      // eslint-disable-next-line security/detect-object-injection
      where: Object.assign({}, req.query, config[model]?.where),
    };

    const response = await DefaultService.getAll(model, modelConfig, schema);

    if (isWebSocket(res)) {
      ws!.send(JSON.stringify(response));
      setInterval(() => {
        ws.send(JSON.stringify(response));
      }, 12000);
    } else {
      if (response.data && transform) {
        response.data = transform(response.data as T[] & D[]);
      }
      res.status(getHttpStatusCode(response.status)).json(response).end();
    }
  };
}

/**
 * Controller to create a record
 * @param {prismaModels} model - The Prisma model to create the record with.
 * @returns {ExpressFunction} The response object
 */
export function createRecord(
  schema: Joi.ObjectSchema,
  model?: prismaModels,
): ExpressFunction {
  return async (req: Request, res: Response): Promise<void> => {
    const response = await DefaultService.create(
      model ?? getModel(req),
      req.body,
      schema,
    );

    res.status(getHttpStatusCode(response.status)).json(response).end();
  };
}

/**
 * Controller to update a record
 * @param {prismaModels} model - The Prisma model to update the record in.
 * @returns {ExpressFunction} The response object
 */
export function updateRecord(
  schema: Joi.ObjectSchema,
  model?: prismaModels,
): ExpressFunction {
  return async (req, res) => {
    const id = (req.params.id || req.query.id) as string;

    if (!model) model = getModel(req);

    const response = await DefaultService.update(model, id, req.body, schema);

    res.status(getHttpStatusCode(response.status)).json(response).end();
  };
}

/**
 * Controller to delete a record
 * @param {prismaModels} model - The Prisma model to delete the record from.
 * @returns {ExpressFunction} The response object
 */
export function deleteRecord(model?: prismaModels): ExpressFunction {
  return async (req, res) => {
    const id = (req.params.id || req.query.id) as string;

    if (!model) model = getModel(req);

    const response = await DefaultService.deleteRecord(model, id);

    res.status(getHttpStatusCode(response.status)).json(response).end();
  };
}

/**
 * Controller to delete a record
 * @param {prismaModels} model - The Prisma model to delete the record from.
 * @returns {ExpressFunction} The response object
 */
export function softDeleteRecord(
  updateSchema: Joi.ObjectSchema,
  model?: prismaModels,
): ExpressFunction {
  return async (req, res) => {
    const id = (req.params.id || req.query.id) as string;

    if (!model) model = getModel(req);

    const response = await DefaultService.update(
      model,
      id,
      { active: false },
      updateSchema,
    );

    res.status(getHttpStatusCode(response.status)).json(response).end();
  };
}

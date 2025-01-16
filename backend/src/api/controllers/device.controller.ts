import Joi from 'joi';

import {
  ExpressFunction,
  Status,
  WebsocketFunction,
} from '@api-types/general.types';
import { getWss } from '@app';
import { Prisma } from '@prisma/client';
import * as defaultService from '@services/default.service';
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

/**
 * Controller to update a record
 * @param {Joi.ObjectSchema | Joi.ArraySchema} schema - The schema to validate the update object.
 * @returns {ExpressFunction} The response object
 */
export function updateDevice(
  schema: Joi.ObjectSchema | Joi.ArraySchema,
): ExpressFunction {
  return async (req, res) => {
    const data = req.body as Prisma.DeviceUpdateInput[];

    const response = await defaultService.update('device', '', data, schema);

    if (response.status === Status.Updated && data) {
      data.map(({ uuid }) => getWss().emit('device-update', uuid));
    }

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

/**
 * Websocket controller for device routes.
 * @returns {WebsocketFunction} A promise that resolves when the websocket connection is closed.
 */
export function websocketController(): WebsocketFunction {
  return (ws, req) => {
    const deviceId = req.headers['device-id'] as string;
    
    ws.on('close', () => console.log('Device disconnected. Uuid:', deviceId));
    
    deviceService.websocket(ws, deviceId);
  };
}

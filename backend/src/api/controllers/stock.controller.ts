import { Request, Response } from 'express';

import { APIResponse, Status } from '@api-types/general.types';
import { ChangePasswordRequestBody } from '@api-types/profile.types';
import { StatsResponse } from '@api-types/stats.types';
import { BasicUser } from '@api-types/user.types';
import * as StatsService from '@services/stats.service';
import { getHttpStatusCode } from '@utils/Utils';

// eslint-disable-next-line no-secrets/no-secrets
/**
 * Controller so user's can get their profile data
 * @param {Request<APIResponse<BasicUser>>} req - The request object
 * @param {Request<unknown, APIResponse<StatsResponse[]>, ChangePasswordRequestBody>} res - The response object
 * @returns {Promise<void>} The response object
 */
export async function getStats(
  req: Request<unknown, APIResponse<BasicUser>, ChangePasswordRequestBody>,
  res: Response<APIResponse<StatsResponse[]>>,
): Promise<void> {
  const device = (req.query.device as string)?.split(',') || [];
  if (device.length === 0) {
    res.status(400).json({
      status: Status.MissingDetails,
      message: 'Missing device filter',
    });
    return;
  }

  const sensors = (req.query.sensors as string)?.split(':') || [];
  const startDatetime = new Date(req.query['start-datetime'] as string);
  const endDatetime = new Date(req.query['end-datetime'] as string);

  const response = await StatsService.getStats(
    device,
    sensors,
    startDatetime,
    endDatetime,
  );

  res.status(getHttpStatusCode(response.status)).json(response).end();
}

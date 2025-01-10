import argon2 from 'argon2';
import { NextFunction, Request, Response } from 'express';

import { PrismaClient } from '@prisma/client';

import { unauthorizedResponse } from './authenticate.mw';

const prisma = new PrismaClient();

/**
 * Middleware to check if the user has the required permissions to access a route.
 * @param {Request} req - Express Request object
 * @param {Response} res - Express Response object
 * @param {NextFunction} next - Express NextFunction object
 * @returns {void} The middleware function to check permissions.
 */
export async function useApiKey(
  req: Request,
  res: Response,
  next: NextFunction,
): Promise<void> {
  const apiKey = req.headers['x-api-key'] as string;
  const deviceId = req.headers['device-id'] as string;

  if (!deviceId && !apiKey) {
    next('route');
    return;
  }

  if (!apiKey) {
    res.status(401).json({
      status: 'Unauthorized',
      message: 'x-api-key header is required when deviceId is provided',
    });

    return;
  }

  if (!deviceId) {
    res.status(401).json({
      status: 'Unauthorized',
      message: 'deviceId header is required when x-api-key is provided',
    });

    return;
  }

  const device = await prisma.device.findUnique({ where: { uuid: deviceId } });

  if (!device) {
    res.status(401).json(unauthorizedResponse);
    return;
  }

  const verifyToken = await argon2.verify(device.token as string, apiKey);

  if (verifyToken) {
    req.query.uuid = deviceId;

    next();
    return;
  } else {
    res.status(401).json(unauthorizedResponse);
    return;
  }
}

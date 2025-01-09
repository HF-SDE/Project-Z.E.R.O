import { NextFunction, Request, Response } from 'express';

import { Prisma } from '@prisma/client';
import { searchParams } from '@schemas/device';

interface RequestQuery extends Request {
  query: Partial<{
    id: string;
    name: string;
    location: string;
    status: string;
  }>;
}

/**
 * Middleware to transform the search query
 * @param {RequestQuery} req - The request object
 * @param {Response} res - The response object
 * @param {NextFunction} next - The next function
 * @returns {void}
 */
export function transformSearchQuery(
  req: RequestQuery,
  res: Response,
  next: NextFunction,
): void {
  const validate = searchParams.validate(req.query);

  if (validate.error) {
    res.status(400).json(validate.error.message).end();
    return;
  }

  const prismaSearchQuery = validate.value as Prisma.DeviceWhereInput;

  // const prismaSearchQuery = {
  //   uuid: id,
  //   name,
  //   locationUuid: location,
  //   status: status?.toLocaleUpperCase(),
  // } as Prisma.DeviceWhereInput;

  req.query = prismaSearchQuery as RequestQuery['query'];

  console.log(req.query);

  next();
}

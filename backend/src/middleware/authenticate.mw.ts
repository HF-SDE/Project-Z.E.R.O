import { NextFunction, Request, Response } from 'express';
import passport from 'passport';

import { Status } from '@api-types/general.types';
import { getHttpStatusCode } from '@utils/Utils';

import '../passport';

export const unauthorizedResponse = {
  status: Status.Unauthorized,
  message: 'Unauthorized',
};

/**
 * Verifies the JWT token in the request header.
 * @param {Request} req - The request object containing the JWT token.
 * @param {Response} res - The response object to send the result of the verification.
 * @param {NextFunction} next - The next middleware function in the chain.
 * @returns {void} Resolves with the user data if the token is valid.
 */
export function verifyJWT(
  req: Request,
  res: Response,
  next: NextFunction,
): void {
  // eslint-disable-next-line @typescript-eslint/no-unsafe-call
  passport.authenticate(
    'jwt',
    { session: false },
    (err: number, user: Express.User) => {
      if (err) return res.status(err).json(unauthorizedResponse);

      if (!user) {
        return res
          .status(getHttpStatusCode(Status.Unauthorized))
          .json(unauthorizedResponse);
      }

      req.user = user;
      next();
    },
  )(req, res, next);
}

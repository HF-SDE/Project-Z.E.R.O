import { Router } from 'express';

import { getAll } from '@controllers/default.controller';
import { isAllowed } from '@middlewares/isAllowed.mw';
import { Location } from '@prisma/client';
import { getLocationSchema } from '@schemas/location';

const router = Router();

router.get(
  '/',
  isAllowed(['location:view']),
  getAll<unknown, Location>(getLocationSchema, 'location'),
);


export default router;

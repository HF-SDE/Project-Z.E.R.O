import { Router } from 'express';

import { LocationWithOutCreateAtAndUpdatedAt } from '@api-types/location.types';
import {
  createRecord,
  getAll,
  updateRecord,
} from '@controllers/default.controller';
import { verifyJWT } from '@middlewares/authenticate.mw';
import { isAllowed } from '@middlewares/isAllowed.mw';
import { createLocationSchema, getLocationSchema, updateLocationSchema } from '@schemas/location';

const router = Router();

router.use('/', verifyJWT);
router.get(
  '/',
  getAll<
    LocationWithOutCreateAtAndUpdatedAt,
    LocationWithOutCreateAtAndUpdatedAt
  >(getLocationSchema, 'location'),
);

router.post(
  '/',
  isAllowed(['location:create']),
  createRecord(createLocationSchema, 'location'),
);

router.patch('/', isAllowed(['location:update']), updateRecord(updateLocationSchema, 'location'));

export default router;

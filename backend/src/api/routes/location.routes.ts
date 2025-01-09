import { Router } from 'express';

import { LocationWithOutCreateAtAndUpdatedAt } from '@api-types/location.types';
import {
  createRecord,
  deleteRecord,
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
  isAllowed(['location:view']),
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

router.delete('/:id', isAllowed(['location:delete']), deleteRecord('uuid', 'location'));

export default router;

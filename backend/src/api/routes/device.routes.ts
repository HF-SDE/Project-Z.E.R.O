import { Router } from 'express';

import {
  deleteRecord,
  getAll,
  updateRecord,
} from '@controllers/default.controller';
import { createDevice } from '@controllers/device.controller';
import { verifyJWT } from '@middlewares/authenticate.mw';
import { isAllowed } from '@middlewares/isAllowed.mw';
import {
  createSchema,
  searchParamsSchema,
  updateSchema,
} from '@schemas/device.schema';

const router = Router();

router.use('/', verifyJWT);

router.get('/', isAllowed('device:view'), getAll(searchParamsSchema));
router.post('/', isAllowed('device:create'), createDevice(createSchema));
router.patch('/', isAllowed('device:update'), updateRecord(updateSchema));
router.delete('/:id', isAllowed('device:delete'), deleteRecord());

export default router;

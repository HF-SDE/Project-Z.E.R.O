import { Router } from 'express';

import { deleteRecord, getAll } from '@controllers/default.controller';
import {
  createDevice,
  getDevices,
  resetApiKey,
  updateDevice,
  websocketController,
} from '@controllers/device.controller';
import { verifyApiKey, verifyJWT } from '@middlewares/authenticate.mw';
import { useApiKey } from '@middlewares/device.mw';
import { isAllowed } from '@middlewares/isAllowed.mw';
import {
  createSchema,
  searchParamsSchema,
  updateSchema,
} from '@schemas/device.schema';

const router = Router();

router.get('/', useApiKey, getDevices(searchParamsSchema));

// eslint-disable-next-line @typescript-eslint/no-misused-promises
router.ws('/', verifyApiKey, websocketController());

router.use('/', verifyJWT);

router.get('/', isAllowed('device:view'), getAll(searchParamsSchema));
router.post('/', isAllowed('device:create'), createDevice(createSchema));
router.patch('/', isAllowed('device:update'), updateDevice(updateSchema));
router.delete('/:id', isAllowed('device:delete'), deleteRecord());

router.post('/reset/:uuid', isAllowed('device:update'), resetApiKey());

export default router;

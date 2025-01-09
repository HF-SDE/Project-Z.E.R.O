import { Router } from 'express';

import * as DefaultController from '@controllers/default.controller';
import { verifyJWT } from '@middlewares/authenticate.mw';
import { isAllowed } from '@middlewares/isAllowed.mw';
import {
  createSchema,
  searchParamsSchema,
  updateSchema,
} from '@schemas/device.schema';

const router = Router();

router.use('/', verifyJWT);

router.get(
  '/',
  isAllowed('technician:device:view'),
  DefaultController.getAll(searchParamsSchema),
);
router.post(
  '/',
  isAllowed('technician:device:create'),
  DefaultController.createRecord(createSchema),
);
router.patch(
  '/',
  isAllowed('technician:device:update'),
  DefaultController.updateRecord(updateSchema),
);
router.delete(
  '/:id',
  isAllowed('technician:device:delete'),
  DefaultController.deleteRecord(),
);

export default router;

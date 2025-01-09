import { Router } from 'express';

import { createRecord, getAll } from '@controllers/default.controller';
import { verifyJWT } from '@middlewares/authenticate.mw';
import { isAllowed } from '@middlewares/isAllowed.mw';
import { createAlertSchema, getAlertSchema } from '@schemas/alert';

const router = Router();

router.use('/', verifyJWT);
router.get('/', isAllowed(['alert:view']), getAll(getAlertSchema, 'alert'));
router.post('/', isAllowed(['alert:create']), createRecord(createAlertSchema, 'alert'));

export default router;

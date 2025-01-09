import { Router } from 'express';

import { createRecord, deleteRecord, getAll, updateRecord } from '@controllers/default.controller';
import { verifyJWT } from '@middlewares/authenticate.mw';
import { isAllowed } from '@middlewares/isAllowed.mw';
import { createAlertSchema, getAlertSchema, updateAlertSchema } from '@schemas/alert';

const router = Router();

router.use('/', verifyJWT);
router.get('/', isAllowed(['alert:view']), getAll(getAlertSchema, 'alert'));
router.post('/', isAllowed(['alert:create']), createRecord(createAlertSchema, 'alert'));
router.patch('/', isAllowed(['alert:update']), updateRecord(updateAlertSchema, 'alert'));
router.delete('/', isAllowed(['alert:delete']), deleteRecord('uuid', 'alert'));

export default router;

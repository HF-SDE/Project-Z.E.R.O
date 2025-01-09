import { Router } from 'express';

import { getAll } from '@controllers/default.controller';
import { verifyJWT } from '@middlewares/authenticate.mw';
import { isAllowed } from '@middlewares/isAllowed.mw';
import { getAlertSchema } from '@schemas/alert';

const router = Router();

router.use('/', verifyJWT);
router.get('/', isAllowed(['alert:view']), getAll(getAlertSchema, 'alert'));

export default router;

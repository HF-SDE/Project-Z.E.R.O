import { Router } from 'express';

import { getStats } from '@controllers/stock.controller';
import { verifyJWT } from '@middlewares/authenticate.mw';
import { isAllowed } from '@middlewares/isAllowed.mw';

const router = Router();

router.use('/', verifyJWT);

router.get('/', isAllowed('data:view'), getStats);

export default router;

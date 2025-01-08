import { Router } from 'express';

import * as DefaultController from '@controllers/default.controller';
import { getAllSchema } from '@schemas/device';

const router = Router();

router.get('/', DefaultController.getAll(getAllSchema));

export default router;

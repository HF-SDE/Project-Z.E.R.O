import { Router } from 'express';

import Joi from '@/joi';
import * as DefaultController from '@controllers/default.controller';
import { transformSearchQuery } from '@middlewares/device.mw';
import { StatusSchema } from '@schemas/prisma/schemas/enums';
// import { getAllSchema } from '@schemas/device';
import {
  DeviceUncheckedCreateInputSchemaObject as createSchema,
  DeviceWhereInputSchemaObject as findSchema,
} from '@schemas/prisma/schemas/objects';

const router = Router();

const getSchema = Joi.object({ ...findSchema, status: StatusSchema });

router.get('/', transformSearchQuery, DefaultController.getAll(getSchema));
router.post('/', DefaultController.createRecord(Joi.object(createSchema)));

export default router;

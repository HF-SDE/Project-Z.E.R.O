import Joi from '@/joi';
import { StatusSchema } from '@schemas/prisma/schemas/enums';

export const searchParams = Joi.object({
  id: Joi.string().uuid(),
  name: Joi.string(),
  location: Joi.alternatives()
    .try(Joi.string().uuid(), Joi.string())
    .custom((value: unknown) => {
      console.log(value);
      if (Joi.attempt(value, Joi.string().uuid(), { abortEarly: true })) {
        return { key: 'locationUuid', value };
      }
      return { key: 'locationName', value };
    }),
  status: StatusSchema,
}).rename('id', 'uuid', { alias: true, ignoreUndefined: true });

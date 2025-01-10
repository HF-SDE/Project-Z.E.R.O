import Joi from '@/joi';
import { StatusSchema } from '@schemas/prisma/schemas/enums';
import { DeviceUncheckedCreateInputSchemaObject } from '@schemas/prisma/schemas/objects';

export const createSchema = Joi.object({
  ...DeviceUncheckedCreateInputSchemaObject,
  uuid: Joi.string().uuid(),
  locationUuid: Joi.string().uuid().required(),
});

export const searchParamsSchema = Joi.object({
  id: Joi.string().uuid(),
  uuid: Joi.string().uuid(),
  name: Joi.string(),
  Location: Joi.object({
    uuid: Joi.string().uuid(),
    name: Joi.string(),
  }),
  location: Joi.alternatives()
    .try(Joi.string().uuid(), Joi.string())
    .custom(getType),
  status: StatusSchema.insensitive(),
})
  .rename('id', 'uuid', { ignoreUndefined: true, override: true })
  .custom(getSearchQuery);

// eslint-disable-next-line jsdoc/require-jsdoc
function getType(value: string): object {
  try {
    Joi.assert(value, Joi.string().uuid());
    return { key: 'uuid', value };
  } catch {
    return { key: 'name', value };
  }
}

// eslint-disable-next-line jsdoc/require-jsdoc
function getSearchQuery(value: { location?: string; Location: any }): object {
  if (value.location && typeof value.location === 'object') {
    const { key, value: locationValue } = value.location;
    delete value.location;
    value.Location = { [key]: locationValue };
  }

  return value;
}

export const updateSchema = Joi.array().items(
  Joi.object({
    uuid: Joi.string().uuid().required(),
    name: Joi.string(),
    frequency: Joi.number().max(2147483647),
    status: StatusSchema.insensitive(),
    locationUuid: Joi.string().uuid(),
  }),
);

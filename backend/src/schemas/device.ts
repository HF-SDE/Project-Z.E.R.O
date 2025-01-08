import Joi from '@/joi';

export const getAllSchema = Joi.object({
  name: Joi.string().optional(),
});

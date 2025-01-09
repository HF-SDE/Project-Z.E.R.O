import Joi from "joi";

export const getLocationSchema = Joi.object({
    id: Joi.string().guid({version: 'uuidv4', separator: '-'}),
    name: Joi.string().min(3).max(255),
});
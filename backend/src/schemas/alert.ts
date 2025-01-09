import Joi from "joi";
import { UuidSchema } from "./general";

export const getAlertSchema = Joi.object({
    id: UuidSchema,
    name: Joi.string().min(3).max(255),
    type: Joi.string().lowercase().trim().valid('info', 'warning', 'error'),
    Identifier: Joi.string().lowercase().trim().valid("celsius","fahrenheit","percentage"),
    deviceId: UuidSchema,
}).rename('id', 'uuid', { alias: true, ignoreUndefined: true });

export const createAlertSchema = Joi.object({
    name: Joi.string().min(3).max(255).required(),
    description: Joi.string().min(3).max(255).required(),
    threshold: Joi.number().required(),
    type: Joi.string().lowercase().trim().valid('info', 'warning', 'error').required(),
    Identifier: Joi.string().lowercase().trim().valid("celsius","fahrenheit","percentage").required(),
    deviceId: UuidSchema.required(),
});
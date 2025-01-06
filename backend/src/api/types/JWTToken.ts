import { ObjectId } from 'bson';

export interface UserToken {
  sub: string;
  jti: ObjectId;
  username: string;
  permissions?: string[];
}

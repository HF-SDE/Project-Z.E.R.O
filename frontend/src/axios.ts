import axios from "axios";

export const client = axios.create({ withCredentials: true });

export const server = axios.create({
  ...client.defaults,
  baseURL: process.env.BACKEND_URL,
});

export default server;

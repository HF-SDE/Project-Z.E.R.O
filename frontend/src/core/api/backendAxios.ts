import axios from "axios";
import getConfig from "next/config";
const { serverRuntimeConfig, publicRuntimeConfig } = getConfig();
const backendAxios = axios.create({
    baseURL: serverRuntimeConfig.URL || publicRuntimeConfig.URL,
    withCredentials: true,
});

console.log('serverRuntime', serverRuntimeConfig.URL);
console.log('publicRuntime', publicRuntimeConfig.URL);


export default backendAxios;
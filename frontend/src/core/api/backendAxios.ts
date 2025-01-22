import axios from "axios";
const backendAxios = axios.create({
    baseURL: process.env.URL,
    withCredentials: true,
});

console.log('Url', process.env.URL);


export default backendAxios;
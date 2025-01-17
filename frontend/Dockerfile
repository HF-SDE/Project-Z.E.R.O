FROM node:22.9-alpine AS builder
RUN apk add --no-cache libc6-compat
WORKDIR /usr/src/app
COPY package.json package-lock.json /usr/src/app/
RUN npm install

COPY . /usr/src/app/

FROM builder AS dev
WORKDIR /usr/src/app
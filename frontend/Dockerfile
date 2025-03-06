# syntax=docker/dockerfile:1.4
FROM node:22.9-alpine AS base

# Set working directory
WORKDIR /usr/src/app

# Copy package.json and install dependencies
COPY package.json ./
RUN npm install

# Copy other application files
COPY . .
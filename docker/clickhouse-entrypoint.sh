#!/bin/bash

# Use envsubst to convert the template into a usable SQL file
envsubst < /docker-entrypoint-initdb.d/init.template.sql > /docker-entrypoint-initdb.d/init.sql

# Start the original ClickHouse entrypoint
exec /entrypoint.sh "$@"
# syntax=docker/dockerfile:1.4
FROM eclipse-mosquitto

COPY docker/mqttEntrypoint.sh /docker-entrypoint-initdb.d/init.sh
RUN chmod +x /docker-entrypoint-initdb.d/init.sh
# syntax=docker/dockerfile:1.4
FROM clickhouse/clickhouse-server:latest

# Install envsubst (part of gettext)
RUN apt-get update && apt-get install -y gettext && rm -rf /var/lib/apt/lists/*

# Copy your custom configuration
COPY config/clickhouse/init.template.sql /docker-entrypoint-initdb.d/init.template.sql

# Copy your custom entrypoint script
COPY docker/clickhouse-entrypoint.sh /usr/local/bin/
RUN chmod +x /usr/local/bin/clickhouse-entrypoint.sh

ENTRYPOINT ["/usr/local/bin/clickhouse-entrypoint.sh"]
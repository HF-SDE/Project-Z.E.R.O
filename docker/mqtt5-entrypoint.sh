#!/bin/ash
set -e

CERT_PATH="/etc/letsencrypt/live/$DOMAIN"

mosquitto_passwd -c -b /mosquitto/config/pwfile iot1 12345678
mosquitto_passwd -b /mosquitto/config/pwfile iot2 12345678
mosquitto_passwd -b /mosquitto/config/pwfile iot3 12345678
mosquitto_passwd -b /mosquitto/config/pwfile iot4 12345678
mosquitto_passwd -b /mosquitto/config/pwfile $MQTT_ADMIN_USERNAME $MQTT_ADMIN_PASSWORD

# Fix permissions
chown root /mosquitto/config/pwfile
chgrp root /mosquitto/config/pwfile

# Get cert if not already present
if [ -z "$(ls $CERT_PATH*/fullchain.pem 2>/dev/null)" ]; then
  echo "[INFO] Requesting Let's Encrypt cert for $DOMAIN..."
  certbot certonly --standalone --non-interactive --agree-tos --test-cert --email phgu03@gmail.com -d $DOMAIN

  echo "[INFO] Copying certs to /mosquitto/certs/"
  mkdir -p /mosquitto/certs
  cp "$CERT_PATH/"* /mosquitto/certs/
elif [ ! -f "/mosquitto/certs/fullchain.pem" ]; then
  echo "[INFO] Copying certs to /mosquitto/certs/"
  mkdir -p /mosquitto/certs
  cp "$CERT_PATH/"* /mosquitto/certs/
fi

# Fix perms
echo "[INFO] Fixing permissions..."
chown -R mosquitto:mosquitto /mosquitto/data
chown -R mosquitto:mosquitto /mosquitto/log
chown -R mosquitto:mosquitto /mosquitto/certs
chown -R mosquitto:mosquitto /mosquitto/config

chmod 600 /mosquitto/config/pwfile
find /mosquitto/certs -type f -exec chmod 600 {} \;
find /mosquitto/certs -type d -exec chmod 700 {} \;

# Set permissions
user="$(id -u)"
if [ "$user" = '0' ]; then
	[ -d "/mosquitto" ] && chown -R mosquitto:mosquitto /mosquitto || true
fi

exec "$@"
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
if [ ! -f "$CERT_PATH/fullchain.pem" ]; then
  echo "[INFO] Requesting Let's Encrypt cert for $DOMAIN..."
  certbot certonly --standalone --non-interactive --agree-tos --email phgu03@gmail.com -d $DOMAIN

  echo "[INFO] Copying certs to /mosquitto/certs/"
  cp "$CERT_PATH/"* /mosquitto/certs/
fi

# Fix perms 
usermod -aG ssl-cert mosquitto
chown -R root:ssl-cert /etc/letsencrypt/live/
chmod -R 750 /etc/letsencrypt/live/

# Set permissions
user="$(id -u)"
if [ "$user" = '0' ]; then
	[ -d "/mosquitto" ] && chown -R mosquitto:mosquitto /mosquitto || true
fi
exec "$@"
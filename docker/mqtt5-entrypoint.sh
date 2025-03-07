#!/bin/ash
set -e


mosquitto_passwd -c -b /mosquitto/config/pwfile iot1 12345678
mosquitto_passwd -b /mosquitto/config/pwfile iot2 12345678
mosquitto_passwd -b /mosquitto/config/pwfile iot3 12345678
mosquitto_passwd -b /mosquitto/config/pwfile iot4 12345678
mosquitto_passwd -b /mosquitto/config/pwfile admin admin
# Set permissions
user="$(id -u)"
if [ "$user" = '0' ]; then
	[ -d "/mosquitto" ] && chown -R mosquitto:mosquitto /mosquitto || true
fi
exec "$@"
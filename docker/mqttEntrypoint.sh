#!/bin/bash
mosquitto_passwd -c /mosquitto/config/pwfile iot1 12345678
mosquitto_passwd -c /mosquitto/config/pwfile iot2 12345678
mosquitto_passwd -c /mosquitto/config/pwfile iot3 12345678
mosquitto_passwd -c /mosquitto/config/pwfile iot4 12345678
exec "$@"
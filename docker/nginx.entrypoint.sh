#!/bin/sh

# Decode and save certs
echo "$CLIENT_CERT" | base64 -d > /etc/nginx/certs/ca.pem
echo "$CLIENT_KEY" | base64 -d > /etc/nginx/certs/ca.key
echo "$ORIGIN_CERT" | base64 -d > /etc/nginx/certs/origin.pem
echo "$ORIGIN_KEY" | base64 -d > /etc/nginx/certs/origin.key

# Formatting certs to linux
sed -i 's/\r$//' /etc/nginx/certs/ca.pem
sed -i 's/ *$//' /etc/nginx/certs/ca.pem
sed -i 's/\r$//' /etc/nginx/certs/ca.key
sed -i 's/ *$//' /etc/nginx/certs/ca.key
sed -i 's/\r$//' /etc/nginx/certs/origin.pem
sed -i 's/ *$//' /etc/nginx/certs/origin.pem
sed -i 's/\r$//' /etc/nginx/certs/origin.key
sed -i 's/ *$//' /etc/nginx/certs/origin.key

echo "Client Cert1\n"
cat -A /etc/nginx/certs/ca.pem

# Overwrite and unset cert env variables
CLIENT_CERT="***"
CLIENT_KEY="***"
ORIGIN_CERT="***"
ORIGIN_KEY="***"

unset CLIENT_CERT
unset CLIENT_KEY
unset ORIGIN_CERT
unset ORIGIN_KEY

exec "$@"

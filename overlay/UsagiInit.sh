#!/UsagiInit
mkdir -p /tmp/nginx/logs
v2ray run -config /etc/v2ray/config.json > /dev/null &
nginx -p /tmp/nginx -c /etc/nginx/nginx.conf -g "daemon off;" > /dev/null &

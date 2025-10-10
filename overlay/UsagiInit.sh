#!/UsagiInit
mkdir -p /tmp/nginx/logs
/NginxUptime-Go > /dev/null &
/bin/timeout -s KILL 23h v2ray run -config /etc/v2ray/config.json > /dev/null &
nginx -p /tmp/nginx -c /etc/nginx/nginx.conf -g "daemon off;" > /dev/null &

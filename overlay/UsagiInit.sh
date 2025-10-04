#!/UsagiInit
mkdir -p /tmp/nginx/logs
/NginxUptime-Go > /dev/null &
v2ray run -config /etc/v2ray/config.json > /dev/null &
timeout 23h nginx -p /tmp/nginx -c /etc/nginx/nginx.conf -g "daemon off;" > /dev/null &

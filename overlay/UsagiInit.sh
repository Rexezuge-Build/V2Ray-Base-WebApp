#!/UsagiInit
mkdir -p /tmp/nginx/logs
/NginxUptime-Go > /dev/null &
/usr/local/bin/cloudflared tunnel --no-autoupdate run --token $CLOUDFLARE_TOKEN &
/bin/timeout 23h v2ray run -config /etc/v2ray/config.json > /dev/null &
/bin/timeout 23h nginx -p /tmp/nginx -c /etc/nginx/nginx.conf -g "daemon off;" > /dev/null &

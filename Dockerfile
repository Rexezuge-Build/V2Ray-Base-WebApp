FROM debian:12 AS builder

WORKDIR /tmp

# Install Dependencies
RUN apt-get update \
 && apt-get install -y --no-install-recommends build-essential curl unzip zlib1g-dev libpcre2-dev perl ca-certificates

# Download and Install upx
ENV UPX_VERSION=5.0.2

RUN curl -L -o /tmp/upx.tar.xz https://github.com/upx/upx/releases/download/v${UPX_VERSION}/upx-${UPX_VERSION}-amd64_linux.tar.xz \
 && tar -xf /tmp/upx.tar.xz -C /tmp \
 && mv /tmp/upx-${UPX_VERSION}-amd64_linux/upx /usr/local/bin/upx

# Download V2Ray and Compress
RUN curl -L -o /tmp/v2ray.zip https://github.com/v2fly/v2ray-core/releases/latest/download/v2ray-linux-64.zip \
 && unzip /tmp/v2ray.zip -d /tmp/v2ray \
 && upx --best --lzma /tmp/v2ray/v2ray

# Download Cloudflare Tunnel and Compress
RUN curl -L -o /tmp/cloudflared https://github.com/cloudflare/cloudflared/releases/latest/download/cloudflared-linux-amd64 \
 && chmod +x /tmp/cloudflared \
 && upx --best --lzma /tmp/cloudflared

FROM rexezugedockerutils/nginx-static AS nginx-static

FROM rexezugedockerutils/nginx-uptime-go AS nginx-uptime-go

FROM rexezugedockerutils/usagi-init:release AS runtime

COPY --from=builder /tmp/v2ray/v2ray /usr/local/bin/v2ray

COPY --from=builder /tmp/cloudflared /usr/local/bin/cloudflared

COPY --from=nginx-static /nginx /usr/sbin/nginx

COPY --from=nginx-uptime-go /NginxUptime-Go /NginxUptime-Go

COPY overlay/ /

FROM scratch

COPY --from=runtime / /

ENTRYPOINT ["/UsagiInit"]

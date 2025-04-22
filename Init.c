#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>

#define likely(x) __builtin_expect((x), 1)
#define unlikely(x) __builtin_expect((x), 0)

#define DEV_NULL "/dev/null"

#define ENV_DEBUG_ENABLE_LOG_OUTPUT "DEBUG_ENABLE_LOG_OUTPUT"
#define ENV_CLOUDFLARE_TUNNEL_TOKEN "CLOUDFLARE_TUNNEL_TOKEN"

static char ENABLE_LOG_OUTPUT = 0;
static char *CLOUDFLARE_TOKEN = NULL;

void processEnvironmentVariables(void) {
  // Read Environment Variables
  if (getenv(ENV_DEBUG_ENABLE_LOG_OUTPUT) != NULL) {
    ENABLE_LOG_OUTPUT = 1;
  }

  CLOUDFLARE_TOKEN = getenv(ENV_CLOUDFLARE_TUNNEL_TOKEN);

  // Set Environment Variables
  // putenv("TRANSMISSION_WEB_HOME=/.TransmissionWebControl");
}

void runService_V2Ray(void) {
  // Start V2Ray Anti Censorship Platform
  printf("\033[0;32m%s\033[0m%s\n",
         "INFO: ", "Starting V2Ray Anti Censorship Platform");
  fflush(stdout);

  pid_t pidV2Ray = fork();
  if (likely(pidV2Ray == 0)) {
    fclose(stdin);
    if (!ENABLE_LOG_OUTPUT) {
      freopen(DEV_NULL, "w", stdout);
      freopen(DEV_NULL, "w", stderr);
    }
    execl("/usr/local/bin/v2ray", "v2ray", "run", "-config",
          "/etc/v2ray/config.json", NULL);
    printf("\033[0;31m%s\033[0m%s\n",
           "ERROR: ", "Failed to Run V2Ray Anti Censorship Platform");
    exit(EXIT_FAILURE);
  } else if (unlikely(pidV2Ray == -1)) {
    printf("\033[0;31m%s\033[0m%s\n", "ERROR: ", "Failed to Fork");
    exit(EXIT_FAILURE);
  }
}

void runService_Nginx(void) {
  // Start Nginx Web Server
  printf("\033[0;32m%s\033[0m%s\n", "INFO: ", "Starting Nginx Web Server");
  fflush(stdout);

  // Create Required Directory Structure for Nginx
  if (mkdir("/tmp/nginx", 0755) && errno != EEXIST) {
    printf("\033[0;31m%s\033[0m%s\n",
           "ERROR: ", "Failed to Create Directory For Nginx");
    exit(EXIT_FAILURE);
  }

  if (mkdir("/tmp/nginx/logs", 0755) && errno != EEXIST) {
    printf("\033[0;31m%s\033[0m%s\n",
           "ERROR: ", "Failed to Create Directory For Nginx");
    exit(EXIT_FAILURE);
  }

  pid_t pidV2Ray = fork();
  if (likely(pidV2Ray == 0)) {
    fclose(stdin);
    if (!ENABLE_LOG_OUTPUT) {
      freopen(DEV_NULL, "w", stdout);
      freopen(DEV_NULL, "w", stderr);
    }
    execl("/usr/sbin/nginx", "nginx", "-p", "/tmp/nginx", "-c",
          "/etc/nginx/nginx.conf", "-g", "daemon off;", NULL);
    printf("\033[0;31m%s\033[0m%s\n",
           "ERROR: ", "Failed to Run Nginx Web Server");
    exit(EXIT_FAILURE);
  } else if (unlikely(pidV2Ray == -1)) {
    printf("\033[0;31m%s\033[0m%s\n", "ERROR: ", "Failed to Fork");
    exit(EXIT_FAILURE);
  }
}

void runService_CloudflareTunnel(void) {
  if (CLOUDFLARE_TOKEN == NULL) {
    printf("\033[0;33m%s\033[0m%s\n",
           "WARNING: ", "Not Starting Cloudflare Tunnel");
    return;
  }

  // Start Cloudflare Tunnel
  printf("\033[0;32m%s\033[0m%s\n", "INFO: ", "Starting Cloudflare Tunnel");
  fflush(stdout);

  pid_t pidV2Ray = fork();
  if (likely(pidV2Ray == 0)) {
    fclose(stdin);
    if (!ENABLE_LOG_OUTPUT) {
      freopen(DEV_NULL, "w", stdout);
      freopen(DEV_NULL, "w", stderr);
    }
    execl("/usr/local/bin/cloudflared", "cloudflared", "tunnel",
          "--no-autoupdate", "run", "--token", CLOUDFLARE_TOKEN, NULL);
    printf("\033[0;31m%s\033[0m%s\n",
           "ERROR: ", "Failed to Run Cloudflare Tunnel");
    exit(EXIT_FAILURE);
  } else if (unlikely(pidV2Ray == -1)) {
    printf("\033[0;31m%s\033[0m%s\n", "ERROR: ", "Failed to Fork");
    exit(EXIT_FAILURE);
  }
}

int main(void) {
  // Refuse to Start as Non-Pid=1 Program
  if (getpid() != 1) {
    printf("\033[0;31m%s\033[0m%s\n", "ERROR: ", "Must be Run as PID 1");
    exit(EXIT_FAILURE);
  }

  processEnvironmentVariables();

  // Close Output Stream
  if (!ENABLE_LOG_OUTPUT) {
    freopen(DEV_NULL, "w", stdout);
    freopen(DEV_NULL, "w", stderr);
  }

  // Start Services
  runService_V2Ray();
  runService_Nginx();
  runService_CloudflareTunnel();

  // Collect Zombine Process
  while (1) {
    waitpid(-1, NULL, 0);
  }

  return EXIT_SUCCESS;
}

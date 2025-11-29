/* ************************************************************************** */
/*                                                                            */
/*                        ______                                              */
/*                     .-"      "-.                                           */
/*                    /            \                                          */
/*        _          |              |          _                              */
/*       ( \         |,  .-.  .-.  ,|         / )                             */
/*        > "=._     | )(__/  \__)( |     _.=" <                              */
/*       (_/"=._"=._ |/     /\     \| _.="_.="\_)                             */
/*              "=._ (_     ^^     _)"_.="                                    */
/*                  "=\__|IIIIII|__/="                                        */
/*                 _.="| \IIIIII/ |"=._                                       */
/*       _     _.="_.="\          /"=._"=._     _                             */
/*      ( \_.="_.="     `--------`     "=._"=._/ )                            */
/*       > _.="                            "=._ <                             */
/*      (_/                                    \_)                            */
/*                                                                            */
/*      Filename: socket_handler.c                                            */
/*      By: espadara <espadara@pirate.capn.gg>                                */
/*      Created: 2025/11/29 15:32:12 by espadara                              */
/*      Updated: 2025/11/29 15:34:51 by espadara                              */
/*                                                                            */
/* ************************************************************************** */

#include "ft_ping.h"

/**
 * setup_timeout - Configures the socket reception timeout.
 * @sock: The socket file descriptor.
 *
 * We set a timeout so 'recvfrom' doesn't block forever if a packet is lost.
 * This allows the loop to continue and send the next ping.
 */
static void setup_timeout(int sock)
{
  struct timeval timeout;

  timeout.tv_sec = 1;
  timeout.tv_usec = 0;
  if (setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) < 0)
    {
      sea_printf("Error: setsockopt failed: %s\n", strerror(errno));
      exit(EXIT_FAILURE);
    }
}

/**
 * resolve_hostname - Converts FQDN to IP address.
 * @ping: The global ping structure.
 *
 * Uses getaddrinfo to resolve. We only take the first valid IPv4 address.
 */
static void resolve_hostname(t_ping *ping)
{
  struct addrinfo hints;
  struct addrinfo *res;
  int             status;

  sea_memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_RAW;

    status = getaddrinfo(ping->hostname, NULL, &hints, &res);
    if (status != 0)
      {
        sea_printf("ft_ping: %s: %s\n", ping->hostname, gai_strerror(status));
        exit(EXIT_FAILURE);
      }
    sea_memcpy_fast(&ping->dest_addr, res->ai_addr, sizeof(struct sockaddr_in));
    inet_ntop(AF_INET, &(ping->dest_addr.sin_addr), ping->ip_str, INET_ADDRSTRLEN);
    freeaddrinfo(res);
}

/**
 * init_socket - The main setup routine.
 * @ping: The global ping structure.
 *
 * 1. Resolves hostname.
 * 2. Opens the RAW socket (requires root).
 * 3. Sets TTL and Timeout options.
 */
void init_socket(t_ping *ping)
{
  int ttl_val;

  resolve_hostname(ping);

  ping->sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
  if (ping->sockfd < 0)
    {
      // Usually fails here if not sudo
      // THAT'S WHY WE NEED TO DO IT IN A VM!
      if (errno == EPERM)
        sea_printf("ft_ping: Lacking privileges. Run as root/sudo.\n");
      else
        sea_printf("ft_ping: Socket error: %s\n", strerror(errno));
      exit(EXIT_FAILURE);
    }
    // Set Time-To-Live (TTL)
    ttl_val = TTL_DEFAULT;
    if (setsockopt(ping->sockfd, IPPROTO_IP, IP_TTL, &ttl_val, sizeof(ttl_val)) != 0)
      {
        sea_printf("ft_ping: Failed to set TTL\n");
        exit(EXIT_FAILURE);
      }
    setup_timeout(ping->sockfd);
    sea_printf("PING %s (%s): %d data bytes\n",
        ping->hostname, ping->ip_str, PING_PKT_SIZE - 8);
}

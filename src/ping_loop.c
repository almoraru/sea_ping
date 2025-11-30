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
/*      Filename: ping_loop.c                                                 */
/*      By: espadara <espadara@pirate.capn.gg>                                */
/*      Created: 2025/11/29 16:41:04 by espadara                              */
/*      Updated: 2025/11/30 16:40:59 by espadara                              */
/*                                                                            */
/* ************************************************************************** */

#include "ft_ping.h"

/* Helper: Calculate time difference in milliseconds */
static double get_time_diff(struct timeval *start, struct timeval *end)
{
  double s;
  double e;

  s = start->tv_sec * 1000.0 + start->tv_usec / 1000.0;
  e = end->tv_sec * 1000.0 + end->tv_usec / 1000.0;
  return (e - s);
}

static int check_deadline(t_ping *ping)
{
  struct timeval curr;
  double elapsed;

  if (ping->deadline == 0)
    return (0);

  gettimeofday(&curr, NULL);
  elapsed = get_time_diff(&ping->start_time, &curr) / 1000.0;

  if (elapsed >= ping->deadline)
    return (1);
  return (0);
}

static void update_stats(t_ping *ping, double rtt)
{
  ping->stats.rx_packets++;
  ping->stats.t_sum += rtt;
  ping->stats.t_sq_sum += (rtt * rtt); // For standard deviation later

  if (ping->stats.rx_packets == 1 || rtt < ping->stats.t_min)
    ping->stats.t_min = rtt;
  if (rtt > ping->stats.t_max)
    ping->stats.t_max = rtt;
}

static void print_reply(t_ping *ping, char *buf, ssize_t ret, double rtt)
{
  struct ip *ip;
  struct icmp *icmp;
  char src_ip[INET_ADDRSTRLEN];

  (void)ping;
  // IP Header is at the start of the buffer
  ip = (struct ip *)buf;
  // ICMP Header starts after the IP Header (length is in 32-bit words)
  icmp = (struct icmp *)(buf + (ip->ip_hl << 2));

  // Convert Source IP to string
  inet_ntop(AF_INET, &ip->ip_src, src_ip, INET_ADDRSTRLEN);

  // Format: "64 bytes from 1.2.3.4: icmp_seq=1 ttl=64 time=0.045 ms"
  sea_printf("%ld bytes from %s: icmp_seq=%d ttl=%d time=%.2f ms\n",
             ret - (ip->ip_hl << 2), // Payload size (Total - IP Header)
             src_ip,
             ntohs(icmp->icmp_seq),
             ip->ip_ttl,
             rtt
    );
}

void loop_ping(t_ping *ping)
{
  char            send_buf[PING_PKT_SIZE];
  char            recv_buf[RECV_BUFFER_SIZE];
  char            src_ip[INET_ADDRSTRLEN];
  struct sockaddr_in from_addr;
  socklen_t       addr_len;
  int             seq;
  ssize_t         ret;
  struct ip       *ip_header;
  struct icmp     *icmp_header;

  seq = 0;
  while (1)
    {

      // Check deadline
      if (check_deadline(ping))
        handle_signal(SIGINT);
      // --- SEND ---
      sea_bzero(send_buf, PING_PKT_SIZE);
      craft_packet(ping, send_buf, ++seq);
      if (sendto(ping->sockfd, send_buf, PING_PKT_SIZE, 0,
                 (struct sockaddr *)&ping->dest_addr, sizeof(ping->dest_addr)) < 0)
        {
          if (ping->verbose) sea_printf("ft_ping: sendto error\n");
          if (ping->flood) write(1, "E", 1);
        }
      else{
          ping->stats.tx_packets++;
          if (ping->flood) write (1, ".", 1);
          }

      // --- Recieve ---
        addr_len = sizeof(from_addr);
        ret = recvfrom(ping->sockfd, recv_buf, RECV_BUFFER_SIZE, 0,
                       (struct sockaddr *)&from_addr, &addr_len);
        if (ret > 0)
          {
            // Unpack IP Header to find ICMP
            ip_header = (struct ip *)recv_buf;
            icmp_header = (struct icmp *)(recv_buf + (ip_header->ip_hl << 2));

            // re-populate src_ip to avoid empty ip
            inet_ntop(AF_INET, &ip_header->ip_src, src_ip, INET_ADDRSTRLEN);

            // Check: Is it an Echo Reply (Type 0) and is it OURS (ID match)?
            if (icmp_header->icmp_type == ICMP_ECHOREPLY &&
                icmp_header->icmp_id == htons(ping->pid))
              {
                // Retrieve the timestamp we hid in the payload
                struct timeval sent_time;
                struct timeval curr_time;

                char *payload = recv_buf + (ip_header->ip_hl << 2) + 8;
                sea_memcpy_fast(&sent_time, payload, sizeof(sent_time));
                gettimeofday(&curr_time, NULL);

                double rtt = get_time_diff(&sent_time, &curr_time);
                update_stats(ping, rtt);

                if (ping->flood)
                  write(1, "\b \b", 3);
                else
                  print_reply(ping, recv_buf, ret, rtt);
              }
            else if (ping->verbose)
              {
                if (!ping->flood)
                  sea_printf("%d bytes from %s: type=%d code=%d\n",
                             ret, src_ip, icmp_header->icmp_type, icmp_header->icmp_code);
              }
          }
        if (!ping->flood)
          sleep(ping->interval);
    }
}

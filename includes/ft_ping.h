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
/*      Filename: ft_ping.h                                                   */
/*      By: espadara <espadara@pirate.capn.gg>                                */
/*      Created: 2025/11/29 12:34:36 by espadara                              */
/*      Updated: 2025/11/30 15:46:08 by espadara                              */
/*                                                                            */
/* ************************************************************************** */

#ifndef FT_PING_H
# define FT_PING_H

/* 🐙 KRAKENLIB INTEGRATION */
# include "krakenlib.h"

/* Network Specific Headers */
# include <signal.h>
# include <sys/time.h>
# include <arpa/inet.h>
# include <sys/socket.h>
# include <netdb.h>
# include <netinet/ip.h>
# include <netinet/ip_icmp.h>
# include <errno.h>

/* Configuration */
# define PING_PKT_SIZE 64
# define RECV_BUFFER_SIZE 1024
# define TTL_DEFAULT 64

/* ** The Global Logbook
** We use double for calculations to handle sub-millisecond precision.
*/
typedef struct s_ping_stats
{
    long    tx_packets;
    long    rx_packets;
    double  t_min;
    double  t_max;
    double  t_sum;
    double  t_sq_sum;
}   t_ping_stats;

typedef struct s_ping
{
    int                 sockfd;
    int                 pid;
    char                *hostname;
    char                ip_str[INET_ADDRSTRLEN];
    struct sockaddr_in  dest_addr;
    struct timeval      start_time;
    t_ping_stats        stats;
    int                 interval;
    int                 verbose;
    int                 flood;
}   t_ping;

/* Global Access for Signal Handlers */
extern t_ping *g_ping;

/* Prototypes */
unsigned short  checksum(void *b, int len);
void            init_socket(t_ping *ping);
void            craft_packet(t_ping *ping, char *buf, int seq);
void            loop_ping(t_ping *ping);
void            print_stats(t_ping *ping);
void            handle_signal(int sig);

#endif

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
/*      Filename: icmp_packer.c                                               */
/*      By: espadara <espadara@pirate.capn.gg>                                */
/*      Created: 2025/11/29 16:18:18 by espadara                              */
/*      Updated: 2025/11/29 16:19:44 by espadara                              */
/*                                                                            */
/* ************************************************************************** */


#include "ft_ping.h"

/**
 * craft_packet - Assembles the ICMP Echo Request.
 * @ping: The global struct (for PID/ID).
 * @buf: The stack-allocated buffer to write into.
 * @seq: The current sequence number.
 *
 * Structure:
 * [ ICMP Header (8 bytes) ] + [ Timestamp (16 bytes) ] + [ Padding/Data ]
 */
void craft_packet(t_ping *ping, char *buf, int seq)
{
  struct icmp *icmp;
  struct timeval *tv;

  // Point struct to buffer (No Malloc!)
  icmp = (struct icmp *)buf;

  // Set ICMP Header Fields
  icmp->icmp_type = ICMP_ECHO;  // Type 8 -> ECHO Request
  icmp->icmp_code = 0;          // Code 0
  icmp->icmp_id = htons(ping->pid); // Our PID identifies this ping session
  icmp->icmp_seq = htons(seq);  // Sequence number (network byte order)
  // Embed Timestamp in Payload
  // This allows us to calculate RTT statelessly when we get the reply.
  tv = (struct timeval *)(buf + sizeof(struct icmp));
  gettimeofday(tv, NULL);
  //  Calculate Checksum
  // Checksum must be 0 before calculation
  icmp->icmp_cksum = 0;
  icmp->icmp_cksum = checksum(buf, PING_PKT_SIZE);
}

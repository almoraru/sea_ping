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
/*      Filename: checksum.c                                                  */
/*      By: espadara <espadara@pirate.capn.gg>                                */
/*      Created: 2025/11/29 14:49:58 by espadara                              */
/*      Updated: 2025/11/29 15:22:37 by espadara                              */
/*                                                                            */
/* ************************************************************************** */

#include "ft_ping.h"

/**
 * checksum - Calculates the Internet Checksum (RFC 1071)
 * @b: Pointer to the data buffer
 * @len: Length of the data in bytes
 *
 * This function treats the data as 16-bit words, sums them up,
 * handles the carry, and returns the one's complement of the sum.
 */
unsigned short checksum(void *b, int len)
{
    unsigned short *buf;
    unsigned int    sum;
    unsigned short  result;

    buf = (unsigned short *)b;
    sum = 0;

    // Sum up 16-bit words
    while (len > 1)
    {
        sum += *buf++;
        len -= 2;
    }

    //  Handle odd byte (if length is odd)
    if (len == 1)
        sum += *(unsigned char *)buf;

    // Fold 32-bit sum to 16 bits
    // Add the high 16 bits to the low 16 bits until only 16 bits remain
    while (sum >> 16)
        sum = (sum & 0xFFFF) + (sum >> 16);

    // One's complement (bitwise NOT)
    result = ~sum;
    return (result);
}

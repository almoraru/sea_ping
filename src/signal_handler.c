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
/*      Filename: signal_handler.c                                            */
/*      By: espadara <espadara@pirate.capn.gg>                                */
/*      Created: 2025/11/29 21:30:17 by espadara                              */
/*      Updated: 2025/11/29 21:32:59 by espadara                              */
/*                                                                            */
/* ************************************************************************** */

#include "ft_ping.h"

/**
 * @ping: The struct containing sums and counts.
 * @avg: Pointer to store the average.
 * @stddev: Pointer to store the standard deviation.
 *
 * Uses the variance formula: Var = E[X^2] - (E[X])^2
 */
static void calculate_stats(t_ping *ping, double *avg, double *stddev)
{
  double variance;

  if (ping->stats.rx_packets > 0)
    {
      *avg = ping->stats.t_sum / ping->stats.rx_packets;
      // E[X^2] - (E[X])^2
      variance = (ping->stats.t_sq_sum / ping->stats.rx_packets) - (*avg * *avg);
      // Absolute value to handle floating point noise near zero
      *stddev = sqrt(fabs(variance));
    }
  else
    {
      *avg = 0.0;
      *stddev = 0.0;
    }
}

/**
 * Example:
 * --- google.com ping statistics ---
 * 4 packets transmitted, 4 packets received, 0% packet loss
 * round-trip min/avg/max/stddev = 14.1/14.2/14.3/0.1 ms
 */
void print_stats(t_ping *ping)
{
  double  avg;
  double  stddev;
  long    loss_pct;

  calculate_stats(ping, &avg, &stddev);

  if (ping->stats.tx_packets > 0)
    loss_pct = ((ping->stats.tx_packets - ping->stats.rx_packets) * 100)
      / ping->stats.tx_packets;
  else
    loss_pct = 0;

  sea_printf("--- %s ping statistics ---\n", ping->hostname);
  sea_printf("%ld packets transmitted, %ld packets received, %ld%% packet loss\n",
             ping->stats.tx_packets,
             ping->stats.rx_packets,
             loss_pct);

  if (ping->stats.rx_packets > 0)
    {
      sea_printf("round-trip min/avg/max/stddev = %.3f/%.3f/%.3f/%.3f ms\n",
                 ping->stats.t_min,
                 avg,
                 ping->stats.t_max,
                 stddev);
    }
}

/**
 * handle_signal - The actual interrupt handler.
 * @sig: The signal number (e.g., SIGINT).
 *
 * Closes the socket and exits cleanly.
 * Note: Since we use stack allocation in main, we don't need to free(g_ping).
 */
void handle_signal(int sig)
{
  (void)sig;

    // Move cursor down to not overwrite the ^C character on some terminals
  sea_printf("\n");

  if (g_ping)
    {
      print_stats(g_ping);
      if (g_ping->sockfd > 0)
        close(g_ping->sockfd);
    }

  exit(EXIT_SUCCESS);
}

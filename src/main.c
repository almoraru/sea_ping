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
/*      Filename: main.c                                                      */
/*      By: espadara <espadara@pirate.capn.gg>                                */
/*      Created: 2025/11/29 21:34:00 by espadara                              */
/*      Updated: 2025/11/29 21:34:59 by espadara                              */
/*                                                                            */
/* ************************************************************************** */

#include "ft_ping.h"

t_ping *g_ping = NULL;

static void init_struct(t_ping *ping, char *addr)
{
  sea_bzero(ping, sizeof(t_ping));
  ping->hostname = addr;
  ping->pid = getpid();
  ping->stats.t_min = 0.0;
  ping->stats.t_max = 0.0;
  // Default interval (1 second)
  ping->interval = 1;
  g_ping = ping;
}

int main(int argc, char **argv)
{
  t_ping  ping;

  if (getuid() != 0)
    {
      sea_printf("ft_ping: usage error: Destination address required\n");
      return (EXIT_FAILURE);
    }
  if (argc < 2)
    {
      sea_printf("ft_ping: usage error: Destination address required\n");
      return (EXIT_FAILURE);
    }
  init_struct(&ping, argv[1]);
  signal(SIGINT, handle_signal);
  init_socket(&ping);
  loop_ping(&ping);

  return (EXIT_SUCCESS);
}

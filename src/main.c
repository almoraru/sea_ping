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
/*      Updated: 2025/11/30 15:38:40 by espadara                              */
/*                                                                            */
/* ************************************************************************** */

#include "ft_ping.h"

t_ping *g_ping = NULL;

void print_usage(void)
{
    sea_printf("Usage: ft_ping [OPTION...] HOST ...\n");
    sea_printf("Send ICMP ECHO_REQUEST packets to network hosts.\n\n");
    sea_printf(" Options:\n");
    sea_printf("  -v, --verbose      verbose output\n");
    sea_printf("  -?, --help         give this help list\n");
    sea_printf("\n");
    sea_printf("Mandatory or optional arguments to long options are also mandatory for any corresponding short options.\n");
}

static void parse_args(t_ping *ping, int argc, char **argv)
{
  int i;

  for (i = 1; i < argc; i++)
    {
      if (argv[i][0] == '-')
        {
          if (sea_strcmp(argv[i], "-v") == 0 || sea_strcmp(argv[i], "--verbose") == 0)
            ping->verbose = 1;
          else if (sea_strcmp(argv[i], "-?") == 0 || sea_strcmp(argv[i], "--help") == 0)
            {
              print_usage();
              exit(EXIT_SUCCESS);
            }
          else
            {
              sea_printf("ft_ping: invalid option -- '%s'\n", argv[i] + 1);
              print_usage();
              exit(EXIT_FAILURE);
            }
        }
      else
        {
          if (ping->hostname != NULL)
            {
              // Already have a hostname? standard ping ignores extras or errors out.
              // We will error out to be strict.
              sea_printf("ft_ping: usage error: More than one destination.\n");
              exit(EXIT_FAILURE);
            }
          ping->hostname = argv[i];
        }
    }
  if (ping->hostname == NULL)
    {
      sea_printf("ft_ping: usage error: Destination address required\n");
      exit(EXIT_FAILURE);
    }
}

static void init_struct(t_ping *ping, char *addr)
{
  sea_bzero(ping, sizeof(t_ping));
  ping->hostname = addr;
  ping->pid = getpid();
  ping->stats.t_min = 0.0;
  ping->stats.t_max = 0.0;
  // Default interval (1 second)
  ping->interval = 1;
  ping->verbose = 0;
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
  // Init
  init_struct(&ping, argv[1]);
  // Parse
  parse_args(&ping, argc, argv);
  // Setup signals
  signal(SIGINT, handle_signal);
  // Launch
  init_socket(&ping);
  loop_ping(&ping);

  return (EXIT_SUCCESS);
}

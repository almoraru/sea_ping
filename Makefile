# ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;; #
#                                                                              #
#                        ______                                                #
#                     .-"      "-.                                             #
#                    /            \                                            #
#        _          |              |          _                                #
#       ( \         |,  .-.  .-.  ,|         / )                               #
#        > "=._     | )(__/  \__)( |     _.=" <                                #
#       (_/"=._"=._ |/     /\     \| _.="_.="\_)                               #
#              "=._ (_     ^^     _)"_.="                                      #
#                  "=\__|IIIIII|__/="                                          #
#                 _.="| \IIIIII/ |"=._                                         #
#       _     _.="_.="\          /"=._"=._     _                               #
#      ( \_.="_.="     `--------`     "=._"=._/ )                              #
#       > _.="                            "=._ <                               #
#      (_/                                    \_)                              #
#                                                                              #
#      Filename: Makefile                                                      #
#      By: espadara <espadara@pirate.capn.gg>                                  #
#      Created: 2025/11/29 12:33:21 by espadara                                #
#      Updated: 2025/11/29 14:49:56 by espadara                                #
#                                                                              #
# ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;; #


CC = cc
NAME = ft_ping

SRCS_PATH = src/
OBJ_PATH = objs/
INC_PATH = includes/

# --- Library Config: krakenlib ---
LIB_PATH = krakenlib/
LIB = $(addprefix $(LIB_PATH), krakenlib.a)
KRAKENLIB_URL = https://github.com/almoraru/krakenlib.git

INC = -I $(INC_PATH) -I $(LIB_PATH)includes -I $(LIB_PATH)
FLAGS = -Wall -Wextra -Werror -g
LDFLAGS = -L$(LIB_PATH)
LDLIBS = $(LIB) -lm

# Expected architecture
SOURCES = \
    main.c \
    socket_handler.c \
    icmp_packer.c \
    ping_loop.c \
    signal_handler.c \
    stats_calculator.c \
    checksum.c

OBJS = $(addprefix $(OBJ_PATH), $(SOURCES:.c=.o))
VPATH = $(SRCS_PATH)

# --- Rules ---

all: $(NAME)

$(NAME): $(OBJS) $(LIB)
	@echo "Linking $(NAME)..."
	$(CC) $(OBJS) $(LDFLAGS) $(LDLIBS) -o $(NAME)
	@echo "Executable '$(NAME)' ready to ping the seven seas! 🏴‍☠️"

# --- KrakenLib Auto-Clone & Update ---
$(LIB_PATH):
	@if [ -d "$(LIB_PATH)/.git" ]; then \
		echo "Updating krakenlib in $(LIB_PATH)..."; \
		(cd $(LIB_PATH) && git pull); \
	else \
		echo "Cloning krakenlib into $(LIB_PATH)..."; \
		git clone $(KRAKENLIB_URL) $(LIB_PATH); \
	fi

$(LIB): $(LIB_PATH)
	@echo "Awakening the Kraken..."
	@$(MAKE) -C $(LIB_PATH)

# --- Object Compilation ---
$(OBJ_PATH)%.o: %.c $(LIB) | $(OBJ_PATH)
	@echo "Compiling [ft_ping]: $<"
	$(CC) $(FLAGS) $(INC) -c $< -o $@

$(OBJ_PATH):
	@mkdir -p $(OBJ_PATH)

# --- Cleaning ---
clean:
	@if [ -d "$(LIB_PATH)" ]; then \
		$(MAKE) -C $(LIB_PATH) clean; \
	fi
	@/bin/rm -rf $(OBJ_PATH)
	@echo "[ft_ping] object files scrubbed."

fclean:
	@if [ -d "$(LIB_PATH)" ]; then \
		$(MAKE) -C $(LIB_PATH) fclean; \
	fi
	@/bin/rm -rf $(OBJ_PATH)
	@/bin/rm -f $(NAME)
	@echo "[ft_ping] executable thrown overboard."
	@/bin/rm -rf $(LIB_PATH)
	@echo "[ft_ping] library 'krakenlib' removed."

re:
	@$(MAKE) fclean
	@$(MAKE) all

.PHONY: all clean fclean re

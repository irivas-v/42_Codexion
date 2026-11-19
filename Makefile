# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: irivas-v <irivas-v@student.42madrid.com    +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2026/10/23 10:40:00 by irivas-v          #+#    #+#              #
#    Updated: 2026/11/16 11:35:00 by irivas-v         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME		= codexion

CC			= cc
CFLAGS		= -Wall -Wextra -Werror -pthread -Iincludes

HEADER		= includes/codexion.h

SRCS		= src/main.c \
			  src/parsing/parse_args.c \
			  src/utils/time.c \
			  src/utils/log.c \
			  src/heap/heap_priority.c \
			  src/heap/heap_push.c \
			  src/heap/heap_pop.c \
			  src/init/init_dongles.c \
			  src/init/init_coders.c \
			  src/init/init_simulation.c \
			  src/init/cleanup.c \
			  src/routine/take_dongles.c \
			  src/routine/release_dongles.c \
			  src/routine/actions.c \
			  src/routine/coder_routine.c \
			  src/monitor/monitor.c

OBJS		= $(SRCS:.c=.o)

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(NAME)

%.o: %.c $(HEADER)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re

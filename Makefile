NAME = codexion
CC = cc
CFLAGS = -Wall -Wextra -Werror -pthread -Iincludes
RM = rm -f
SRCS_DIR = srcs

SRCS = $(SRCS_DIR)/main.c \
    	$(SRCS_DIR)/parsing.c \
        $(SRCS_DIR)/time.c \
        $(SRCS_DIR)/init.c \
        $(SRCS_DIR)/actions.c \
        $(SRCS_DIR)/cleanup.c \
        $(SRCS_DIR)/heap.c \
        $(SRCS_DIR)/monitor.c \
        $(SRCS_DIR)/print_utils.c \
        $(SRCS_DIR)/routine.c \
        $(SRCS_DIR)/simulation.c

OBJS = $(SRCS:.c=.o)

all: $(NAME)

$(NAME): $(OBJS)
	@echo "⚙️  $(NAME) compiling..."
	@$(CC) $(CFLAGS) $(OBJS) -o $(NAME)
	@echo "✨ $(NAME) compiled successfully!"

%.o: %.c
	@$(CC) $(CFLAGS) -c $< -o $@

clean:
	@$(RM) $(OBJS)
	@echo "🧹 Object files removed."

fclean: clean
	@$(RM) $(NAME)
	@echo "🗑️  $(NAME) removed."

re: fclean all

.PHONY: all clean fclean re
NAME = wbs
CC = c++
CFLAGS = #-Wall -Wextra -Werror #-std=c++98# -fsanitize=address -g

SRCS = $(shell find ./ -name "*.cpp")

OBJS = $(SRCS:.cpp=.o)

HEADER = -I include/

all : $(NAME)

$(NAME) : $(OBJS) #$(HEADER)
	$(CC) $(CFLAGS) $(OBJS) -o $(NAME)

# $(OBJS) : $(HEADER)

%.o : %.cpp
	$(CC) $(CFLAGS) $(HEADER) -c $< -o $@

fclean : clean
	$(RM) $(NAME)

clean :
	$(RM) $(OBJS)

re : fclean all

.PHONY: all clean fclean re

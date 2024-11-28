NAME = Req
CC = c++
CFLAGS = #-Wall -Wextra -Werror #-std=c++98# -fsanitize=address -g

SRCS = $(shell find . -name "*.cpp")

OBJS = $(SRCS:.cpp=.o)

HEADERS = $(shell find . -name "*.hpp")

INCLUDE = -I include/

all : $(NAME)

$(NAME) : $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(NAME)

%.o : %.cpp $(HEADERS)
	$(CC) $(CFLAGS) $(INCLUDE) -c $< -o $@

fclean : clean
	$(RM) $(NAME)

clean :
	$(RM) $(OBJS)

re : fclean all

.PHONY: all clean fclean re
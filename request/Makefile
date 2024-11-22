NAME = Req
CC = c++
CFLAGS = #-Wall -Wextra -Werror -std=c++98# -fsanitize=address -g

SRCS = main.cpp Request.cpp Response.cpp

OBJS = $(SRCS:.cpp=.o)

HEADER = Request.hpp Response.hpp

all : $(NAME)

$(NAME) : $(OBJS) $(HEADER)
	$(CC) $(CFLAGS) $(OBJS) -o $(NAME)

$(OBJS) : $(HEADER)

%.o : %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

fclean : clean
	$(RM) $(NAME)

clean :
	$(RM) $(OBJS)

re : fclean all

.PHONY: all clean fclean re
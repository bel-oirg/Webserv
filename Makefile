NAME = webserv
RM = rm -rf
CC = c++
CFLAGS =  -Wall -Wextra -Werror -I include/ -std=c++98 -g -fsanitize=undefined 


SRCS = $(shell find ./srcs -name "*.cpp")

OBJS = $(SRCS:%.cpp=$(OBJ_DIR)/%.o)

HEADERS = $(shell find ./include -name "*.hpp")

INCLUDE = -I include/

OBJ_DIR = $(CURDIR)/Obj


all :
	make -j $(NAME)

$(NAME) : $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(NAME)

$(OBJ_DIR)/%.o : %.cpp $(HEADERS) Makefile
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(INCLUDE) -c $< -o $@


fclean : clean
	$(RM) $(NAME)


clean :
	$(RM) Obj

re : fclean all


.PHONY: all clean fclean re

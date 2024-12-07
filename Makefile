NAME = Req
RM = rm -rf
CC = c++
CFLAGS = -std=c++11 -pedantic  -fsanitize=address -g #-Wall -Wextra -Werror #-std=c++98# 

SRCS = $(shell find ./ -name "*.cpp")

OBJS = $(SRCS:%.cpp=$(OBJ_DIR)/%.o)

HEADERS = $(shell find . -name "*.hpp")

INCLUDE = -I include/

OBJ_DIR = $(CURDIR)/Obj


all : $(NAME)

$(NAME) : $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(NAME)

$(OBJ_DIR)/%.o : %.cpp $(HEADERS)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(INCLUDE) -c $< -o $@

fclean : clean
	$(RM) $(NAME)

clean :
	$(RM) Obj

re : fclean all

.PHONY: all clean fclean re
NAME = webserv
RM = rm -rf
CC = c++
CFLAGS = -Wall -Wextra -Werror -I include/ -I/usr/include/c++/11/ -I/usr/include/c++/11/x86_64-linux-gnu/ 


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
	$(RM) YoupiBanane
	$(RM) tester
	$(RM) ubuntu_tester
	$(RM) cgi_tester
	$(RM) ubuntu_cgi_tester

clean :
	$(RM) Obj

re : fclean all


init_test_mac :
	curl https://cdn.intra.42.fr/document/document/27434/tester -o		tester
	curl https://cdn.intra.42.fr/document/document/27436/cgi_tester -o	cgi_tester 
	mkdir YoupiBanane
	touch YoupiBanane/youpi.bad_extension
	touch YoupiBanane/youpi.bla
	mkdir -p YoupiBanane/nop
	touch YoupiBanane/nop/youpi.bad_extension
	touch YoupiBanane/nop/other.pouic
	mkdir -p YoupiBanane/Yeah
	touch YoupiBanane/Yeah/not_happy.bad_extension


init_test_linux :
	curl https://cdn.intra.42.fr/document/document/27435/ubuntu_cgi_tester -o  ubuntu_cgi_tester
	curl https://cdn.intra.42.fr/document/document/27437/ubuntu_tester -o ubuntu_tester
	mkdir YoupiBanane
	touch YoupiBanane/youpi.bad_extension
	touch YoupiBanane/youpi.bla
	mkdir -p YoupiBanane/nop
	touch YoupiBanane/nop/youpi.bad_extension
	touch YoupiBanane/nop/other.pouic
	mkdir -p YoupiBanane/Yeah
	touch YoupiBanane/Yeah/not_happy.bad_extension

.PHONY: all clean fclean re

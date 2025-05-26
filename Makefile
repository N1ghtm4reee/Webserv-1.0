CC = c++
FLAGS = -Iincludes #-Wall -Wextra -Werror -std=c++98
SRC = main.cpp utils.cpp Request.cpp EventLoop.cpp Response.cpp ConfigParser.cpp
OBJ = $(SRC:.cpp=.o)
NAME = Webserv

all : $(NAME)

$(NAME) : $(OBJ)
	$(CC) $(FLAGS) $(OBJ) -o $(NAME)

%.o: %.cpp
	$(CC) $(FLAGS) -c $< -o $@

clean :
	@rm -f $(OBJ)

fclean : clean
	@rm -f $(NAME)

re : fclean all


.SECONDARY: $(OBJ)
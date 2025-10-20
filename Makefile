NAME		= scop

CXX			= g++
CXXFLAGS	= -Wall -Wextra -Werror -std=c++17
DEBUGFLAGS	= -g -fsanitize=address
OPTFLAGS	= -O3 -DNDEBUG

LIBS		= -lglfw -lGLEW -lGL -lGLU -lm -lSOIL

SRCDIR		= src
INCDIR		= include
OBJDIR		= obj
SHADERDIR	= shaders

INCLUDES	= -I$(INCDIR) -I/usr/include/GL

SRCS		= $(SRCDIR)/main.cpp $(SRCDIR)/keyhandler.cpp $(SRCDIR)/obj_parser.cpp $(SRCDIR)/render_obj.cpp $(SRCDIR)/texture.cpp $(SRCDIR)/material.cpp
OBJS		= $(SRCS:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o)

RED			= \033[0;31m
GREEN		= \033[0;32m
YELLOW		= \033[0;33m
BLUE		= \033[0;34m
MAGENTA		= \033[0;35m
CYAN		= \033[0;36m
WHITE		= \033[0;37m
RESET		= \033[0m

all: $(NAME)

$(NAME): $(OBJS)
	@echo "$(CYAN)Linking $(NAME)...$(RESET)"
	@$(CXX) $(CXXFLAGS) $(OBJS) -o $(NAME) $(LIBS)
	@echo "$(GREEN)✓ $(NAME) created successfully!$(RESET)"

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	@mkdir -p $(OBJDIR)
	@echo "$(YELLOW)Compiling $<...$(RESET)"
	@$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

clean:
	@echo "$(RED)Cleaning object files...$(RESET)"
	@rm -rf $(OBJDIR)

fclean: clean
	@echo "$(RED)Cleaning executable...$(RESET)"
	@rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
#******************************************************************************#
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:   #
#                                                     +:+ +:+         +:+      #
#    By: mmoumini <https://moustoifa.moumini.xyz/>  +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/04/21 12:11:44 by mmoumini          #+#    #+#              #
#    Updated: 2025/04/27 14:41:34 by mmoumini         ###   ########.fr        #
#                                                                              #
#******************************************************************************#

UNAME_S := $(shell uname -s)

# Detect OS and set variables
ifeq ($(OS),Windows_NT)
    # Windows (MinGW/MSYS2)
    NAME = monjeu.exe
    RM = rm -f
    RMDIR = rm -rf
    LIB_DIR = lib/Windows
    LIB_EXT = .a
    # Windows static libraries linking (MinGW uses .a files)
    # Use --whole-archive for Windows (MinGW doesn't support -force_load)
    # Use --start-group/--end-group to handle circular dependencies
    SFML_LIBS = -Wl,--whole-archive \
                -L$(LIB_DIR) -lsfml-graphics -lsfml-window -lsfml-audio -lsfml-network -lsfml-system \
                -Wl,--no-whole-archive \
                -Wl,--start-group \
                -L$(LIB_DIR) -lvorbisfile -lvorbisenc -lvorbis -logg -lFLAC -lfreetype \
                -Wl,--end-group \
                -lopengl32 -lwinmm -lgdi32 -lws2_32
else ifeq ($(UNAME_S),Darwin)
    # macOS
    NAME = monjeu
    RM = rm -f
    RMDIR = rm -rf
    LIB_DIR = lib/macOS
    LIB_EXT = .a
    # macOS static libraries with force_load for Objective-C categories
    SFML_LIBS = -Wl,-force_load,$(LIB_DIR)/libsfml-graphics$(LIB_EXT) \
                -Wl,-force_load,$(LIB_DIR)/libsfml-window$(LIB_EXT) \
                -Wl,-force_load,$(LIB_DIR)/libsfml-audio$(LIB_EXT) \
                -Wl,-force_load,$(LIB_DIR)/libsfml-network$(LIB_EXT) \
                -Wl,-force_load,$(LIB_DIR)/libsfml-system$(LIB_EXT) \
                -L$(LIB_DIR) -lFLAC -lvorbis -lvorbisenc -lvorbisfile -logg -lfreetype \
                -framework OpenGL -framework AppKit -framework IOKit \
                -framework CoreServices -framework Carbon -framework OpenAL
else
    # Linux
    NAME = monjeu
    RM = rm -f
    RMDIR = rm -rf
    LIB_DIR = lib/linux
    LIB_EXT = .a
    # Linux static libraries
    SFML_LIBS = -Wl,--whole-archive \
                -L$(LIB_DIR) -lsfml-graphics -lsfml-window -lsfml-audio -lsfml-network -lsfml-system \
                -Wl,--no-whole-archive \
                -L$(LIB_DIR) -lFLAC -lvorbis -lvorbisenc -lvorbisfile -logg -lfreetype \
                -lGL -lX11 -lpthread -lrt
endif

CC = g++
CFLAGS = -Wall -Wextra -std=c++17 -DSFML_STATIC
INC = -I./include
LIB_PATH = $(SFML_LIBS)

SRC = src/main.cpp 
OBJ = $(SRC:.cpp=.o)

all: $(NAME)

$(NAME): $(OBJ)
	$(CC) $(OBJ) -o $(NAME) $(CFLAGS) $(LIB_PATH) $(INC)

%.o: %.cpp
	$(CC) -c $< -o $@ $(CFLAGS) $(INC)

clean:
	@echo "Cleaning object files..."
	$(RM) $(OBJ)

fclean: clean
	@echo "Cleaning executable..."
	$(RM) $(NAME)

re: fclean all

.PHONY: all clean fclean re

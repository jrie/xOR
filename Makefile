#Makefile template from Lazyfoo Productions
#OBJS specifies which files to compile as part of the project
OBJS = xor.c

#CC specifies which compiler we're using
CC = gcc

#COMPILER_FLAGS specifies the additional compilation options we're using
# -w suppresses all warnings
COMPILER_FLAGS = -Wall -std=c11 -O2

#LINKER_FLAGS specifies the libraries we're linking against
#LINKER_FLAGS = -I/usr/include/X11 -lX11
LINKER_FLAGS = -lm

#OBJ_NAME specifies the name of our exectuable
OBJ_NAME = xor
ARGS = nomoresecrets passa passb -iinput.txt -ooutput.txt
#This is the target that compiles our executable
all : $(OBJS)
	$(CC) $(OBJS) $(COMPILER_FLAGS) $(LINKER_FLAGS) -o $(OBJ_NAME)

CC = gcc

INCDIR = ./include

#--------------------Include files-----------------------------------------
CFLAGS      += -I$(INCDIR)/
#--------------------link files-----------------------------------------
LIBS		+= -lm

#-------------------- Build --------------------------------------
PACKAGE = SDRParser
SRC = $(PACKAGE).c
OBJ = $(PACKAGE).o
OUTPUT = $(PACKAGE)

all:
	$(CC) -c $(SRC) $(CFLAGS)
	$(CC) -o $(OUTPUT) $(OBJ) $(LIBS)

clean:
	rm -rf $(OUTPUT)
	rm -rf $(OBJ)

CC = g++
MACRO = -O3 -DMPI16 -DCTOC
#OPT = -O1
#OPT = -g -DMPI16 -DDEBUG
OPT = $(MACRO) #-DDEBUG_FILE_READ #-DDEBUG_DETAIL -DDEBUG_FILE_READ
#OPT = -O3 -DOMP16
WARN = -Wall
ERR = -Werror
INC =
LIB = 

CFLAGS = $(OPT) $(WARN) $(ERR) $(INC) $(LIB)

SIM_SRC = main.cc sim.cc cache.cc directory.cc

SIM_OBJ = main.o sim.o cache.o directory.o

all: cmn_sim
	@echo "Compilation Done."

cmn_sim: $(SIM_OBJ)
	$(CC) -o cmn_sim $(CFLAGS) $(SIM_OBJ) -lm
 
.cc.o:
	$(CC) $(CFLAGS) -c $*.cc

clean:
	rm -f *.o cmn_sim

clobber:
	rm -f *.o



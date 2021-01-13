PROJECT_INC = -I. -I.. -I../.. -I../../CgosLib
PROJECT_LIB =
TARGET = libcgos.so
C_LIBSRC = LibOsaLx.c ../LibOsaM.c
C_SRC = ../Cgos.c
C_OBJ = libcgosp.o
DEF = -DUNIX
OPT = -Wall -fPIC

default: $(TARGET)

$(TARGET): $(C_LIBSRC) $(C_OBJ)
	gcc -shared -o $(TARGET) $(OPT) $(DEF) $(PROJECT_INC) $(C_LIBSRC) $(C_OBJ)

$(C_OBJ): $(C_SRC)
	gcc -Wl,-r -nostdlib -o $@ $(OPT) $(DEF) $(PROJECT_INC) $(C_SRC)

clean:
	rm -f $(TARGET)

cleanall:
	rm -f $(TARGET) $(C_OBJ)

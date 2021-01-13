PROJECT_INC = -I. -I.. -I../.. -I../../CgosLib
PROJECT_LIB = -lcgos -L../../CgosLib/Lx
TARGET = cgosbcgpio
C_SRC = ../CgosBCgpio.c
OPT = -Wall

default: $(TARGET)

$(TARGET): $(C_SRC)
	gcc $(C_SRC) -o $(TARGET) $(OPT) $(DEF) $(PROJECT_INC) $(PROJECT_LIB)

clean:
	rm -f $(TARGET)

cleanall: clean


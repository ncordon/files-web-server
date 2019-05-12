BIN=./bin
DOC=./doc
INCLUDE=./include
LIB=./lib
OBJ=./obj
SRC=./src

all: FLAGS= -Wall -g -std=c++0x
grupos: FLAGS= -Wall -g -std=c++0x -DGRUPOS

all: clean $(OBJ)/cifras.o $(OBJ)/main.o
	g++ -o $(BIN)/RetoII  $(OBJ)/cifras.o $(OBJ)/main.o $(FLAGS)
grupos: clean $(OBJ)/cifras.o $(OBJ)/main.o
	g++ -o $(BIN)/Grupos $(OBJ)/cifras.o $(OBJ)/main.o $(FLAGS)
$(OBJ)/cifras.o: $(SRC)/cifras.cpp
	g++ -o $(OBJ)/cifras.o -c $< -I$(INCLUDE) $(FLAGS)
$(OBJ)/main.o: $(SRC)/main.cpp
	g++ -o $(OBJ)/main.o -c $< -I$(INCLUDE) $(FLAGS)
clean:
	rm -f $(OBJ)/*.o
doc:
	doxygen $(DOC)/doxys/Doxyfile

.PHONY: clean

BIN=./bin
DOC=./doc
INCLUDE=./include
LIB=./lib
OBJ=./obj
SRC=./src

all: FLAGS= -Wall -g -std=c++11

all: clean $(OBJ)/file.o $(OBJ)/server.o $(OBJ)/main.o
	g++ -o $(BIN)/file-indexer $(OBJ)/main.o $(OBJ)/file.o $(OBJ)/server.o
$(OBJ)/file.o: $(SRC)/file.cc
	g++ -o $(OBJ)/file.o -c $< -I$(INCLUDE) $(FLAGS)
$(OBJ)/server.o: $(SRC)/server.cc
	g++ -o $(OBJ)/server.o -c $< -I$(INCLUDE) $(FLAGS)
$(OBJ)/main.o: $(SRC)/main.cc
	g++ -o $(OBJ)/main.o -c $< -I$(INCLUDE) $(FLAGS)
clean:
	rm -f $(OBJ)/*.o

.PHONY: clean

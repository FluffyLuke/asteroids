SRC=src
INCLUDE=include
BIN=target

FLAGS=-Wall --std=c++23

SOURCE_FILES=$(wildcard $(SRC)/*.cpp)

$(BIN)/game: $(SOURCE_FILES)
	g++ $(FLAGS) -o $(BIN)/game $(SOURCE_FILES)

clean:
	rm -r ./target/*
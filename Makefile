CXX		  := g++
CXX_FLAGS := -w -std=c++17 -ggdb

BIN			 := bin
SRC			 := src/source.cpp

SDL_CONFIG   := `sdl2-config --cflags --libs` -lSDL2_image

LIBRARIES	:=
EXECUTABLE	:= my_game


all: $(SRC)
	$(CXX) $(CXX_FLAGS) $(SRC) $(SDL_CONFIG) -o $(BIN)/$(EXECUTABLE)

run: clean all
	clear
	./$(BIN)/$(EXECUTABLE)

clean:
	-rm $(BIN)/*
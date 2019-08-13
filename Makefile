CXX		  := g++
CXX_FLAGS := -w -std=c++17 -ggdb

BIN			 := bin
SRC			 := src/*

SDL_CONFIG   := `sdl2-config --cflags --libs` -lSDL2_image -lSDL2_ttf

LIBRARIES	:=
EXECUTABLE	:= my_game


all: $(SRC)
	$(CXX) $(CXX_FLAGS) $(SRC) $(SDL_CONFIG) $(LIBRARIES) -o $(BIN)/$(EXECUTABLE)

run: clean all
	clear
	./$(BIN)/$(EXECUTABLE)

clean:
	-rm $(BIN)/*

OBJ = obj
CPP_FILES = $(wildcard ./*.cpp)
OBJ_FILES = $(addprefix obj/,$(notdir $(CPP_FILES:.cpp=.o)))
TARGET = zajaczki.out

build: create_directory $(TARGET)
	@echo "Build successful!"
	@echo "Type mpirun -np 2 ./$(TARGET) to run the program"
$(TARGET): $(OBJ_FILES)
	mpic++ $^ -o $(TARGET)
$(OBJ)/main.o: main.cpp
	mpic++ -c $< -o $@
create_directory: 
	@mkdir -p $(OBJ)
clean:
	@rm -r -f $(OBJ) $(TARGET)


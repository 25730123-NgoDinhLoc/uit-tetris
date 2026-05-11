# --- Compiler settings ---
CXX      = g++                           # C++ compiler
CXXFLAGS = -std=c++17 -Wall -Wextra -O2  # language standard, all warnings, optimize
LDLIBS   = -lsfml-graphics -lsfml-window -lsfml-system  # SFML libraries for graphics and windowing

# --- File lists ---
TARGET   = tetris                        # final executable name
SRCS     = main.cpp tetris.cpp           # all .cpp source files
OBJS     = $(SRCS:.cpp=.o)               # replace .cpp with .o to get object file names

# --- Targets ---
.PHONY: all run clean                     # these targets are not real files

# Default target: build the executable
all: $(TARGET)

# Link step: combine all object files into the executable
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDLIBS)
#   $@ = the target name (tetris)
#   $^ = all prerequisites (main.o tetris.o)

# Compile step: turn each .cpp into a .o object file
# Also depend on tetris.h so changes to the header trigger recompilation
%.o: %.cpp tetris.h
	$(CXX) $(CXXFLAGS) -c $< -o $@
#   $< = the first prerequisite (the .cpp file)
#   $@ = the target name (the .o file)

# Run the game after building
run: $(TARGET)
	./$(TARGET)

# Remove all generated files (objects + executable)
clean:
	rm -f $(OBJS) $(TARGET)


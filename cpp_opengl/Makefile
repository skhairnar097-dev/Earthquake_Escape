# Earthquake Escape - OpenGL Makefile

# Compiler
CXX = g++
CXXFLAGS = -O2 -Wall -I./include

# Windows (MinGW / FreeGLUT)
ifeq ($(OS),Windows_NT)
    TARGET = earthquake_escape.exe
    LIBS = -L./lib -lfreeglut -lopengl32 -lglu32
    CLEAN_CMD = del /Q /F $(TARGET) *.o 2>nul || exit 0
else
    # Linux
    UNAME_S := $(shell uname -s)
    ifeq ($(UNAME_S),Linux)
        TARGET = earthquake_escape
        LIBS = -lglut -lGL -lGLU -lm
        CLEAN_CMD = rm -f $(TARGET) *.o
    endif
    # macOS
    ifeq ($(UNAME_S),Darwin)
        TARGET = earthquake_escape
        LIBS = -framework OpenGL -framework GLUT
        CLEAN_CMD = rm -f $(TARGET) *.o
    endif
endif

all: $(TARGET)

$(TARGET): earthquake_escape.cpp
	$(CXX) $(CXXFLAGS) earthquake_escape.cpp -o $(TARGET) $(LIBS)

clean:
	$(CLEAN_CMD)

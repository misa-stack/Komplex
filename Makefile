SOURCES = $(wildcard *.cpp)
OBJECTS = $(SOURCES:.cpp=.o)
DEPENDS = $(SOURCES:.cpp=.d)
TARGET = program

CFLAGS = -ggdb

LFLAGS = -lSDL -lSDL_image

all: $(TARGET)

$(TARGET): $(OBJECTS)
	g++ $(OBJECTS) -o $(TARGET) $(LFLAGS)

%.o: %.cpp
	g++ -MMD -MP $(CFLAGS) -c $< -o $@

-include $(DEPENDS)

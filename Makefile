CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -pthread
TARGET = LiteRedis
SRCS = main.cpp LiteRedis.cpp

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) $(SRCS) -o $(TARGET)

clean:
	rm -f $(TARGET) *.o
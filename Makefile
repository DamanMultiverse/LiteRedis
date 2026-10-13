# Compiler settings
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -pthread

# Project files (NAYA: Logger.cpp add kiya)
SRCS = main.cpp LiteRedis.cpp Logger.cpp
OBJS = $(SRCS:.cpp=.o)
TARGET = LiteRedis

# Build rules
all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)
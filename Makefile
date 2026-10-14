# Compiler settings
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -pthread

# Source files (Added TcpServer.cpp)
SRCS = main.cpp LiteRedis.cpp Logger.cpp TcpServer.cpp
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
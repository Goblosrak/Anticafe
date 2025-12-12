# Компилятор
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2
LDFLAGS = 

# Определяем ОС
UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Linux)
    # Linux
    CXXFLAGS += -I/usr/include/jsoncpp
    LDFLAGS += -ljsoncpp -lpthread
endif
ifeq ($(UNAME_S),Darwin)
    # macOS
    CXXFLAGS += -I/opt/homebrew/include
    LDFLAGS += -L/opt/homebrew/lib -ljsoncpp -lpthread
endif
ifeq ($(OS),Windows_NT)
    # Windows
    CXXFLAGS += -D_WIN32_WINNT=0x0601
    LDFLAGS = -lws2_32 -ljsoncpp
endif

# Цели
TARGET = antycafe_server
SRCS = main.cpp
OBJS = $(SRCS:.cpp=.o)

# Правила
all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

run: $(TARGET)
	./$(TARGET)

# Зависимости
main.o: main.cpp

.PHONY: all clean run
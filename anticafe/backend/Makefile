# Makefile для Антикафе Чилл
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -pthread
LDFLAGS = -pthread
TARGET = antycafe_server
SRC_DIR = backend/server
BUILD_DIR = build
SRC = $(SRC_DIR)/main.cpp
OBJ = $(BUILD_DIR)/main.o

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) $(OBJ) $(LDFLAGS) -o $(BUILD_DIR)/$(TARGET)

$(BUILD_DIR)/main.o: $(SRC)
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR)/*

run: $(TARGET)
	@echo "Запуск сервера..."
	@cd $(BUILD_DIR) && ./$(TARGET)

install-deps-ubuntu:
	sudo apt update
	sudo apt install -y g++ cmake

install-deps-fedora:
	sudo dnf install -y gcc-c++ cmake

install-deps-arch:
	sudo pacman -Syu
	sudo pacman -S gcc cmake

.PHONY: all clean run install-deps
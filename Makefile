# Compiler settings
CXX := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -O2

# Platform-specific settings
ifeq ($(OS),Windows_NT)
    LDFLAGS := -lws2_32
    SERVER_EXE := build/server.exe
    CLIENT_EXE := build/client.exe
    # A simpler RM command that is less verbose
    RM := del /F /Q build\\* 2>nul || echo No executables to delete.
    MKDIR := if not exist build mkdir build
    RMDIR := if exist build rmdir /S /Q build
else
    LDFLAGS := -pthread
    SERVER_EXE := build/server
    CLIENT_EXE := build/client
    RM := rm -f $(SERVER_EXE) $(CLIENT_EXE)
    MKDIR := mkdir -p build
    RMDIR := rm -rf build
endif

# Source files
SRC_DIR := src
SERVER_SRCS := $(SRC_DIR)/server.cpp $(SRC_DIR)/kvstore.cpp
CLIENT_SRCS := $(SRC_DIR)/client.cpp

# Default target
all: $(SERVER_EXE) $(CLIENT_EXE)

# --- FIX IS HERE ---
# Add a target for the build directory.
# The server and client executables will depend on this.
build:
	@$(MKDIR)

# Build server - Now depends on the 'build' target
$(SERVER_EXE): $(SERVER_SRCS) $(SRC_DIR)/kvstore.h | build
	@echo Building server...
	$(CXX) $(CXXFLAGS) $(SERVER_SRCS) -o $@ $(LDFLAGS)
	@echo Server built successfully!

# Build client - Now depends on the 'build' target
# Pro-tip: Use $^ instead of $< to include all source files if you ever add more.
$(CLIENT_EXE): $(CLIENT_SRCS) | build
	@echo Building client...
	$(CXX) $(CXXFLAGS) $(CLIENT_SRCS) -o $@ $(LDFLAGS)
	@echo Client built successfully!

# Clean rule
clean:
	@echo Cleaning project...
	@$(RMDIR)
	@echo Clean operation completed.

.PHONY: all clean build
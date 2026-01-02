CXX=g++
CXXFLAGS=-std=c++17 -Wall -Wextra -O2

SRCS=src/main.cpp \
     src/01_mem/mem_Allocator.cpp \
     src/02_buddy/Buddy_allocator.cpp \
     src/03_cache/Cache.cpp \
     src/04_vm/Virtual_memory.cpp

TARGET=memsim

all: $(TARGET)

$(TARGET): $(SRCS)
    $(CXX) $(CXXFLAGS) -o $@ $(SRCS)

clean:
    rm -f $(TARGET)

.PHONY: all clean
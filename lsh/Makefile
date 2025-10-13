# compiler + flags
CXX      := g++
CXXFLAGS := -std=gnu++17 -O2 -Wall -Wextra

# target and sources
# TARGET := main
# SRCS   := main.cpp Euclidean_Hashing.cpp
# OBJS   := $(SRCS:.cpp=.o)

TARGET := search
SRCS   := search.cpp Euclidean_Hashing.cpp ../utils_functions/Data_loader.cpp ../bruteforce/BruteForceImplementation.cpp ../utils_functions/Euclidean_dist.cpp 
OBJS   := $(SRCS:.cpp=.o)


# default rule
all: $(TARGET)

# link
$(TARGET): $(OBJS)
	$(CXX) $(OBJS) -o $@

# compile each .cpp to .o (header is a dependency)
%.o: %.cpp Euclidean_Hashing.h
	$(CXX) $(CXXFLAGS) -c $< -o $@

# convenience
run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(OBJS) $(TARGET)

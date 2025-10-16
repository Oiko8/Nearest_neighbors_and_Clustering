# compiler + flags
CXX      := g++
CXXFLAGS := -std=gnu++17 -O2 -Wall -Wextra

TARGET := search
SRCS := search.cpp Euclidean_Hashing.cpp
SRCS += ../utils_functions/Data_loader.cpp
SRCS += ../bruteforce/BruteForceImplementation.cpp
SRCS += ../utils_functions/Euclidean_dist.cpp
OBJS   := $(SRCS:.cpp=.o)


# default rule
all: $(TARGET)

# link
$(TARGET): $(OBJS)
	$(CXX) $(OBJS) -o $@


clean:
	rm -f $(OBJS) $(TARGET)


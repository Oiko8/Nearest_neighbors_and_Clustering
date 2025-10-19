# compiler + flags
CXX      := g++
CXXFLAGS := -std=gnu++17 -O2 -Wall -Wextra

TARGET := search
SRCS := search.cpp
SRCS += lsh/LSHmain.cpp lsh/Euclidean_Hashing.cpp
SRCS += hypercube/HCmain.cpp hypercube/Hypercube.cpp 
SRCS += utils_functions/Data_loader.cpp
SRCS += bruteforce/BruteForceImplementation.cpp
SRCS += utils_functions/Euclidean_dist.cpp utils_functions/Rangesearch.cpp
OBJS   := $(SRCS:.cpp=.o)


# default rule
all: $(TARGET)

# link
$(TARGET): $(OBJS)
	$(CXX) $(OBJS) -o $@


clean:
	rm -f $(OBJS) $(TARGET)
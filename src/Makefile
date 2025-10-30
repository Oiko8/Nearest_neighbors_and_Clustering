# compiler + flags
CXX      := g++
CXXFLAGS := -std=gnu++17 -O2 -Wall -Wextra

TARGET := search
SRCS := search.cpp
SRCS += lsh/LSHmain.cpp lsh/Euclidean_Hashing.cpp
SRCS += hypercube/HCmain.cpp hypercube/Hypercube.cpp 
SRCS += ivf/ivf_flat.cpp ivf/ivfflat_main.cpp
SRCS += ivf/ivf_pq.cpp ivf/ivfpq_main.cpp
SRCS += ivf/kmeans.cpp
SRCS += ../utils_functions/Data_loader.cpp ../utils_functions/io.cpp ../utils_functions/euclid.cpp ../utils_functions/nearest_neighbor.cpp
OBJS   := $(SRCS:.cpp=.o)


# default rule
all: $(TARGET)

# link
$(TARGET): $(OBJS)
	$(CXX) $(OBJS) -o $@


clean:
	rm -f $(OBJS) $(TARGET)
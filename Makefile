# Define variables for the include and library paths
LIBCURL_INCLUDE := C:\Users\RepCyber\Downloads\curl-8.0.1_6-win64-mingw\include
LIBCURL_LIB := C:\Users\RepCyber\Downloads\curl-8.0.1_6-win64-mingw\lib

# Define the compiler and linker flags
CXXFLAGS := -I $(LIBCURL_INCLUDE)
LDFLAGS := -L $(LIBCURL_LIB) -lcurl

# Define the source files, objects, and the target executable
SRCS := main.cpp
OBJS := $(SRCS:.cpp=.o)
TARGET := main

# The default target to build the executable
all: $(TARGET)

# Rule to build the executable
$(TARGET): $(OBJS)
	$(CXX) $(OBJS) -o $@ $(LDFLAGS)

# Rule to build the object files
%.o: %.cpp
	$(CXX) -c $< -o $@ $(CXXFLAGS)

# Rule to clean the build artifacts
clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all clean
# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.20

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Disable VCS-based implicit rules.
% : %,v

# Disable VCS-based implicit rules.
% : RCS/%

# Disable VCS-based implicit rules.
% : RCS/%,v

# Disable VCS-based implicit rules.
% : SCCS/s.%

# Disable VCS-based implicit rules.
% : s.%

.SUFFIXES: .hpux_make_needs_suffix_list

# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

#Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:
.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/local/bin/cmake

# The command to remove a file.
RM = /usr/local/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/yyx/sharehash

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/yyx/sharehash/cmake-build-debug

# Include any dependencies generated for this target.
include CMakeFiles/sharehash.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/sharehash.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/sharehash.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/sharehash.dir/flags.make

CMakeFiles/sharehash.dir/main.cpp.o: CMakeFiles/sharehash.dir/flags.make
CMakeFiles/sharehash.dir/main.cpp.o: ../main.cpp
CMakeFiles/sharehash.dir/main.cpp.o: CMakeFiles/sharehash.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/yyx/sharehash/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/sharehash.dir/main.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/sharehash.dir/main.cpp.o -MF CMakeFiles/sharehash.dir/main.cpp.o.d -o CMakeFiles/sharehash.dir/main.cpp.o -c /home/yyx/sharehash/main.cpp

CMakeFiles/sharehash.dir/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/sharehash.dir/main.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/yyx/sharehash/main.cpp > CMakeFiles/sharehash.dir/main.cpp.i

CMakeFiles/sharehash.dir/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/sharehash.dir/main.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/yyx/sharehash/main.cpp -o CMakeFiles/sharehash.dir/main.cpp.s

# Object files for target sharehash
sharehash_OBJECTS = \
"CMakeFiles/sharehash.dir/main.cpp.o"

# External object files for target sharehash
sharehash_EXTERNAL_OBJECTS =

sharehash: CMakeFiles/sharehash.dir/main.cpp.o
sharehash: CMakeFiles/sharehash.dir/build.make
sharehash: CMakeFiles/sharehash.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/yyx/sharehash/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable sharehash"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/sharehash.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/sharehash.dir/build: sharehash
.PHONY : CMakeFiles/sharehash.dir/build

CMakeFiles/sharehash.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/sharehash.dir/cmake_clean.cmake
.PHONY : CMakeFiles/sharehash.dir/clean

CMakeFiles/sharehash.dir/depend:
	cd /home/yyx/sharehash/cmake-build-debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/yyx/sharehash /home/yyx/sharehash /home/yyx/sharehash/cmake-build-debug /home/yyx/sharehash/cmake-build-debug /home/yyx/sharehash/cmake-build-debug/CMakeFiles/sharehash.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/sharehash.dir/depend

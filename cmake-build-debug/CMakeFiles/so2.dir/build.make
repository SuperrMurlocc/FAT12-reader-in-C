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
CMAKE_COMMAND = "/Users/jakubbednarski/Library/Application Support/JetBrains/Toolbox/apps/CLion/ch-0/212.5080.54/CLion.app/Contents/bin/cmake/mac/bin/cmake"

# The command to remove a file.
RM = "/Users/jakubbednarski/Library/Application Support/JetBrains/Toolbox/apps/CLion/ch-0/212.5080.54/CLion.app/Contents/bin/cmake/mac/bin/cmake" -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /Users/jakubbednarski/Coding/CLionProjects/so2

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /Users/jakubbednarski/Coding/CLionProjects/so2/cmake-build-debug

# Include any dependencies generated for this target.
include CMakeFiles/so2.dir/depend.make
# Include the progress variables for this target.
include CMakeFiles/so2.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/so2.dir/flags.make

CMakeFiles/so2.dir/main.c.o: CMakeFiles/so2.dir/flags.make
CMakeFiles/so2.dir/main.c.o: ../main.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/jakubbednarski/Coding/CLionProjects/so2/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object CMakeFiles/so2.dir/main.c.o"
	/usr/bin/gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/so2.dir/main.c.o -c /Users/jakubbednarski/Coding/CLionProjects/so2/main.c

CMakeFiles/so2.dir/main.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/so2.dir/main.c.i"
	/usr/bin/gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /Users/jakubbednarski/Coding/CLionProjects/so2/main.c > CMakeFiles/so2.dir/main.c.i

CMakeFiles/so2.dir/main.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/so2.dir/main.c.s"
	/usr/bin/gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /Users/jakubbednarski/Coding/CLionProjects/so2/main.c -o CMakeFiles/so2.dir/main.c.s

# Object files for target so2
so2_OBJECTS = \
"CMakeFiles/so2.dir/main.c.o"

# External object files for target so2
so2_EXTERNAL_OBJECTS =

so2: CMakeFiles/so2.dir/main.c.o
so2: CMakeFiles/so2.dir/build.make
so2: CMakeFiles/so2.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/Users/jakubbednarski/Coding/CLionProjects/so2/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking C executable so2"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/so2.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/so2.dir/build: so2
.PHONY : CMakeFiles/so2.dir/build

CMakeFiles/so2.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/so2.dir/cmake_clean.cmake
.PHONY : CMakeFiles/so2.dir/clean

CMakeFiles/so2.dir/depend:
	cd /Users/jakubbednarski/Coding/CLionProjects/so2/cmake-build-debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /Users/jakubbednarski/Coding/CLionProjects/so2 /Users/jakubbednarski/Coding/CLionProjects/so2 /Users/jakubbednarski/Coding/CLionProjects/so2/cmake-build-debug /Users/jakubbednarski/Coding/CLionProjects/so2/cmake-build-debug /Users/jakubbednarski/Coding/CLionProjects/so2/cmake-build-debug/CMakeFiles/so2.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/so2.dir/depend


# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.22

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
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/dmytro/topd

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/dmytro/topd/build

# Include any dependencies generated for this target.
include CMakeFiles/topd.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/topd.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/topd.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/topd.dir/flags.make

CMakeFiles/topd.dir/main.c.o: CMakeFiles/topd.dir/flags.make
CMakeFiles/topd.dir/main.c.o: ../main.c
CMakeFiles/topd.dir/main.c.o: CMakeFiles/topd.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/dmytro/topd/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object CMakeFiles/topd.dir/main.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT CMakeFiles/topd.dir/main.c.o -MF CMakeFiles/topd.dir/main.c.o.d -o CMakeFiles/topd.dir/main.c.o -c /home/dmytro/topd/main.c

CMakeFiles/topd.dir/main.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/topd.dir/main.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/dmytro/topd/main.c > CMakeFiles/topd.dir/main.c.i

CMakeFiles/topd.dir/main.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/topd.dir/main.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/dmytro/topd/main.c -o CMakeFiles/topd.dir/main.c.s

CMakeFiles/topd.dir/linux.c.o: CMakeFiles/topd.dir/flags.make
CMakeFiles/topd.dir/linux.c.o: ../linux.c
CMakeFiles/topd.dir/linux.c.o: CMakeFiles/topd.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/dmytro/topd/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building C object CMakeFiles/topd.dir/linux.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT CMakeFiles/topd.dir/linux.c.o -MF CMakeFiles/topd.dir/linux.c.o.d -o CMakeFiles/topd.dir/linux.c.o -c /home/dmytro/topd/linux.c

CMakeFiles/topd.dir/linux.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/topd.dir/linux.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/dmytro/topd/linux.c > CMakeFiles/topd.dir/linux.c.i

CMakeFiles/topd.dir/linux.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/topd.dir/linux.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/dmytro/topd/linux.c -o CMakeFiles/topd.dir/linux.c.s

# Object files for target topd
topd_OBJECTS = \
"CMakeFiles/topd.dir/main.c.o" \
"CMakeFiles/topd.dir/linux.c.o"

# External object files for target topd
topd_EXTERNAL_OBJECTS =

bin/topd: CMakeFiles/topd.dir/main.c.o
bin/topd: CMakeFiles/topd.dir/linux.c.o
bin/topd: CMakeFiles/topd.dir/build.make
bin/topd: CMakeFiles/topd.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/dmytro/topd/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Linking C executable bin/topd"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/topd.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/topd.dir/build: bin/topd
.PHONY : CMakeFiles/topd.dir/build

CMakeFiles/topd.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/topd.dir/cmake_clean.cmake
.PHONY : CMakeFiles/topd.dir/clean

CMakeFiles/topd.dir/depend:
	cd /home/dmytro/topd/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/dmytro/topd /home/dmytro/topd /home/dmytro/topd/build /home/dmytro/topd/build /home/dmytro/topd/build/CMakeFiles/topd.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/topd.dir/depend


# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.5

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list


# Suppress display of executed commands.
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
RM = /usr/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/karimae/projects/lernaean-hydra/hydra2/code/flann

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/karimae/projects/lernaean-hydra/hydra2/code/flann

# Utility rule file for tests.

# Include the progress variables for this target.
include test/CMakeFiles/tests.dir/progress.make

tests: test/CMakeFiles/tests.dir/build.make

.PHONY : tests

# Rule to build all files generated by this target.
test/CMakeFiles/tests.dir/build: tests

.PHONY : test/CMakeFiles/tests.dir/build

test/CMakeFiles/tests.dir/clean:
	cd /home/karimae/projects/lernaean-hydra/hydra2/code/flann/test && $(CMAKE_COMMAND) -P CMakeFiles/tests.dir/cmake_clean.cmake
.PHONY : test/CMakeFiles/tests.dir/clean

test/CMakeFiles/tests.dir/depend:
	cd /home/karimae/projects/lernaean-hydra/hydra2/code/flann && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/karimae/projects/lernaean-hydra/hydra2/code/flann /home/karimae/projects/lernaean-hydra/hydra2/code/flann/test /home/karimae/projects/lernaean-hydra/hydra2/code/flann /home/karimae/projects/lernaean-hydra/hydra2/code/flann/test /home/karimae/projects/lernaean-hydra/hydra2/code/flann/test/CMakeFiles/tests.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : test/CMakeFiles/tests.dir/depend


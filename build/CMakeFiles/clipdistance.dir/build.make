# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.8

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
CMAKE_COMMAND = /usr/local/Cellar/cmake/3.8.0/bin/cmake

# The command to remove a file.
RM = /usr/local/Cellar/cmake/3.8.0/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /Users/mikhailkumskov/Documents/qrsach/HairWerxv3

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /Users/mikhailkumskov/Documents/qrsach/HairWerxv3/build

# Include any dependencies generated for this target.
include CMakeFiles/clipdistance.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/clipdistance.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/clipdistance.dir/flags.make

# Object files for target clipdistance
clipdistance_OBJECTS =

# External object files for target clipdistance
clipdistance_EXTERNAL_OBJECTS =

../bin/clipdistance: CMakeFiles/clipdistance.dir/build.make
../bin/clipdistance: lib/libsb7.a
../bin/clipdistance: CMakeFiles/clipdistance.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/Users/mikhailkumskov/Documents/qrsach/HairWerxv3/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Linking CXX executable ../bin/clipdistance"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/clipdistance.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/clipdistance.dir/build: ../bin/clipdistance

.PHONY : CMakeFiles/clipdistance.dir/build

CMakeFiles/clipdistance.dir/requires:

.PHONY : CMakeFiles/clipdistance.dir/requires

CMakeFiles/clipdistance.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/clipdistance.dir/cmake_clean.cmake
.PHONY : CMakeFiles/clipdistance.dir/clean

CMakeFiles/clipdistance.dir/depend:
	cd /Users/mikhailkumskov/Documents/qrsach/HairWerxv3/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /Users/mikhailkumskov/Documents/qrsach/HairWerxv3 /Users/mikhailkumskov/Documents/qrsach/HairWerxv3 /Users/mikhailkumskov/Documents/qrsach/HairWerxv3/build /Users/mikhailkumskov/Documents/qrsach/HairWerxv3/build /Users/mikhailkumskov/Documents/qrsach/HairWerxv3/build/CMakeFiles/clipdistance.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/clipdistance.dir/depend

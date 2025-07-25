# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 4.0

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
CMAKE_COMMAND = /opt/homebrew/bin/cmake

# The command to remove a file.
RM = /opt/homebrew/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /Users/vovkes/BULLETUAV

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /Users/vovkes/BULLETUAV/build

# Include any dependencies generated for this target.
include CMakeFiles/example_opencv.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/example_opencv.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/example_opencv.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/example_opencv.dir/flags.make

CMakeFiles/example_opencv.dir/codegen:
.PHONY : CMakeFiles/example_opencv.dir/codegen

CMakeFiles/example_opencv.dir/example_opencv_usage.cpp.o: CMakeFiles/example_opencv.dir/flags.make
CMakeFiles/example_opencv.dir/example_opencv_usage.cpp.o: /Users/vovkes/BULLETUAV/example_opencv_usage.cpp
CMakeFiles/example_opencv.dir/example_opencv_usage.cpp.o: CMakeFiles/example_opencv.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/Users/vovkes/BULLETUAV/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/example_opencv.dir/example_opencv_usage.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/example_opencv.dir/example_opencv_usage.cpp.o -MF CMakeFiles/example_opencv.dir/example_opencv_usage.cpp.o.d -o CMakeFiles/example_opencv.dir/example_opencv_usage.cpp.o -c /Users/vovkes/BULLETUAV/example_opencv_usage.cpp

CMakeFiles/example_opencv.dir/example_opencv_usage.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/example_opencv.dir/example_opencv_usage.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/vovkes/BULLETUAV/example_opencv_usage.cpp > CMakeFiles/example_opencv.dir/example_opencv_usage.cpp.i

CMakeFiles/example_opencv.dir/example_opencv_usage.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/example_opencv.dir/example_opencv_usage.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/vovkes/BULLETUAV/example_opencv_usage.cpp -o CMakeFiles/example_opencv.dir/example_opencv_usage.cpp.s

# Object files for target example_opencv
example_opencv_OBJECTS = \
"CMakeFiles/example_opencv.dir/example_opencv_usage.cpp.o"

# External object files for target example_opencv
example_opencv_EXTERNAL_OBJECTS =

example_opencv: CMakeFiles/example_opencv.dir/example_opencv_usage.cpp.o
example_opencv: CMakeFiles/example_opencv.dir/build.make
example_opencv: src/sensors/liboptical_tracker.a
example_opencv: src/targeting/libtargeting_core.a
example_opencv: /opt/homebrew/lib/libopencv_gapi.4.12.0.dylib
example_opencv: /opt/homebrew/lib/libopencv_stitching.4.12.0.dylib
example_opencv: /opt/homebrew/lib/libopencv_alphamat.4.12.0.dylib
example_opencv: /opt/homebrew/lib/libopencv_aruco.4.12.0.dylib
example_opencv: /opt/homebrew/lib/libopencv_bgsegm.4.12.0.dylib
example_opencv: /opt/homebrew/lib/libopencv_bioinspired.4.12.0.dylib
example_opencv: /opt/homebrew/lib/libopencv_ccalib.4.12.0.dylib
example_opencv: /opt/homebrew/lib/libopencv_dnn_objdetect.4.12.0.dylib
example_opencv: /opt/homebrew/lib/libopencv_dnn_superres.4.12.0.dylib
example_opencv: /opt/homebrew/lib/libopencv_dpm.4.12.0.dylib
example_opencv: /opt/homebrew/lib/libopencv_face.4.12.0.dylib
example_opencv: /opt/homebrew/lib/libopencv_freetype.4.12.0.dylib
example_opencv: /opt/homebrew/lib/libopencv_fuzzy.4.12.0.dylib
example_opencv: /opt/homebrew/lib/libopencv_hfs.4.12.0.dylib
example_opencv: /opt/homebrew/lib/libopencv_img_hash.4.12.0.dylib
example_opencv: /opt/homebrew/lib/libopencv_intensity_transform.4.12.0.dylib
example_opencv: /opt/homebrew/lib/libopencv_line_descriptor.4.12.0.dylib
example_opencv: /opt/homebrew/lib/libopencv_mcc.4.12.0.dylib
example_opencv: /opt/homebrew/lib/libopencv_quality.4.12.0.dylib
example_opencv: /opt/homebrew/lib/libopencv_rapid.4.12.0.dylib
example_opencv: /opt/homebrew/lib/libopencv_reg.4.12.0.dylib
example_opencv: /opt/homebrew/lib/libopencv_rgbd.4.12.0.dylib
example_opencv: /opt/homebrew/lib/libopencv_saliency.4.12.0.dylib
example_opencv: /opt/homebrew/lib/libopencv_sfm.4.12.0.dylib
example_opencv: /opt/homebrew/lib/libopencv_signal.4.12.0.dylib
example_opencv: /opt/homebrew/lib/libopencv_stereo.4.12.0.dylib
example_opencv: /opt/homebrew/lib/libopencv_structured_light.4.12.0.dylib
example_opencv: /opt/homebrew/lib/libopencv_superres.4.12.0.dylib
example_opencv: /opt/homebrew/lib/libopencv_surface_matching.4.12.0.dylib
example_opencv: /opt/homebrew/lib/libopencv_tracking.4.12.0.dylib
example_opencv: /opt/homebrew/lib/libopencv_videostab.4.12.0.dylib
example_opencv: /opt/homebrew/lib/libopencv_viz.4.12.0.dylib
example_opencv: /opt/homebrew/lib/libopencv_wechat_qrcode.4.12.0.dylib
example_opencv: /opt/homebrew/lib/libopencv_xfeatures2d.4.12.0.dylib
example_opencv: /opt/homebrew/lib/libopencv_xobjdetect.4.12.0.dylib
example_opencv: /opt/homebrew/lib/libopencv_xphoto.4.12.0.dylib
example_opencv: /opt/homebrew/lib/libopencv_phase_unwrapping.4.12.0.dylib
example_opencv: /opt/homebrew/lib/libopencv_optflow.4.12.0.dylib
example_opencv: /opt/homebrew/lib/libopencv_highgui.4.12.0.dylib
example_opencv: /opt/homebrew/lib/libopencv_datasets.4.12.0.dylib
example_opencv: /opt/homebrew/lib/libopencv_plot.4.12.0.dylib
example_opencv: /opt/homebrew/lib/libopencv_text.4.12.0.dylib
example_opencv: /opt/homebrew/lib/libopencv_videoio.4.12.0.dylib
example_opencv: /opt/homebrew/lib/libopencv_ml.4.12.0.dylib
example_opencv: /opt/homebrew/lib/libopencv_shape.4.12.0.dylib
example_opencv: /opt/homebrew/lib/libopencv_ximgproc.4.12.0.dylib
example_opencv: /opt/homebrew/lib/libopencv_video.4.12.0.dylib
example_opencv: /opt/homebrew/lib/libopencv_imgcodecs.4.12.0.dylib
example_opencv: /opt/homebrew/lib/libopencv_objdetect.4.12.0.dylib
example_opencv: /opt/homebrew/lib/libopencv_calib3d.4.12.0.dylib
example_opencv: /opt/homebrew/lib/libopencv_dnn.4.12.0.dylib
example_opencv: /opt/homebrew/lib/libopencv_features2d.4.12.0.dylib
example_opencv: /opt/homebrew/lib/libopencv_flann.4.12.0.dylib
example_opencv: /opt/homebrew/lib/libopencv_photo.4.12.0.dylib
example_opencv: /opt/homebrew/lib/libopencv_imgproc.4.12.0.dylib
example_opencv: /opt/homebrew/lib/libopencv_core.4.12.0.dylib
example_opencv: CMakeFiles/example_opencv.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --bold --progress-dir=/Users/vovkes/BULLETUAV/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable example_opencv"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/example_opencv.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/example_opencv.dir/build: example_opencv
.PHONY : CMakeFiles/example_opencv.dir/build

CMakeFiles/example_opencv.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/example_opencv.dir/cmake_clean.cmake
.PHONY : CMakeFiles/example_opencv.dir/clean

CMakeFiles/example_opencv.dir/depend:
	cd /Users/vovkes/BULLETUAV/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /Users/vovkes/BULLETUAV /Users/vovkes/BULLETUAV /Users/vovkes/BULLETUAV/build /Users/vovkes/BULLETUAV/build /Users/vovkes/BULLETUAV/build/CMakeFiles/example_opencv.dir/DependInfo.cmake "--color=$(COLOR)"
.PHONY : CMakeFiles/example_opencv.dir/depend


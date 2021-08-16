################################################################
# A minimal CMake file that is compatible with sbt-jni         #
#                                                              #
# All settings required by sbt-jni have been marked so, please #
# add/modify/remove settings to build your specific library.   #
################################################################

cmake_minimum_required(VERSION 3.12)

# Define project and related variables
# (required by sbt-jni) please use semantic versioning
#
project (native)
set(PROJECT_VERSION_MAJOR 0)
set(PROJECT_VERSION_MINOR 1)
set(PROJECT_VERSION_PATCH 0)

set (CMAKE_CXX_STANDARD 17)

link_directories($ENV{WORK}/local/lib $ENV{WORK}/local/lib64)
# Setup JNI
find_package(JNI REQUIRED)
if (JNI_FOUND)
    message (STATUS "JNI include directories: ${JNI_INCLUDE_DIRS}")
endif()

find_package(Arrow REQUIRED)
# Include directories
include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/jni/include)
include_directories(${CMAKE_SOURCE_DIR}/arrow/include)
include_directories($ENV{WORK}/local/include)
include_directories(${JNI_INCLUDE_DIRS})
include_directories(proto)

# Sources
file(GLOB LIB_SRC_3  
  "jni/Converters.cpp"
  "jni/JNILoader.cpp"
  "jni/JavaMemoryPool.cpp"
  "jni/JavaResizableBuffer.cpp"
  "jni/ProtobufSchemaDeserializer.cpp"
)
# Sources
file(GLOB LIB_SRC_1
  "arrow/FileReader.cpp"
  "arrow/ColumnScheduler.cpp"
  "arrow/PlatformWrapper.cpp"
  "arrow/DataSetParquetReader.cpp"
  "arrow/FletcherParquetReader.cpp"
  "arrow/JNIFletcherParquetReader.cpp"
)
# Sources
file(GLOB LIB_SRC_2  
  "proto/Types_.pb.cc"
)

# Setup installation targets
# (required by sbt-jni) major version should always be appended to library name
#
#set (LIB_NAME ${PROJECT_NAME}${PROJECT_VERSION_MAJOR})
set (LIB_NAME native)
add_library(${LIB_NAME} SHARED ${LIB_SRC_1} ${LIB_SRC_2} ${LIB_SRC_3})
install(TARGETS ${LIB_NAME} LIBRARY DESTINATION .)
target_link_libraries(${LIB_NAME} gandiva_jni protobuf arrow_dataset arrow fletcher fletcher_snap parquet)

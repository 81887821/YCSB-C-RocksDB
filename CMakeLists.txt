# version of cmake
cmake_minimum_required(VERSION 3.10)

# define the name of project
project(ycsbc LANGUAGES CXX)

#specify c++ compiler version
set(CMAKE_CXX_STANDARD 11)

#compile_commands.json
set(CMAKE_EXPORT_COMPILE_COMMANDS ON) 

find_package(PkgConfig REQUIRED)
pkg_check_modules(ROCKSDB REQUIRED rocksdb)

add_executable(ycsbc "")

target_compile_options(ycsbc PRIVATE -g -Wall -pthread)

target_sources(ycsbc
  PRIVATE
    "ycsbc.cc"
    "core/core_workload.cc"
    "db/db_factory.cc"
    "db/hashtable_db.cc"
    "db/rocksdb_db.cc"
    "utils/histogram.cc"
)

target_include_directories(ycsbc
  PRIVATE
  ${PROJECT_SOURCE_DIR}
  ${ROCKSDB_INCLUDE_DIRS}
)

target_link_directories(ycsbc
  PRIVATE
  ${ROCKSDB_LIBRARY_DIRS}
)


target_link_libraries(ycsbc
  pthread
  tbb
  zstd
  lz4
  bz2
  z
  snappy
  dl
  ${ROCKSDB_STATIC_LIBRARIES}
)

target_link_options(ycsbc
  PRIVATE
  ${ROCKSDB_STATIC_LDFLAGS}
)

add_custom_command(
  TARGET ycsbc
  COMMAND cp ${PROJECT_BINARY_DIR}/compile_commands.json ${CMAKE_SOURCE_DIR}
)

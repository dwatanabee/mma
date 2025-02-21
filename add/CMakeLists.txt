# ###############################################################################

add_executable(mma file_io.cpp)
target_link_libraries(mma PRIVATE mma::mma)
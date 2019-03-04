cmake_policy(SET CMP0048 NEW)

project(aws-rest-proto VERSION 1.0.0 LANGUAGES CXX)
cmake_minimum_required(VERSION 3.10)

find_package(nlohmann_json)
find_package(Boost 1.67 COMPONENTS system)
find_package(kvasir_mpl)
find_package(OpenSSL)
find_package(CryptoPP)
find_package(Threads)
find_package(fmt)

add_library(aws-rest-proto STATIC src/aws_client_request.cpp src/encode.cpp)
add_library(AwsRestProto::aws-rest-proto ALIAS aws-rest-proto)

target_include_directories(aws-rest-proto PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
    )

target_link_libraries(aws-rest-proto PUBLIC CryptoPP::cryptopp OpenSSL::SSL Threads::Threads
    fmt::fmt Boost::system nlohmann_json::nlohmann_json kvasir_mpl span-lite)
install(TARGETS aws-rest-proto EXPORT aws-rest-proto-Targets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
    INCLUDES DESTINATION include
    )
install(EXPORT aws-rest-proto-Targets
    FILE aws-rest-proto.cmake
    NAMESPACE AwsRestProto::
    DESTINATION lib/cmake/
    )

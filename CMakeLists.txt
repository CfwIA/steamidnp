cmake_minimum_required(VERSION 3.0.0)
project(steamidn VERSION 0.1.0 LANGUAGES C CXX)

add_executable(steamidnp source.cpp)

set_property(TARGET steamidnp PROPERTY CXX_STANDARD 23)

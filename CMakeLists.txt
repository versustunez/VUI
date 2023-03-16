cmake_minimum_required(VERSION 3.24)
project(VeNoUILang)

set(CMAKE_CXX_STANDARD 17)

add_executable(VeNoUILang main.cpp
        src/Compiler.cpp
        src/PreInterpreter.cpp
        src/Token.cpp
        src/AstNode.cpp
        src/AstNodeDump.cpp
        src/AstNodeRun.cpp
        )

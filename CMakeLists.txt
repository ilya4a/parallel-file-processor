cmake_minimum_required(VERSION 4.1)
project(ThreadPool)

set(CMAKE_CXX_STANDARD 20)

add_executable(ThreadPool main.cpp
        src/ThreadPool.cpp
        include/ThreadPool.h
        src/App.cpp
        include/App.h
        include/FileCollector.h
        include/Config.h
        src/FileProcessor.cpp
        include/FileProcessor.h
        include/results_structs.h
        include/TextSearcher.h
        src/TextReplacer.cpp
        include/TextReplacer.h
        include/ReplaceOptions.h
)

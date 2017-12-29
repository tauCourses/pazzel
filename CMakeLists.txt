cmake_minimum_required(VERSION 3.8)
project(puzzle)

set( CMAKE_CXX_FLAGS  "-pthread")

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES main.cpp
        PuzzlePiece.cpp
        PuzzlePiece.h
        PuzzleSolver.cpp
        PuzzleSolver.h
        AbstractPieceManager.cpp
        AbstractPieceManager.h
        PuzzlePieceConstrain.h
        CommandLineManager.h
        CommandLineManager.cpp
        PieceManagerFactory.cpp
        PieceManagerFactory.h
        PuzzleParser.cpp
        PuzzleParser.h
        BasicPieceManager.cpp
        BasicPieceManager.h
        RotatablePieceManager.cpp
        RotatablePieceManager.h)

add_executable(puzzle ${SOURCE_FILES} PuzzleParser.cpp PuzzleParser.h)
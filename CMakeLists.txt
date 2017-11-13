cmake_minimum_required(VERSION 3.8)
project(puzzle)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES main.cpp ParsedPuzzle.h ParsedPuzzle.cpp PuzzlePiece.cpp PuzzlePiece.h PuzzleSolver.cpp PuzzleSolver.h PieceManager.cpp PieceManager.h exporter.cpp exporter.h PuzzlePieceConstrain.h)
add_executable(puzzle ${SOURCE_FILES})
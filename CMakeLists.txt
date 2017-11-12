cmake_minimum_required(VERSION 3.8)
project(pazzel)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES main.cpp ParsedPuzzle.h ParsedPuzzle.cpp PuzzlePiece.cpp PuzzlePiece.h PuzzleSolver.cpp PuzzleSolver.h PieceManager.cpp PieceManager.h exporter.cpp exporter.h)
add_executable(pazzel ${SOURCE_FILES})
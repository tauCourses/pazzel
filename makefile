CPP = g++

EXEC = ex2
CPP_COMP_FLAG = -std=gnu++11 -Wall -Wextra \
-Werror -pedantic-errors -DNDEBUG

OBJS =  PuzzlePiece.o \
		ParsedPuzzle.o \
		PieceManager.o \
		PuzzleSolver.o \
		exporter.o \
		main.o

$(EXEC): $(OBJS)
	$(CPP) $(OBJS) -o $@

PuzzlePiece.o: PuzzlePiece.cpp 
	$(CPP) $(CPP_COMP_FLAG) -c $*.cpp
ParsedPuzzle.o: ParsedPuzzle.cpp PuzzlePiece.h
	$(CPP) $(CPP_COMP_FLAG) -c $*.cpp
PieceManager.o: PieceManager.cpp ParsedPuzzle.h PuzzlePiece.h
	$(CPP) $(CPP_COMP_FLAG) -c $*.cpp
PuzzleSolver.o: PuzzleSolver.cpp PieceManager.h PuzzlePieceConstrain.h
	$(CPP) $(CPP_COMP_FLAG) -c $*.cpp
exporter.o: exporter.cpp ParsedPuzzle.h PuzzleSolver.h
	$(CPP) $(CPP_COMP_FLAG) -c $*.cpp
main.o: main.cpp ParsedPuzzle.h PuzzleSolver.h exporter.h 
	$(CPP) $(CPP_COMP_FLAG) -c $*.cpp
clean:
	rm -f $(OBJS) $(EXEC)


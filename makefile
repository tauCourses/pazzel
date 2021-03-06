CPP = g++-5.3.0 -pthread

EXEC = ex3
CPP_COMP_FLAG = -std=c++11 -Wall -Wextra \
-Werror -pedantic-errors -DNDEBUG 

OBJS =  PuzzlePiece.o \
		PuzzleException.o \
		CommandLineManager.o \
		AbstractPieceManager.o \
		BasicPieceManager.o \
		RotatablePieceManager.o \
		PieceManagerFactory.o \
		PuzzleParser.o \
		SolverThread.o \
		PuzzleSolver.o \
		main.o

$(EXEC): $(OBJS)
	$(CPP) $(OBJS) -o $@

PuzzlePiece.o: PuzzlePiece.cpp 
	$(CPP) $(CPP_COMP_FLAG) -c $*.cpp
PuzzleException.o: PuzzleException.cpp
	$(CPP) $(CPP_COMP_FLAG) -c $*.cpp
CommandLineManager.o: CommandLineManager.cpp 
	$(CPP) $(CPP_COMP_FLAG) -c $*.cpp
AbstractPieceManager.o: AbstractPieceManager.cpp PuzzlePiece.h PuzzleException.h
	$(CPP) $(CPP_COMP_FLAG) -c $*.cpp
BasicPieceManager.o: BasicPieceManager.cpp AbstractPieceManager.h 
	$(CPP) $(CPP_COMP_FLAG) -c $*.cpp
RotatablePieceManager.o: RotatablePieceManager.cpp AbstractPieceManager.h
	$(CPP) $(CPP_COMP_FLAG) -c $*.cpp
PieceManagerFactory.o: PieceManagerFactory.cpp AbstractPieceManager.h RotatablePieceManager.h BasicPieceManager.h
	$(CPP) $(CPP_COMP_FLAG) -c $*.cpp
PuzzleParser.o: PuzzleParser.cpp AbstractPieceManager.h PuzzlePiece.h PuzzleException.h
	$(CPP) $(CPP_COMP_FLAG) -c $*.cpp
SolverThread.o: SolverThread.cpp AbstractPieceManager.h PuzzlePieceConstrain.h
	$(CPP) $(CPP_COMP_FLAG) -c $*.cpp 
PuzzleSolver.o: PuzzleSolver.cpp SolverThread.h 
	$(CPP) $(CPP_COMP_FLAG) -c $*.cpp 
main.o: main.cpp CommandLineManager.h PieceManagerFactory.h PuzzleParser.h
	$(CPP) $(CPP_COMP_FLAG) -c $*.cpp
clean:
	rm -f $(OBJS) $(EXEC)


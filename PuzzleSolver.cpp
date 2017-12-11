#include "PuzzleSolver.h"

namespace {
	bool hasWrongNumberOfStraightLines(AbstractPieceManager &pm, int numberOfRows, int numberOfCols) {
		bool ret = false;
		ret |= pm.countConstrainPiece(hasLeftStraight) < numberOfRows;
		ret |= pm.countConstrainPiece(hasUpperStraight) < numberOfCols;
		ret |= pm.countConstrainPiece(hasRightStraight) < numberOfRows;
		ret |= pm.countConstrainPiece(hasDownStraight) < numberOfCols;
		return ret;
	}

	void hasWrongNumberOfStraightLines(SolverErrors &se, AbstractPieceManager &pm, int numberOfPieces) {
		bool wrongNumberOfStraightLines = true; // to be checked
		for (int i = 1; i * i <= numberOfPieces; ++i) {
			if (numberOfPieces % i == 0) {
				int row = i, col = numberOfPieces / i;
				// do one time if row == col else do 2 times row X col, col X row
				for (int j = 0; j < ((row == col) ? 1 : 2); ++j) {
					wrongNumberOfStraightLines &= hasWrongNumberOfStraightLines(pm, row, col);

					row = col, col = i;
				}
			}
		}
		se.wrongNumberOfStraightLine = wrongNumberOfStraightLines;
	}

	void hasASumOfZero(SolverErrors &se, AbstractPieceManager &pm) {
		se.sumOfEdgesIsNotZero =
			((pm.countConstrainPiece(hasLeftMale) - pm.countConstrainPiece(hasRightFemale)) != 0) ||
			((pm.countConstrainPiece(hasLeftFemale) - pm.countConstrainPiece(hasRightMale)) != 0) ||
			((pm.countConstrainPiece(hasUpperMale) - pm.countConstrainPiece(hasDownFemale)) != 0) ||
			((pm.countConstrainPiece(hasUpperFemale) - pm.countConstrainPiece(hasDownMale)) != 0);
	}

	void hasAllCorners(SolverErrors &se, AbstractPieceManager &pm) {
		se.missingTL = pm.countConstrainPiece(hasUpperStraight & hasLeftStraight) == 0;
		se.missingTR = pm.countConstrainPiece(hasUpperStraight & hasRightStraight) == 0;
		se.missingBL = pm.countConstrainPiece(hasDownStraight & hasLeftStraight) == 0;
		se.missingBR = pm.countConstrainPiece(hasDownStraight & hasRightStraight) == 0;
	}

}

PuzzleSolver::PuzzleSolver(AbstractPieceManager &pieceManager) : pieceManager(pieceManager){}

bool PuzzleSolver::trySolve() {
    auto allPossibleSize = getAllPosibleSizes();
    for(auto size:allPossibleSize)
        if(trySolveForShape(shape))
            return true;
    return false;
}

PuzzleSolver::~PuzzleSolver() {
	delete puzzleSolution;
}

bool SolverErrors::hasError() {
	return wrongNumberOfStraightLine || missingTL || missingTR || missingBL || missingBR || sumOfEdgesIsNotZero ||
		couldNotSolvePuzzle;
}

PuzzleSolution::PuzzleSolution(int row, int col) : row(row), col(col) {
	puzzleSolution = new Piece_t[row * col];
	std::fill_n(puzzleSolution, row * col, nullPiece);
}

PuzzleSolution::~PuzzleSolution() {
	delete[] puzzleSolution;
}


Piece_t inline PuzzleSolution::get(int _row, int _col) {
	return puzzleSolution[_row * col + _col];
}

void inline PuzzleSolution::set(int _row, int _col, Piece_t newVal) {
	puzzleSolution[_row * col + _col] = newVal;
}

namespace {
	typedef struct PuzzlePieceLocation_t {
		int rowNumber, colNumber;

		inline bool operator==(const PuzzlePieceLocation_t &r) {
			return rowNumber == r.rowNumber && colNumber == r.colNumber;
		}
	} PuzzlePieceLocation;

	PuzzlePieceLocation nullLocation = { -1, -1 };

	inline Piece_t getConstrainOpposite(Piece_t currentConstrain) {
		return static_cast<uint8_t>(
			(((currentConstrain & 0x1) == (uint8_t)(currentConstrain >> 1)) << 1) |
			(currentConstrain & 0x1));
	}

	PuzzlePieceLocation nextLocationToCheck(AbstractPieceManager &pm, Piece_t *constrains, PuzzleSolution *puzzleSolution) {
		int minOption = 1 << 30;
		PuzzlePieceLocation bestLocation, currentLocation;
		int row = puzzleSolution->row, col = puzzleSolution->col;
		for (currentLocation.rowNumber = 0; currentLocation.rowNumber < row; ++currentLocation.rowNumber) {
			for (currentLocation.colNumber = 0; currentLocation.colNumber < col; ++currentLocation.colNumber) {
				if (nullPiece == puzzleSolution->get(currentLocation.rowNumber, currentLocation.colNumber)) {
					int currentConstrainCount = pm.countConstrainOptions(
						constrains[currentLocation.rowNumber * col + currentLocation.colNumber]);
					if (currentConstrainCount < minOption) {
						if (currentConstrainCount == 0) {
							return nullLocation; // no available piece for a location, should go back.
						}
						else {
							minOption = currentConstrainCount;
							bestLocation = currentLocation;
						}
					}
				}
			}
		}
		return bestLocation;
	}

	void setConstrainMatrix(Piece_t* constrains, int row, int col) {
		for (int i = 0; i < col; i++) {
			constrains[i] &= hasUpperStraight;
			constrains[(row - 1)*col + i] &= hasDownStraight;
		}
		for (int i = 0; i < row; i++) {
			constrains[i*col] &= hasLeftStraight;
			constrains[i*col + col - 1] &= hasRightStraight;
		}
	}

	inline void addConstrain(Piece_t *constrains, Piece_t newVal, int row, int col, int currentRow, int currentCol) {
		if (currentCol > 0) {
			constrains[currentRow * col + currentCol - 1] &=
					(getConstrainOpposite(static_cast<uint8_t>((newVal >> 6) & 0x3)) << 2) | ((0x3 << 2) ^ nullPiece);
		}
		if (currentRow > 0) {
			constrains[(currentRow - 1) * col + currentCol] &=
					(getConstrainOpposite(static_cast<uint8_t>((newVal >> 4) & 0x3)) << 0) | ((0x3 << 0) ^ nullPiece);
		}
		if (currentCol < col - 1) {
			constrains[currentRow * col + currentCol + 1] &=
					(getConstrainOpposite(static_cast<uint8_t>((newVal >> 2) & 0x3)) << 6) | ((0x3 << 6) ^ nullPiece);
		}
		if (currentRow < row - 1) {
			constrains[(currentRow + 1) * col + currentCol] &=
					(getConstrainOpposite(static_cast<uint8_t>((newVal >> 0) & 0x3)) << 4) | ((0x3 << 4) ^ nullPiece);
		}
	}

	inline void removeConstrain(Piece_t *constrains, int row, int col, int currentRow, int currentCol) {
		if (currentCol > 0) {
			constrains[currentRow * col + currentCol - 1] |= 0x3 << 2;
		}
		if (currentRow > 0) {
			constrains[(currentRow - 1) * col + currentCol] |= 0x3 << 0;
		}
		if (currentCol < col - 1) {
			constrains[currentRow * col + currentCol + 1] |= 0x3 << 6;
		}
		if (currentRow < row - 1) {
			constrains[(currentRow + 1) * col + currentCol] |= 0x3 << 4;
		}
	}
}

bool PuzzleSolver::solvePuzzle(int row, int col) {
	int queueLength = row * col;
	auto *queue = new PuzzlePieceLocation[queueLength];
	int queueCurrentSize = 0;
	delete this->puzzleSolution;
	this->puzzleSolution = new PuzzleSolution(row, col);
	bool shouldCheckNewLocation = true;
	PuzzlePieceLocation pieceLocation{};
	auto * constrains = new Piece_t[row*col];
	for (int i = 0; i < row*col; ++i) constrains[i] = nullPiece;
	setConstrainMatrix(constrains, row, col);
	do {
		Piece_t lastValue = nullPiece;
		if (shouldCheckNewLocation) {
			pieceLocation = nextLocationToCheck(pieceManager, constrains, this->puzzleSolution);
			if (pieceLocation == nullLocation) {
				shouldCheckNewLocation = false;
				continue; // last move screwed up the solution.
			}
		}
		else {
			pieceLocation = queue[--queueCurrentSize];
			removeConstrain(constrains, row, col, pieceLocation.rowNumber, pieceLocation.colNumber);
			lastValue = this->puzzleSolution->get(pieceLocation.rowNumber, pieceLocation.colNumber);
		}
		Piece_t constraints = constrains[pieceLocation.rowNumber * col + pieceLocation.colNumber];
		Piece_t newValue = pieceManager.getNextPiece(constraints, lastValue);
		this->puzzleSolution->set(pieceLocation.rowNumber, pieceLocation.colNumber, newValue);
		addConstrain(constrains, newValue, row, col, pieceLocation.rowNumber, pieceLocation.colNumber);

		if (newValue == nullPiece) {
			shouldCheckNewLocation = false;
		}
		else {
			shouldCheckNewLocation = true;
			queue[queueCurrentSize++] = pieceLocation;
		}
	} while (queueCurrentSize != 0 && queueCurrentSize < queueLength);

	delete[] queue;
	delete[] constrains;
	return queueCurrentSize == queueLength;
}



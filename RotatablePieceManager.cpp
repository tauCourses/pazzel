#include "RotatablePieceManager.h"
#include "BasicPieceManager.h"

Piece_t RotatablePieceManager::lookupTable[numberOfConstrains];

vector<AbstractPieceManager::Shape> RotatablePieceManager::getAllPossiblePuzzleShapes() const {

    vector<AbstractPieceManager::Shape> shapes;
    auto numberOfPieces = static_cast<int>(this->pieces.size());
    for (int i = 1; i * i <= numberOfPieces; i++) {
        if (numberOfPieces % i != 0)
            continue;
        AbstractPieceManager::Shape s;
        s.width = i;
        s.height = (numberOfPieces / i);
        if (this->isPuzzleShapePossible(s))
            shapes.emplace_back(s);
    }
    return shapes;
}

void RotatablePieceManager::addPiece(PieceRepository &pieceRepository, unique_ptr<PuzzlePiece> piece) {
    this->pieces.emplace_back(piece.get());
    this->addPieceToRepository(pieceRepository, piece->representor());
}

bool RotatablePieceManager::hasAllCorners(const PieceRepository &pieceRepository) const {
    (void) pieceRepository;
    if (this->pieces.size() == 1) {
        return this->pieces[0].up == 0 || this->pieces[0].down == 0 ||
               this->pieces[0].left == 0 || this->pieces[0].right == 0;
    }
    return this->numberOfCorners() >= 4 || this->hasTwoSideForARaw();
}

int RotatablePieceManager::numberOfCorners() const {
    int corners = 0;
    for (auto piece : this->pieces)
        if ((piece.up == 0 || piece.down == 0) && (piece.left == 0 || piece.right == 0))
            ++corners;
    return corners;
}

bool RotatablePieceManager::hasTwoSideForARaw() const {
    int numOfRawSide = 0;
    for (auto piece : this->pieces) {
        int strightSides = 0;
        if (piece.down == 0)
            strightSides++;
        if (piece.up == 0)
            strightSides++;
        if (piece.right == 0)
            strightSides++;
        if (piece.left == 0)
            strightSides++;
        if (strightSides >= 3)
            numOfRawSide++;
    }
    return numOfRawSide >= 2;
}

void RotatablePieceManager::printMissingCorners(const PieceRepository &pieceRepository, ofstream &fout) const {
    (void) pieceRepository;
    if (this->pieces.size() == 1) {
        fout << "Piece is not square" << endl;
        return;
    }
    if (this->numberOfCorners() >= 4 || this->hasTwoSideForARaw())
        return;

    fout << "There are not enough corners" << endl;
}

RotatablePieceManager::RotatablePieceManager() {
    this->initialLookupTable();
}

void RotatablePieceManager::initialLookupTable() const {
    for (int l = 0; l <= nullPiece; l++) //for each piece_t
    {
        for (int t = 0;; t++) //look for the first permutation
        {
            if (this->isPermutation(static_cast<Piece_t>(l), static_cast<Piece_t>(t))) {
                RotatablePieceManager::lookupTable[l] = static_cast<Piece_t>(t); //set as representor
                break;
            }
        }
    }
}

bool RotatablePieceManager::isPermutation(Piece_t p1, Piece_t p2) const {
    for (int i = 0; i < 4; i++) {
        if (p1 == p2)
            return true;
        p1 = rotatePieceCounterClockWise(p1); //roll one left
    }
    return false;
}

int RotatablePieceManager::getPermutationDegree(Piece_t current, Piece_t origin) const {
    for (int i = 0; i < 4; i++) {
        if (current == origin)
            return i * 90;
        current = rotatePieceCounterClockWise(current); //roll one left
    }
    cerr << "Error: Couldn't get permutation degree";
    return 0;
}

void RotatablePieceManager::removePieceFromRepository(PieceRepository &pieceRepository, Piece_t piece) const {
    if (--pieceRepository.pieceRepository[lookupTable[piece]] == 0)
        changeConstrainsCount(pieceRepository, piece, -1);

}

void RotatablePieceManager::addPieceToRepository(PieceRepository &pieceRepository, Piece_t piece) const {
    if (pieceRepository.pieceRepository[lookupTable[piece]]++ == 0)
        changeConstrainsCount(pieceRepository, piece, +1);
}

int RotatablePieceManager::numOfOptionsForConstrain(const PieceRepository &pieceRepository, Piece_t constrain) const {
    return pieceRepository.constrainRepository[constrain];
}

void RotatablePieceManager::changeConstrainsCount(PieceRepository &pieceRepository, Piece_t piece, int delta) const {
    Piece_t p1 = piece;
    for (Piece_t maskOption : maskOptions)
        pieceRepository.constrainRepository[p1 | maskOption] += delta;

    Piece_t p2 = rotatePieceCounterClockWise(p1);
    if (p1 == p2)
        return; // piece completely symmetric

    // else - add it:
    for (Piece_t maskOption : maskOptions)
        pieceRepository.constrainRepository[p2 | maskOption] += delta;

    Piece_t p3 = rotatePieceCounterClockWise(p2);
    if (p1 == p3)
        return; // piece is symmetric to 180 degree turns

    Piece_t p4 = rotatePieceCounterClockWise(p3);
    for (Piece_t maskOption : maskOptions) {
        pieceRepository.constrainRepository[p3 | maskOption] += delta;
        pieceRepository.constrainRepository[p4 | maskOption] += delta;
    }
}

bool RotatablePieceManager::isPuzzleShapePossible(AbstractPieceManager::Shape shape) const {
    if (shape.width == 1) {
        auto straightTest = [](PuzzlePiece c) { return (c.up == 0 && c.down == 0) || (c.right == 0 && c.left == 0); };
        return std::all_of(this->pieces.begin(), this->pieces.end(), straightTest);
    }
    int numberOfStraightNeeded = shape.height * 2 + shape.width * 2 - 4;
    for (auto piece : this->pieces) {
        if (piece.left == 0 || piece.right == 0 ||
            piece.down == 0 || piece.up == 0) {
            if (--numberOfStraightNeeded == 0)
                return true;
        }
    }
    return false;
}

void RotatablePieceManager::printPiece(Piece_t piece, ofstream &out) {
    for (auto it = pieces.begin(); it != pieces.end(); ++it) {
        if (isPermutation(it->representor(), piece) && !it->wasUsedInSolution) {
            it->wasUsedInSolution = true;
            out << it->index;
            int degree = getPermutationDegree(piece, it->representor());
            if (degree != 0)
                out << " [" << degree << "]";
            return;
        }
    }
    cerr << "Error: couldn't find piece: " << int(piece) << endl;
}

inline bool RotatablePieceManager::pieceExistInRepository(const PieceRepository &pieceRepository, Piece_t piece) const {
    return pieceRepository.constrainRepository[RotatablePieceManager::lookupTable[piece]] > 0;
}

inline Piece_t RotatablePieceManager::rotatePieceCounterClockWise(Piece_t piece) const {
    return (piece << 2) | (piece >> 6); //roll one left
}
//
// Created by private on 12/12/17.
//

#include <algorithm>
#include "RotatablePieceManager.h"

vector<AbstractPieceManager::Shape> RotatablePieceManager::getAllPossiblePuzzleShapes() {

    vector<AbstractPieceManager::Shape> shapes;
    for(int i=1; i*i<=this->pieces.size();i++)
    {
        if(this->pieces.size()%i != 0)
            continue;
        Shape s = {.width=i, .height=static_cast<int>(this->pieces.size() / i)};
        if(this->isPuzzleShapePossible(s))
            shapes.emplace_back(s);
    }
    return shapes;
}

void RotatablePieceManager::addPiece(unique_ptr<PuzzlePiece> piece) {
    this->pieces.emplace_back(piece.get());
    this->addPiece(piece->representor());
}

bool RotatablePieceManager::hasASumOfZero() {
    int sum = 0;
    for(auto piece : this->pieces)
        sum += piece.right + piece.left + piece.down + piece.up;

    return sum == 0;
}

bool RotatablePieceManager::hasAllCorners() {
    return this->numberOfCorners() >= 4;
}

int RotatablePieceManager::numberOfCorners() {
    int corners = 0;
    for(auto piece : this->pieces)
        if((piece.up == 0 || piece.down == 0) && (piece.left == 0 || piece.right == 0))
            corners++;
    return corners;
}

void RotatablePieceManager::printMissingCorners(ofstream &fout) {
    auto corners = this->numberOfCorners();
    if(corners >= 4)
        return;

    fout << "There are only " << corners << " corners" << endl;
}

RotatablePieceManager::RotatablePieceManager() {
    this->initialLookupTable();
    this->initialNextPieceTable();
    this->initialMaskOptionTable();
}

void RotatablePieceManager::initialLookupTable() {
    for(Piece_t l=0; l<256;l++) //for each piece_t
    {
        for(Piece_t t=0; t<256;t++) //look for the first permutation
        {
            if(this->isPermutation(l,t))
            {
                this->lookupTable[l]=t; //set as representor
                break;
            }
        }
    }
}

void RotatablePieceManager::initialNextPieceTable()
{
    for(Piece_t constrain=0; constrain < nullPiece; constrain++)
    {
        auto last = nullPiece;
        for(Piece_t next = 0;next<nullPiece;next++)
        {
            if(!PuzzlePiece::isValidPiece(next))
                continue;
            if(PuzzlePiece::isPieceRespectConstrain(next, constrain))
            {
                this->nextPieceWithConstrain[constrain][last] = next;
                last = next;
            }
        }
        this->nextPieceWithConstrain[constrain][last] = nullPiece;
    }
}

void RotatablePieceManager::initialMaskOptionTable() {
    Piece_t maskPiece;
    for (int mask = 0; mask < (1 << 4); ++mask) {
        maskPiece = 0;
        if ((mask & (1 << 3)) == 0)
            maskPiece |= (0x3 << 6);
        if ((mask & (1 << 2)) == 0)
            maskPiece |= (0x3 << 4);
        if ((mask & (1 << 1)) == 0)
            maskPiece |= (0x3 << 2);
        if ((mask & (1 << 0)) == 0)
            maskPiece |= (0x3 << 0);
        this->maskOptions[mask] = maskPiece;
    }
}

bool RotatablePieceManager::isPermutation(Piece_t p1, Piece_t p2) {
    for(int i=0; i<4;i++)
    {
        if(p1 == p2)
            return true;
        p1 = p1<<2 & p1>>6; //roll one left
    }
    return false;
}

Piece_t RotatablePieceManager::getNextPiece(Piece_t constrain, Piece_t last) {
    if(last != nullPiece)
        addPiece(last);

    while(true)
    {
        Piece_t next = this->nextPieceWithConstrain[constrain][last];
        if(next == nullPiece)
            return nullPiece;
        if(this->pieceRepository[lookupTable[next]] > 0)
        {
            removePiece(next);
            return next;
        }
    }
}

void RotatablePieceManager::removePiece(Piece_t piece) {
    if(--this->pieceRepository[lookupTable[piece]] == 0)
        this->removeFromConstrainRepository(piece);

}

void RotatablePieceManager::addPiece(Piece_t piece) {
    if(++this->pieceRepository[lookupTable[piece]] == 1)
        this->addToConstrainRepository(piece);
}

int RotatablePieceManager::numOfOptionsForConstrain(Piece_t constrain) {
    return this->constrainRepository[constrain];
}

void RotatablePieceManager::removeFromConstrainRepository(Piece_t piece) {
    Piece_t p1 = piece;
    Piece_t p2 = p1<<2 & p1>>6;

    for (Piece_t maskOption : this->maskOptions)
        --this->constrainRepository[p1 | maskOption];
    if(p1==p2)
        return;

    for (Piece_t maskOption : this->maskOptions)
        --this->constrainRepository[p2 | maskOption];
    Piece_t p3 = p2<<2 & p2>>6;
    if(p1==p3)
        return;

    Piece_t p4 = p3<<2 & p3>>6;
    for (Piece_t maskOption : this->maskOptions)
        --this->constrainRepository[p3 | maskOption];
    for (Piece_t maskOption : this->maskOptions)
        --this->constrainRepository[p4 | maskOption];
}

void RotatablePieceManager::addToConstrainRepository(Piece_t piece) {
    Piece_t p1 = piece;
    Piece_t p2 = p1<<2 & p1>>6;

    for (Piece_t maskOption : this->maskOptions)
        ++this->constrainRepository[p1 | maskOption];
    if(p1==p2)
        return;

    for (Piece_t maskOption : this->maskOptions)
        ++this->constrainRepository[p2 | maskOption];
    Piece_t p3 = p2<<2 & p2>>6;
    if(p1==p3)
        return;

    Piece_t p4 = p3<<2 & p3>>6;
    for (Piece_t maskOption : this->maskOptions)
        ++this->constrainRepository[p3 | maskOption];
    for (Piece_t maskOption : this->maskOptions)
        ++this->constrainRepository[p4 | maskOption];
}

bool RotatablePieceManager::isPuzzleShapePossible(AbstractPieceManager::Shape shape) {
    if(shape.width == 1)
    {
        auto straightTest = [](PuzzlePiece c) { return (c.up == 0 && c.down == 0) || (c.right == 0 && c.left == 0);};
        return std::all_of(this->pieces.begin(), this->pieces.end(), straightTest);
    }
    int numberOfStraightNeeded = shape.height*2 + shape.width*2;
    for(auto piece : this->pieces)
    {
        if(piece.left == 0 || piece.right == 0 || piece.down == 0 || piece.up == 0)
        {
            if(--numberOfStraightNeeded == 0)
                return true;
        }
    }
    return false;
}




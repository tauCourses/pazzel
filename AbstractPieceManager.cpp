#include "AbstractPieceManager.h"





int AbstractPieceManager::checkPieceIdExistOnce(int id) const{
	int occurrences = 0;
	for(auto piece : this->pieces)
		if(piece.index == id)
			occurrences++;
	
	return occurrences;
}

bool AbstractPieceManager::hasErrors() {
	this->noPossibleShape = this->getAllPossiblePuzzleShapes().empty();
	this->pieceSumNotZero = this->hasASumOfZero();

	return this->noPossibleShape ||
			pieceSumNotZero ||
			!this->hasAllCorners();
}

void AbstractPieceManager::exportErrors(ofstream& fout) {
	this->printNoPissibleShape(fout);
	this->printMissingCorners(fout);
	this->printSumNotZero(fout);
}

void AbstractPieceManager::printNoPissibleShape(ofstream &fout) {
	fout << "Cannot solve puzzle: wrong number of straight edges" << endl;
}

void AbstractPieceManager::printSumNotZero(ofstream &fout) {
	fout << "Cannot solve puzzle: sum of edges is not zero" << endl;
}



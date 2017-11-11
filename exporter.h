//
// Created by private on 11/11/17.
//

#ifndef PAZZEL_EXPORTER_H
#define PAZZEL_EXPORTER_H

#include "parsedData.h"

void exportDataErrors(char* outputFile, parsedData data);
void exportSolution(char* outputFile, parsedData data, Sulotion s);
void exportNoSulotion(char* outputFile);


#endif //PAZZEL_EXPORTER_H

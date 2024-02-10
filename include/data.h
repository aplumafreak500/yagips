#ifndef TSV_H
#define TSV_H
#include "stdio.h"
#include <vector>
#include <unordered_map>
#include <string>

typedef std::unordered_map<std::string, std::string> excelTableEnt;

const std::vector<excelTableEnt>* loadJsonExcelData(FILE* const);
const std::vector<excelTableEnt>* loadTsvExcelData(const std::vector<std::string>&, FILE* const);
#endif

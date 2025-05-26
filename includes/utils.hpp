#ifndef UTILS_HPP
#define UTILS_HPP

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <stdexcept>
#include <sstream>

void trim(std::string& str);
std::vector<std::string> split(const std::string& str, char delimiter);
std::string extractValue(const std::string& line, const std::string& directive);
bool containsWord(const std::string& str, const std::string& word);

#endif
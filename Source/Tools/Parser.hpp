/**
* @headerfile Parser.hpp
*
*  CSV Parser, part of our own SWE1D tools
*/

#pragma once

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include "RealType.hpp"

namespace Tools {

 class Parser {
 public:
   enum LogLevel { INFO, ERROR };

 private:
   /** Input file stream for the CSV file */
   std::ifstream inputFile_;

 private:
   Parser();

 public:
   static Parser parser;

   ~Parser() = default;

   void openFile(const std::string& filePath);
   void closeFile();

   /**
        * Parses the CSV file line by line.
        * Each line must have 5 values.
        * returns the values of n files
        * skips the first s values
    */
   std::vector<std::vector<RealType>> parseFile(int n, int s);

 private:
   void printValues(const std::vector<double>& values);
   void log(const std::string& message, LogLevel level);
 };

} // namespace Tools

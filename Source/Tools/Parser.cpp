/**
 * @file Parser.cpp
 */
#include "Parser.hpp"

#include <sstream>
#include <iostream>

namespace Tools {

  Parser Parser::parser;

  Parser::Parser() {}

  void Parser::openFile(const std::string& filePath) {
    inputFile_.open(filePath);
    if (!inputFile_.is_open()) {
      log("Failed to open the file: " + filePath, ERROR);
      exit(1);
    } else {
      log("File successfully opened: " + filePath, INFO);
    }
  }

  void Parser::closeFile() {
    if (inputFile_.is_open()) {
      inputFile_.close();
      log("File successfully closed.", INFO);
    }
  }

  std::vector<std::vector<RealType>>  Parser::parseFile(int n, int s) {
    if (!inputFile_.is_open()) {
      log("No file is open for parsing.", ERROR);
      return {};
    }

    std::string line;
    int counter = 0;
    int skippedLines = 0;
    std::vector<std::vector<RealType>> parsedData;

    // Skip the first 's' lines
    while (skippedLines < s && std::getline(inputFile_, line)) {
      skippedLines++;
    }

    // Read the next 'n' lines
    while (counter < n && std::getline(inputFile_, line)) {
      std::stringstream ss(line);
      std::vector<double> values;
      double number;

      // Parse each line and convert the values to double
      while (ss >> number) {
        if (ss.peek() == ',') {
          ss.ignore();
        }
        values.push_back(number);
      }

      // Check if we have exactly 5 values
      if (values.size() == 5) {
        parsedData.push_back(values);
        counter++;
      } else {
        log("Invalid data format in line: " + line, ERROR);
      }

      return parsedData;

    }

    // print the parsed data
    for (const auto& values : parsedData) {
      printValues(values);
    }
    return {};
  }

  void Parser::printValues(const std::vector<double>& values) {
    // Example processing - you can replace this with your desired function logic
    std::cout << "Processed values: "
              << values[0] << ", "
              << values[1] << ", "
              << values[2] << ", "
              << values[3] << ", "
              << values[4] << std::endl;
  }

  void Parser::log(const std::string& message, LogLevel level) {
    switch (level) {
    case INFO:
      std::cout << "INFO: " << message << std::endl;
      break;
    case ERROR:
      std::cerr << "ERROR: " << message << std::endl;
      break;
    }
  }

} // namespace Tools

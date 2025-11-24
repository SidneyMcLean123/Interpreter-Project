#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <exception>
#include <sstream>
#include <algorithm>

class Tokenizer {
    private:
    int lineCount;
    int currentIndex;
    std::vector<std::string> currentLine;
    std::ifstream inputFile;

    const std::vector<std::string> reserved = {
        "program", "begin", "end", "int",
        "if", "then", "else", "while",
        "loop", "read", "write",
        ";", ",", "=", "!", "[", "]",
        "&&", "||", "(", ")", "+", "-",
        "*", "!=", "==", "<", ">", "<=",">="
    };

    const int INVALIDTOKEN = 0;
    const int INTEGER = 31;
    const int ID = 32;
    const int eOF = 33;

    // Used for determining if token has symbol wedeged inside
    const std::string symbolPrimers = ";,!=[]&|()+-*<>";
    
    void readLine() {
        // prevent tokens from accumulating across lines
        currentLine.clear();
        // Grab the next non-empty line
        std::string line;
        while(std::getline(inputFile, line)) {
            lineCount++;
            if (line.empty()) continue;
            break;
        }
        if (!line.empty()) {
            // Split line into tokens
            // little lambda action
            // remove leading/training whitespaces
            auto removeLeadingTrailing = [](std::string& str) {
                str.erase(0, str.find_first_not_of(" \n\t\r"));
                str.erase(str.find_last_not_of(" \n\t\r") + 1);
            };

            removeLeadingTrailing(line);
            // split on the whitespaces
            std::stringstream ss(line);
            std::string token;

            while (ss >> token) {
                currentLine.push_back(token);
            }

            // separate symbols and reserved keywords that are connected together
            for (int i = 0; i < currentLine.size(); i++) {
                // skip if token is already perfect reserved word or symbol
                if (std::find(reserved.begin(), reserved.end(), currentLine.at(i)) == reserved.end()) {
                    // determine if token contains char from symbols
                    for (char c: symbolPrimers) {
                        if (currentLine[i].find(c) != std::string::npos){
                            separate(i);
                            break;
                        }
                    }
                }
            }
            // // Debugging purposes *************************
            // for (auto t: currentLine) {
            //     std::cout << t << "\n";
            // }
        } else {
            currentLine.clear();
        }
        currentIndex = 0;            
    }

    // separate tokens 
    void separate(int i) {
        std::string token = currentLine.at(i);
        std::vector<std::string> separatedTokens;
        int current = 0;
        int start = 0;

        while (current < token.length()) {
            // check for start of new symbol
            if (symbolPrimers.find(token.substr(current, 1)) != std::string::npos) {
                // add token up till this point
                if (current > 0 && (current - start) > 0) {
                    separatedTokens.push_back(token.substr(start, current - start));
                    // update current for next token
                    start = current;
                } 
                // Greedy check 
                if (current + 1 < token.length() && std::count(reserved.begin(), reserved.end(), token.substr(current, 2))) {
                    separatedTokens.push_back(token.substr(current, 2));
                    start += 2;
                // greedy check fails, try for length of 1
                } else if (std::count(reserved.begin(), reserved.end(), token.substr(current, 1))) {
                    separatedTokens.push_back(token.substr(current, 1));
                    start++;
                } else {
                    throw std::runtime_error("Invalid symbol at line: " + std::to_string(lineCount));
                }
                current = start;
            } else current++;
        } 
        if (start != current) {
            separatedTokens.push_back(token.substr(start, current - start));
        } 

        // erase the current token with the vector of broken up tokens
        auto it_pos = currentLine.begin() + i;
        auto next_it = currentLine.erase(it_pos);
        currentLine.insert(next_it, separatedTokens.begin(), separatedTokens.end());
    }

    bool isValidInt() {
        for (char c: currentLine.at(currentIndex)){
            if (!std::isdigit(c)) return false;
        }
        return true;
    }

    // Valid Id is first char is uppercase, followed by zero or more uppercase letters or digits
    bool isValidID() {
        if (!std::isupper(currentLine.at(currentIndex).at(0))) return false;
        for (char c: currentLine.at(currentIndex)) {
            if (!std::isupper(c) && !std::isdigit(c)) return false;
        }
        return true;
    }

    public:
    Tokenizer(std::string fname) {
        // Attempt to open file
        try {
            inputFile.open(fname);
            if (!inputFile.is_open()) throw std::runtime_error("Couldn't open file :(");
        } catch (const std::exception& e) {
            std::cout << e.what();
        }


        lineCount = 0;
        readLine();
    }

    int getToken() {
        // token is EOF
        if (currentLine.empty() && inputFile.eof()) {
            return eOF;
        }
        int value;
        auto it = std::find(reserved.begin(), reserved.end(), currentLine.at(currentIndex));
        if (it != reserved.end()) {
            // assign token number based on index + 1
            value = std::distance(reserved.begin(), it) + 1;
        } else if (isValidInt()) {
            value = INTEGER;
        } else if (isValidID()) {
            value = ID;
        } else {
            // token is invalid
            throw std::runtime_error("ERROR: Invalid Token: " + currentLine[currentIndex] + " at line: " + std::to_string(lineCount));
        }
        return value;
    }


};

int main() {
    Tokenizer trial("test");
    int k = trial.getToken();
    std::cout << k;
}

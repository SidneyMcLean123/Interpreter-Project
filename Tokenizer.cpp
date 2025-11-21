#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <exception>
#include <sstream>

using namespace std;

class Tokenizer {
    private:
    int lineCount;
        std::vector<std::string> currentLine;
        std::ifstream inputFile;

        const std::vector<std::string> reserved = {
            "program", "begin", "end", "int",
		    "if", "then", "else", "while",
		    "loop", "read", "write"
        };

        const std::vector<std::string> symbols = {
            ";", ",", "=", "!", "[", "]",
		    "&&", "||", "(", ")", "+", "-",
		    "*", "!=", "==", "<", ">", "<=",">="
        };
        
        void readLine() {
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
                    if (!count(reserved.begin(), reserved.end(), currentLine.at(i)) 
                    && !count(symbols.begin(), symbols.end(), currentLine.at(i))) {
                        // separate token and symbol
                        /*
                            TODO **************************************************
                            separate token and symbol *****************************
                            don't have to worry about foriegn symbols or characters
                        */
                    }
                }
            } else {
                currentLine.clear();
            }            
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
};

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>

using namespace std;

class ArithmeticCoding {
public:
    struct BitSymbol {
        bool value;
        long long int left;   
        long long int right;  
        long long int probability;  
    };

    vector<BitSymbol> buildBitTable(const string& input) {
        vector<BitSymbol> bitSymbols;
        for (char c : input) {
            bool found = false;
            for (BitSymbol& bitSymbol : bitSymbols) {
                if (bitSymbol.value == (c != ' ')) {
                    bitSymbol.probability += 1LL;  
                    found = true;
                    break;
                }
            }
            if (!found) {
                bitSymbols.push_back({ (c != ' '), 0LL, 0LL, 1LL });  
            }
        }

        long long int left = 0LL;
        for (BitSymbol& bitSymbol : bitSymbols) {
            bitSymbol.probability *= (1LL << 30); 
            bitSymbol.probability /= input.length();
            bitSymbol.left = left;
            bitSymbol.right = left + bitSymbol.probability;
            left = bitSymbol.right;
        }

        return bitSymbols;
    }

    long long int encode(const string& input, const vector<BitSymbol>& bitSymbolTable) {
        long long int left = 0LL;
        long long int right = (1LL << 60) - 1LL;
        for (char c : input) {
            bool isOne = (c != ' ');
            for (const BitSymbol& bitSymbol : bitSymbolTable) {
                if (bitSymbol.value == isOne) {
                    long long int range = right - left;
                    right = left + (range * bitSymbol.right) / (1LL << 30);  
                    left = left + (range * bitSymbol.left) / (1LL << 30);    
                    break;
                }
            }
        }
        return (left + right) / 2;
    }

    vector<char> decode(long long int code, const vector<BitSymbol>& bitSymbolTable, int length) {
        vector<char> result;
        for (int i = 0; i < length; ++i) {
            for (const BitSymbol& bitSymbol : bitSymbolTable) {
                if (code >= bitSymbol.left && code < bitSymbol.right) {
                    result.push_back((result.empty() || result.back() == ' ') ? ' ' : '0' + '1' - '0');
                    code = ((code - bitSymbol.left) << 30) / (bitSymbol.right - bitSymbol.left);  
                    break;
                }
            }
        }
        return result;
    }
};

int main() {
    try {
        ArithmeticCoding ac;
        cout << "Code:" << endl;

        string filename;

        cout << "Enter filename for input text: ";
        getline(cin, filename);

        ifstream inputFile(filename);
        if (!inputFile.is_open()) {
            cout << "Error opening file. Exiting...";
            return 1;
        }

        string input;
        getline(inputFile, input);
        inputFile.close();

        vector<ArithmeticCoding::BitSymbol> bitSymbolTable = ac.buildBitTable(input);

        long long int encodedValue = ac.encode(input, bitSymbolTable);
        cout << "Encoded: " << encodedValue << endl;
        ofstream outputFile("encoded.txt", ios::app);
        if (!outputFile) {
            cerr << "Error." << endl;
            return 1;
        }
        outputFile << encodedValue;
        outputFile.close();

        cout << "\nDecoding:" << endl;

        ifstream decodedFile("encoded.txt");
        if (!decodedFile) {
            cerr << "Error opening the file" << endl;
            return 1;
        }
        string encodedString((istreambuf_iterator<char>(decodedFile)), (istreambuf_iterator<char>()));
        long long int encodedValueFromFile = stoll(encodedString);
        decodedFile.close();

        vector<char> decodedInput = ac.decode(encodedValueFromFile, bitSymbolTable, input.length());
        cout << "Decoded: ";
        for (char c : decodedInput) {
            cout << c;
        }
        cout << endl;

        return 0;
    }
    catch (const exception& e) {
        cerr << "Error " << e.what() << endl;
        return 1;
    }
}
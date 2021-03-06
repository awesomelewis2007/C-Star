#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <algorithm>
#include "global.h"
#include "Modules/traceback.h"
using namespace std;
int getIndex(vector<string> v, string K)
{
    auto it = find(v.begin(), v.end(), K);
    if (it != v.end())
    {
        int index = it - v.begin();
        return index;
    }
    else {
        return -1;
    }
}
int main(int argc, char** argv) {
    ifstream file(argv[1]);
    bool in_main_function = false;
    vector<string> imported;
    vector<string> builtins;
    vector<string> variables;
    vector<string> variables_values;
    builtins.push_back("sys");
    builtins.push_back("vardb");
    if (file.is_open())
    {
        string line;
        while (getline(file, line))
        {
            if (line.rfind("//", 0) == 0) { //comment
                continue;
            }
            if (line.rfind("!main", 0) == 0) { //function
                in_main_function = true;
            }
            if (line.rfind("#using:", 0) == 0) {
                line.erase(0, 7);
                if (std::find(builtins.begin(), builtins.end(), line) != builtins.end()) {
                    imported.push_back(line);
                }
                else {
                    raise_error(3, "Module missing error", "The '"+line+"' module is not in system builtins");
                }
            }
            if (line.rfind("var:", 0) == 0) {
                line.erase(0, 4);
                string removestring = "var:";
                auto n = line.find("var:");
                string delimiter = "=";

                size_t pos = 0;
                string var_name;
                string var_data;
                while ((pos = line.find(delimiter)) != std::string::npos) {
                    var_name = line.substr(0, pos);
                    line.erase(0, pos + delimiter.length());
                }
                var_data = line;
                variables.push_back(var_name);
                variables_values.push_back(var_data);
            }
            
            if (in_main_function == true) {
                //SYS Module
                if (line.rfind("sys.", 0) == 0) {
                    if (std::find(imported.begin(), imported.end(), "sys") != imported.end()) {
                        if (line.rfind("sys.out:", 0) == 0) {
                            line.erase(0, 8);
                            if (line.rfind('"', 0) == 0) {
                                line.erase(remove(line.begin(), line.end(), '"'), line.end());
                                cout << line;
                            }
                            else {
                                if (std::find(variables.begin(), variables.end(), line) != variables.end()) {
                                    cout << variables_values[getIndex(variables, line)];
                                }
                            }
                        }
                        if (line.rfind("sys.out.newline", 0) == 0) {
                            cout << "\n";
                        }
                        if (line.rfind("sys.exit:", 0) == 0) {
                            line.erase(0, 9);
                            exit(stoi(line));
                        }
                        if (line.rfind("sys.run:", 0) == 0) {
                            line.erase(0, 8);
                            system(line.c_str());
                        }
                    }
                    else {
                        raise_error(2, "Module import error", "The sys module was used but never imported");
                        exit(1);
                    }
                }
                //VARDB Module
                if (line.rfind("vardb.", 0) == 0) {
                    if (std::find(imported.begin(), imported.end(), "vardb") != imported.end()) {
                        if (line.rfind("vardb.ls", 0) == 0) {
                            for (int i = 0; i < variables.size(); i++)
                                std::cout << variables[i] << ' ';
                        }
                    }
                    else {
                        raise_error(2, "Module import error", "The sys module was used but never imported");
                    }
                }
            }
        }
    }
    else {
        cout << "Cant find file "<< argv[1] << "\n";
    }
	
}

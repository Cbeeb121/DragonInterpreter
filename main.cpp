#include <fstream>
#include <string.h>
#include <iostream>

// #include "errors.hpp"
#include "scanner.hpp"
#include "ast.hpp"
#include "name_analysis.hpp"
#include "type_analysis.hpp"

using namespace holeyc;
using namespace std;

bool dealWithInput(string input);

int main(){
  cout << "> Welcome to dragoninterp! Enter HoleyC code to be interpreted...\n";
  string input; 
  while(true){
    cin.clear();
    cout << "> ";
    getline(cin, input);
    if(input == "quit"){
      return 0;
    }
    if(input.find("{") != string::npos){
      int brace_equality = 1;
      string tabs = "";
      while(brace_equality != 0) {
        cin.clear();
        cout << ". " << string(brace_equality,'\t');
        getline(cin, input);
        if (input.find("{") != string::npos) { brace_equality++; }
        if (input.find("}") != string::npos) { brace_equality--; }
      }
    }
    if(!dealWithInput(input)){
      // we got an error in the code so lets quit.
      return 1;
    }
  }
  return 0;
}

bool dealWithInput(string input){
  // Here we need to do a few things:
  // 1. Parse the input into Nodes?
  // 2. Update the AST (add to it)
  // 3. Do (partial) NameAnalysis on the existing AST
  // 4. Do TypeAnalysis (if necessary: types, IDs, math, boolean operations)
  // 5. Perform `eval` which will handle:
  //    * Add values to Nodes to necessary (do lookup on SymbolTable)
  //    * Perform TOCONSOLE, Plus stuff
  return true;
}

static holeyc::ProgramNode *syntacticAnalysis(std::ifstream *input)
{
  if (input == nullptr)
  {
    return nullptr;
  }

  holeyc::ProgramNode *root = nullptr;

  holeyc::Scanner scanner(input);
#if 1
  holeyc::Parser parser(scanner, &root);
#else
  holeyc::Parser parser(scanner);
#endif

  int errCode = parser.parse();
  if (errCode != 0)
  {
    return nullptr;
  }

  return root;
}

static holeyc::NameAnalysis *doNameAnalysis(std::ifstream *input){
  holeyc::ProgramNode *ast = syntacticAnalysis(input);
  if (ast == nullptr)
  {
    return nullptr;
  }

  return holeyc::NameAnalysis::build(ast);
}

static holeyc::TypeAnalysis *doTypeAnalysis(std::ifstream *input){
  holeyc::NameAnalysis *nameAnalysis = doNameAnalysis(input);
  if (nameAnalysis == nullptr)
  {
    return nullptr;
  }

  return holeyc::TypeAnalysis::build(nameAnalysis);
}
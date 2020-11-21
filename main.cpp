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
static holeyc::ProgramNode *syntacticAnalysis(std::ifstream *input);
static holeyc::NameAnalysis *doNameAnalysis(std::ifstream *input);
static holeyc::TypeAnalysis * doTypeAnalysis(std::ifstream *input);

int main(){
  holeyc::ProgramNode * ast = new holeyc::ProgramNode(new std::list<StmtNode *>()); // this is the ast we will be adding globals to.
  holeyc::ProgramNode * temp = nullptr;
  StmtNode * stmt = nullptr;
  holeyc::NameAnalysis *nameAnalysis = new holeyc::NameAnalysis;
  TypeAnalysis *typeAnalysis = new TypeAnalysis();
  SymbolTable *symTab = new SymbolTable();
  ast->nameAnalysis(symTab);
  symTab->enterScope();

  cout << "> Welcome to dragoninterp! Enter HoleyC code to be interpreted...\n";
  string input;
  while(true){
    cin.clear();
    cout << "> ";
    getline(cin, input);
    if(input == "quit"){
      symTab->leaveScope();
      return 0;
    }
    if(input.find("{") != string::npos){
      string temp = input + "\n";
      size_t brace_equality = 1;
      string tabs = "";
      while(brace_equality != 0) {
        cin.clear();
        cout << ". " << string(brace_equality,'\t');
        getline(cin, input);
        temp = temp + input + "\n";
        if (input.find("{") != string::npos) { brace_equality++; }
        if (input.find("}") != string::npos) { brace_equality--; }
      }
      input = temp;
    }
    ofstream outStream("string.txt");
    outStream << input;
    outStream.close();
    ifstream myFile("string.txt");
    temp = syntacticAnalysis(&myFile);
    if(temp == nullptr){ cout << "error!"; return 1; }
    stmt = temp->getGlobals()->front(); // expect the input to be converted into a StmtNode found at the front of the globals list
    ast->addGlobal(stmt);
    if(!ast->getGlobals()->back()->nameAnalysis(symTab)){ // perform nameAnalysis on latest addition
      // ERROR
      return 1; 
    } else {
      ast->getGlobals()->back()->typeAnalysis(typeAnalysis); // perform typeAnalysis on latest addition.
      // TypeAnalysis will handle:
      // 1) setting values (a = 2; set a's symbol value to 2)
      // 2) printing (TOCONSOLE b; just print b's value)
      // 3) arithmetic shit ( &&, +, -)
    }
  }
  symTab->leaveScope();
  return 0;
}

bool dealWithInput(string input){
  // Here we need to do a few things:
  
  // * parse the input. 
  // * add input ONTO existing ast's globals. (may change depending on if assignment, etc)
  // * do nameAnalysis on that latest input to AST (ast.getGlobals().last)
  // * do typeAnalysis (maybe) on that input (ast.getGlobals().last)
  // * perform eval if necessary on that last bit of input (ast.getGlobals().last)

  // 1. Parse the input into Nodes?
  // 2. Update the AST (add to it)
  // 3. Do (partial) NameAnalysis on the existing AST
  // 4. Do TypeAnalysis (if necessary: types, IDs, math, boolean operations)
  // 5. Perform `eval` which will handle:
  //    * Add values to Nodes to necessary (do lookup on SymbolTable)
  //    * Perform TOCONSOLE, Plus stuff
  return true;
}

static holeyc::ProgramNode * syntacticAnalysis(std::ifstream *input){
  if (input == nullptr){
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
  if (errCode != 0){
    return nullptr;
  }

  return root;
}

static holeyc::NameAnalysis * doNameAnalysis(std::ifstream *input){
  holeyc::ProgramNode *ast = syntacticAnalysis(input);
  if (ast == nullptr)
  {
    return nullptr;
  }

  return holeyc::NameAnalysis::build(ast);
}

static holeyc::TypeAnalysis * doTypeAnalysis(std::ifstream *input){
  holeyc::NameAnalysis *nameAnalysis = doNameAnalysis(input);
  if (nameAnalysis == nullptr){
    return nullptr;
  }

  return holeyc::TypeAnalysis::build(nameAnalysis);
}
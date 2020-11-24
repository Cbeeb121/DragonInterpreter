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

void evaluateCallStmt(StmtNode * callStmt);
static holeyc::ProgramNode *syntacticAnalysis(std::ifstream *input);
static holeyc::NameAnalysis *doNameAnalysis(std::ifstream *input);
static holeyc::TypeAnalysis * doTypeAnalysis(std::ifstream *input);

holeyc::ProgramNode * ast = new holeyc::ProgramNode(new std::list<StmtNode *>()); // this is the ast we will be adding globals to.
holeyc::NameAnalysis *nameAnalysis = new holeyc::NameAnalysis;
TypeAnalysis *typeAnalysis = new TypeAnalysis();
SymbolTable *symTab = new SymbolTable();

int main(){
  holeyc::ProgramNode * temp = nullptr;
  StmtNode * stmt = nullptr;
  // holeyc::NameAnalysis *nameAnalysis = new holeyc::NameAnalysis;
  // TypeAnalysis *typeAnalysis = new TypeAnalysis();
  // SymbolTable *symTab = new SymbolTable();
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
      if ((input.find("if") != string::npos || input.find("while") != string::npos) && (input.find("(") != string::npos && input.find(")") != string::npos)){
        cout << "ERROR: Cannot perform conditionals outside of a function.\n";
        return 1;
      }
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
    StmtNode * current_stmt = ast->getGlobals()->back();
    if(!current_stmt->nameAnalysis(symTab)){ // perform nameAnalysis on latest addition. Quit if failure.
      return 1;
    } else {
      if (current_stmt->isCallStmt()){
        current_stmt->typeAnalysis(typeAnalysis);
        evaluateCallStmt(current_stmt); // deal with callStmts differently.
      } else if (current_stmt->isFnDecl()) {
      } else {
        current_stmt->typeAnalysis(typeAnalysis); // perform typeAnalysis on latest addition.
      }
    }
  }
  symTab->leaveScope();
  return 0;
}

void evaluateCallStmt(StmtNode * callStmt){
  CallExpNode * callExp = callStmt->getCallExp();
  if(callExp){
    SemSymbol * fnSym = symTab->find(callExp->getID()->getName());
    if(fnSym){
      std::list<StmtNode *>::iterator it;
      it = ast->getGlobals()->begin();
      bool not_found = true;
      while(not_found && (it != ast->getGlobals()->end())){
        if ((*it)->isFnDecl() && (*it)->callFnName(callExp->getID()->getName())){
          not_found = false;
          (*it)->typeAnalysis(typeAnalysis);
        }
        ++it;
      }
    }
  }
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
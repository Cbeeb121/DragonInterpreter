#ifndef HOLEYC_SYMBOL_TABLE_HPP
#define HOLEYC_SYMBOL_TABLE_HPP
#include <string>
#include <unordered_map>
#include <list>
#include "types.hpp"

//Use an alias template so that we can use
// "HashMap" and it means "std::unordered_map"
template <typename K, typename V>
using HashMap = std::unordered_map<K, V>;

using ArgsType = std::list<const holeyc::DataType *>;

using namespace std;

namespace holeyc{

enum SymbolKind {
	VAR, FN
};

//A semantic symbol, which represents a single
// variable, function, etc. Semantic symbols 
// exist for the lifetime of a scope in the 
// symbol table. 
class SemSymbol {
public:
	SemSymbol(std::string nameIn, DataType * typeIn) 
	: myName(nameIn), myType(typeIn){ }
	virtual std::string toString();
	std::string getName() const { return myName; }
	virtual SymbolKind getKind() const = 0;

	virtual DataType * getDataType() const{
		return myType;
	}
	static std::string kindToString(SymbolKind symKind) { 
		switch(symKind){
			case VAR: return "var";
			case FN: return "fn";
		}
		return "UNKNOWN KIND";
	}
  
  void addIntValue(int val) {
    valType = "int";
    myValInt = val;
  }
  void addBoolValue(bool val) {
    valType = "bool";
    myValBool = val;
  }
  void addCharValue(char val) {
    valType = "char";
    myValChar = val;
  }

  string getValType(){
    return valType;
  }

  int * getIntVal() {
    int * val = new int;
    *val = myValInt;
    return val;
  }
  char * getCharVal() {
    char * val = new char;
    *val = myValChar;
    return val;
  }
  bool * getBoolVal() {
    bool * val = new bool;
    *val = myValBool;
    return val;
  }

private:
	std::string myName;
  DataType * myType;
  std::string myValString; // CLB: 11/21/20
  string valType = ""; 
  int myValInt; // CLB: 11/21/20
  char myValChar; // CLB: 11/21/20
  bool myValBool; // CLB: 11/21/20
};

class VarSymbol : public SemSymbol {
public:
	VarSymbol(std::string name, DataType * type) 
	: SemSymbol(name, type) { }
	virtual SymbolKind getKind() const override { return VAR; }
};

class FnSymbol : public SemSymbol{
public:
	FnSymbol(std::string name, FnType * fnType)
	: SemSymbol(name, fnType){ }
	virtual SymbolKind getKind() const { return FN; }
	SymbolKind getKind(){ return FN; } 
};

//A single scope. The symbol table is broken down into a 
// chain of scope tables, and each scope table holds 
// semantic symbols for a single scope. For example,
// the globals scope will be represented by a ScopeTable,
// and the contents of each function can be represented by
// a ScopeTable.
class ScopeTable {
	public:
		ScopeTable();
		SemSymbol * lookup(std::string name);
		bool insert(SemSymbol * symbol);
		bool clash(std::string name);
		std::string toString();
		void addVar(std::string name, DataType * type){
			insert(new VarSymbol(name, type));
		}
		void addFn(std::string name, FnType * type){
			insert(new FnSymbol(name, type));
		}
	private:
		HashMap<std::string, SemSymbol *> * symbols;
};

class SymbolTable{
	public:
		SymbolTable();
		ScopeTable * enterScope();
		void leaveScope();
		ScopeTable * getCurrentScope();
		bool insert(SemSymbol * symbol);
		SemSymbol * find(std::string varName);
		bool clash(std::string name);
		void addVar(std::string name, DataType * type){
			getCurrentScope()->addVar(name, type);
		}
		void addFn(std::string name, FnType * type){
			getCurrentScope()->addFn(name, type);
		}
		void print();
	private:
		std::list<ScopeTable *> * scopeTableChain;
};

	
}

#endif

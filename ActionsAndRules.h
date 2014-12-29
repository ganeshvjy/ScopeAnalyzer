#ifndef ACTIONSANDRULES_H
#define ACTIONSANDRULES_H
/////////////////////////////////////////////////////////////////////
//  ActionsAndRules.h - declares new parsing rules and actions     //
//  ver 3.0                                                        //
//  Lanaguage:       Visual C++ 2013                               //
//  Platform:        Dell Inspiron, Windows 8                      //
//  Application:     CSE687 - Project #1, Spring 2014              //
//  Author:          Manjushri Thyagarajan, Syracuse University    //
//                  (315) 200-3436, mthyagar@syr.edu               //
//  Original Author: Jim Fawcett, CST 2-187, Syracuse University   //
//                  (315) 443-3948, jfawcett@twcny.rr.com          //
/////////////////////////////////////////////////////////////////////
/*
Module Operations:
==================
This module defines several action classes.  Its classes provide
specialized services needed for specific applications.  The modules
Parser, SemiExpression, and Tokenizer, are intended to be reusable
without change.  This module provides a place to put extensions of
these facilities and is not expected to be reusable.

Public Interface:
=================
Toker t(someFile);              // create tokenizer instance
SemiExp se(&t);                 // create a SemiExp attached to tokenizer
Parser parser(se);              // now we have a parser
Rule1 r1;                       // create instance of a derived Rule class
Action1 a1;                     // create a derived action
r1.addAction(&a1);              // register action with the rule
parser.addRule(&r1);            // register rule with parser
while(se.getSemiExp())          // get semi-expression
parser.parse();               //   and parse it

Build Process:
==============
Required files
- Parser.h, Parser.cpp, ScopeStack.h, ScopeStack.cpp,
ActionsAndRules.h, ActionsAndRules.cpp, ConfigureParser.cpp,
ItokCollection.h, SemiExpression.h, SemiExpression.cpp, tokenizer.h, tokenizer.cpp
Build commands (either one)
- devenv CodeAnalysis.sln
- cl /EHsc /DTEST_PARSER parser.cpp ActionsAndRules.cpp \
semiexpression.cpp tokenizer.cpp /link setargv.obj

Maintenance History:
====================
ver 2.0 : 01 Jun 11
- added processing on way to building strong code analyzer
ver 1.1 : 17 Jan 09
- changed to accept a pointer to interfaced ITokCollection instead
of a SemiExpression
ver 1.0 : 12 Jan 06
- first release

*/
//
#pragma once
#include <queue>
#include<iostream>
#include <string>
#include <sstream>
#include "Parser.h"
#include "ITokCollection.h"
#include "ScopeStack.h"
#include "Tokenizer.h"
#include "SemiExpression.h"
#include "MTree\MTree\MNode.h"
#include "MTree\MTree\MTree.h"
#include <list>
#include "XmlWriter\XmlWriter\XmlWriter.h"

using namespace TMTree;


///////////////////////////////////////////////////////////////
// ScopeStack element is application specific

struct element
{
	std::string type;
	std::string name;
	size_t lineCount;
	std::string show()
	{
		std::ostringstream temp;
		temp << "(";
		temp << type;
		temp << ", ";
		temp << name;
		temp << ", ";
		temp << lineCount;
		temp << ")";
		return temp.str();
	}
};

///////////////////////////////////////////////////////////////
//struct var is application specific >--------------------------------------------
struct var
{
	std::string type;
	size_t start_lineCount;
	size_t end_lineCount;
	int level;
	std::string show()
	{
		std::ostringstream temp;
		temp << "(";
		temp << type;
		temp << ", ";
		temp << type;
		temp << ", ";
		temp << start_lineCount;
		temp << ", ";
		temp << end_lineCount;
		temp << ")";
		return temp.str();
	}
};

///////////////////////////////////////////////////////////////
//tree element holds all scopes within a function >--------------------------------------------
struct tree_element{
	std::string func_name;
	std::queue<var> nodes;
	int level;
	int node_count;
};

///////////////////////////////////////////////////////////////
// Display class handles all display operations

class Display{
public:
	tree_element te;
	Display(){}
	~Display(){}
	//Prints all scopes within a particular function
	void show_only_funcDetails(tree_element te){
		    std::cout << "\n Demonstrating requirements for /b switch \n\n";
			std::cout << "Func name| Size | Complexity " << std::endl;
			std::cout << "-------------------------------" << std::endl;
			std::cout << te.func_name << "\t|" << te.node_count << "\t|" << te.nodes.size() << std::endl;	
	}	
	//Prints the scope name
	void printTitle(std::string elemName){
		std::cout << "\n Scope within function   : " << elemName << "\n\n";
		std::cout << "Type   | Start Line | End Line " << std::endl;
		std::cout << "--------------------------------------" << std::endl;
	}
	//Prints the scope type and its attributes
	void printElement(element e, int endLine){
		std::cout << std::endl << "Type           :" << e.type << "\n";
		std::cout << "Name           :" << e.name << std::endl;
		std::cout << "Start line     :" << e.lineCount << std::endl;
		std::cout << "End line       :" << endLine << std::endl;
	}
};

///////////////////////////////////////////////////////////////
// Repository instance is used to share resources
// among all actions.

class Repository  // application specific
{
	ScopeStack<element> stack;
	Toker* p_Toker;
public:
	Repository(Toker* pToker)
	{
		p_Toker = pToker;
	}
	ScopeStack<element>& scopeStack()
	{
		return stack;
	}
	Toker* Toker()
	{
		return p_Toker;
	}
	size_t lineCount()
	{
		return (size_t)(p_Toker->lines());
	}
};

///////////////////////////////////////////////////////////////
// rule to detect beginning of anonymous scope

class BeginningOfScope : public IRule
{
public:
	bool doTest(ITokCollection*& pTc)
	{
		//std::cout << "\n--BeginningOfScope rule";
		if (pTc->find("{") < pTc->length())
		{
			doActions(pTc);
			return false;
		}
		return false;
	}
};
///////////////////////////////////////////////////////////////
// action to handle scope stack at end of scope

class HandlePush : public IAction
{
	Repository* p_Repos;
public:
	HandlePush(Repository* pRepos)
	{
		p_Repos = pRepos;
	}
	void doAction(ITokCollection*& pTc)
	{
		element elem;
		elem.type = "unknown";
		elem.name = "anonymous";
		elem.lineCount = p_Repos->lineCount();
		p_Repos->scopeStack().push(elem);
	}
};

///////////////////////////////////////////////////////////////
// rule to detect end of scope

class EndOfScope : public IRule
{
public:
	bool doTest(ITokCollection*& pTc)
	{
		//std::cout << "\n--EndOfScope rule";
		if (pTc->find("}") < pTc->length())
		{
			doActions(pTc);
			return false;
		}
		return false;
	}
};

///////////////////////////////////////////////////////////////
// action to handle scope stack at end of scope

class HandlePop : public IAction
{
	Repository* p_Repos;
	std::queue<var>q1;
	std::vector<tree_element> list_funcs;
	typedef MNode<std::string> Node;
	Display d;
public:
	//Count to track the function with maximum scopes
	int node_count = 0;
	//tree_element max_node_count;
	//Switch to handle "/b" Function complexity information
	static bool FuncCom;
	//Structure to hold max scope function element
	static struct tree_element max_node_count;
	HandlePop(){

	}
	HandlePop(Repository* pRepos)
	{
		p_Repos = pRepos;
	}
	void clearMax(){
		max_node_count.func_name = "";
		max_node_count.level = 0;
		max_node_count.nodes.empty();
		max_node_count.node_count = 0;
	}
	void setCompSwitch(bool value){
		HandlePop::FuncCom = value;
	}
	int isSpecialKeyWord(const std::string& tok)
	{
		const static std::string keys[]
			= { "for", "while", "switch", "if", "catch","else","try"};
		for (int i = 0; i<8; ++i)
		if (tok == keys[i])
			return 0;
		return -1;
	}
	int isConstruct(const std::string& tok)
	{
		const static std::string keys[]
			= { "namespace", "class", "enum", "struct" };
		for (int i = 0; i < 4; i++)
		if (tok == keys[i])
			return 0;
		return -1;
	}
	//Recursively builds the tree based on the scope levels
	void buildTree(int level, Node* &pRoot, std::list<var>::iterator& it_begin, std::list<var>::iterator it_end){
		if (it_begin == it_end) return;
		if (it_begin->level != level){
			return;
		}
		Node* pChild = new Node(it_begin->type);
		pRoot->addChild(pChild);
		buildTree(level + 1, pChild, ++it_begin, it_end);
		buildTree(level, pRoot, it_begin, it_end);	
	}
	// Prints the tree structure format of function scopes
	void printTree(tree_element te){
		class newOper : public Operation<Node>
		{
			public:
				bool operator()(Node* pNode)
				{
					std::cout << "\n -- " << pNode->ToString();
					return false;
				}
		};
		std::list<var>li;
		Node* pRoot = new Node(te.func_name);
		int level = te.level+1;
		//Stores all the scopes within a function to a list
		//Helps to iterate through it
		for (int i = 0; te.nodes.size()!=0; i++){
			var value = te.nodes.front();
			li.push_front(value);
			te.nodes.pop();		
		}
		std::list<var>::iterator& it_begin = li.begin();
		std::list<var>::iterator& it_end = li.end();
		buildTree(level, pRoot, it_begin,it_end);
		//Outputs the built tree
		MTree<Node> tree(new newOper);
		tree.makeRoot(pRoot);
		tree.verbose() = true;
		tree.walk(pRoot);
		std::cout << std::endl;
	}
	void doAction(ITokCollection*& pTc)
	{
		tree_element te;
		if (p_Repos->scopeStack().size() == 0)
			return;
		element elem = p_Repos->scopeStack().pop();
		if (elem.type == "function")
		{
			d.printElement(elem, p_Repos->lineCount());
			te.func_name = elem.name;
			te.node_count = p_Repos->lineCount() - elem.lineCount;
			if (q1.size() != 0){
				d.printTitle(elem.name);
				te.node_count = q1.size();
			}
			for (int i = 0; q1.size()!=0; i++){	
				//Stores all the scopes within a function
				//Needed to build tree and XML
				var value = q1.front();
				te.level = p_Repos->scopeStack().size();
				te.nodes.push(value);
				q1.pop();
				std::cout << value.type << "\t|" << value.start_lineCount << "\t|" << value.end_lineCount << std::endl;
			}
			list_funcs.push_back(te);
			printTree(te);
			//Prints the node count of every function
			std::cout << "\n  Node count : " << te.nodes.size() << "\n\n";
			//Stores the function element with maximum nodes into a structure
			if (te.nodes.size() > max_node_count.nodes.size()){
				//node_count = te.nodes.size();
				max_node_count = te;
			}
			if (FuncCom){
				d.show_only_funcDetails(te);
			}
		}
		//Store all the scopes of a function into a queue
		//Prints only when function is popped
		if (isSpecialKeyWord(elem.type) == 0)
		{
			var v1;
			v1.type = elem.type;
			v1.start_lineCount = elem.lineCount;
			v1.end_lineCount = p_Repos->lineCount();
			v1.level = p_Repos->scopeStack().size();
			q1.push(v1);
		}
		//Print when any construct from list is encountered
		if (isConstruct(elem.type) == 0){
			d.printElement(elem, p_Repos->lineCount());
		}
	}
	void mOut(const std::string& message)
	{
		std::cout << "\n  " << message.c_str();
	}

	//Recursively builds the XML structure of the function element
	void buildXML(int level,XmlWriter& wtr, std::list<var>::iterator& it_begin, std::list<var>::iterator it_end){
		if (it_begin == it_end) return;
		if (level != it_begin->level){
			return;
		}
		wtr.start(it_begin->type);
		buildXML(level + 1, wtr, ++it_begin, it_end);
		wtr.end();
		buildXML(level, wtr, it_begin, it_end);
	    
	}
	//Prints the XML format of the function with maximum scope count
	void printMaxElement(){
		std::cout << "Do XML Processing here\n";
		std::cout << "  Func name:   " << max_node_count.func_name << std::endl << std::endl;
		std::list<var>li;
		//Rearrange by popping the elements of queue into a list
		//Helps to iterate throught the ist to build the XML
		for (int i = 0; max_node_count.nodes.size() != 0; i++){
			var value = max_node_count.nodes.front();
			li.push_front(value);
			max_node_count.nodes.pop();
		}
		
		std::list<var>::iterator& it_begin = li.begin();
		std::list<var>::iterator& it_end = li.end();

		XmlWriter wtr;
		wtr.indent();
		wtr.addDeclaration();
		wtr.addComment("XML Representation");
		wtr.start("Root");
		int level = max_node_count.level + 1;
		wtr.addAttribute("Function",max_node_count.func_name);
		buildXML(level,wtr,it_begin,it_end);
		wtr.end();
		//Outputs the built XML onto the output
		mOut(wtr.xml());

	}
};


///////////////////////////////////////////////////////////////
// rule to detect preprocessor statements

class PreprocStatement : public IRule
{
public:
	bool doTest(ITokCollection*& pTc)
	{
		//std::cout << "\n--PreprocStatement rule";
		if (pTc->find("#") < pTc->length())
		{
			doActions(pTc);
			return true;
		}
		return false;
	}
};

///////////////////////////////////////////////////////////////
// action to print preprocessor statement to console

class PrintPreproc : public IAction
{
public:
	void doAction(ITokCollection*& pTc)
	{
		std::cout << "\n\n  Preproc Stmt: " << pTc->show().c_str();
	}
};

///////////////////////////////////////////////////////////////
// rule to detect function definitions

class FunctionDefinition : public IRule
{
public:
	bool isSpecialKeyWord(const std::string& tok)
	{
		const static std::string keys[]
			= { "for", "while", "switch", "if", "catch"};
		for (int i = 0; i<8; ++i)
		if (tok == keys[i])
			return true;
		return false;
	}
	
	bool doTest(ITokCollection*& pTc)
	{
		ITokCollection& tc = *pTc;
		if (tc[tc.length() - 1] == "{")
		{
			size_t len = tc.find("(");
			if (len < tc.length() && !isSpecialKeyWord(tc[len - 1]))
			{
				//std::cout << "\n--FunctionDefinition rule";
				doActions(pTc);
				return true;
			}
			else{
				if (isSpecialKeyWord(tc[len - 1])){
					//std::cout << std::endl<< "KeywordDefinition rule";
					doActions(pTc);
					return true;
				}
			}
		}
		return false;
	}
};
///////////////////////////////////////////////////////////////
// action to push function name onto ScopeStack

class PushFunction : public IAction
{
	Repository* p_Repos;
public:
	PushFunction(Repository* pRepos)
	{
		p_Repos = pRepos;
	}
	bool isSpecialKeyWord(const std::string& tok)
	{
		const static std::string keys[]
			= { "for", "while", "switch", "if", "catch"};
		for (int i = 0; i<7; ++i)
		if (tok == keys[i])
			return true;
		return false;
	}
	void doAction(ITokCollection*& pTc)
	{
		// pop anonymous scope
		p_Repos->scopeStack().pop();

		if (isSpecialKeyWord((*pTc)[pTc->find("(") - 1])){
			element elem;
			std::string name = "";
			elem.type = (*pTc)[pTc->find("(") - 1];
			elem.name = name;
			elem.lineCount = p_Repos->lineCount();
			p_Repos->scopeStack().push(elem);
		}
		// push function scope
		else{
			std::string name = (*pTc)[pTc->find("(") - 1];
			element elem;
			elem.type = "function";
			elem.name = name;
			elem.lineCount = p_Repos->lineCount();
			p_Repos->scopeStack().push(elem);
			//call from the func doActions to push
		}
	}
};

///////////////////////////////////////////////////////////////
// action to send semi-expression that starts a function def
// to console
class PrintFunction : public IAction
{
public:
	void doAction(ITokCollection*& pTc)
	{
		std::cout << "\n\n  FuncDef Stmt: " << pTc->show().c_str();
	}
};

///////////////////////////////////////////////////////////////
// action to send signature of a function def to console

class PrettyPrintFunction : public IAction
{
public:
	void doAction(ITokCollection*& pTc) 
	{
		pTc->remove("public");
		pTc->remove(":");
		pTc->trimFront();
		int len = pTc->find(")");
		std::cout << "\n\n  Pretty Stmt:    ";
		for (int i = 0; i<len + 1; ++i)
			std::cout << (*pTc)[i] << " ";
	}
};

class ConstructDefinition : public IRule
{
public:
	bool isConstruct(const std::string& tok)
	{
		const static std::string keys[]
			= { "namespace", "class", "enum", "struct" };
		for (int i = 0; i < 4; i++)
		if (tok == keys[i])
			return true;
		return false;
	}
	bool isSplScope(const std::string& tok){
		const static std::string keys[]
			= { "else", "do", "try"};
		for (int i = 0; i < 3; i++)
		if (tok == keys[i])
			return true;
		return false;
	}
	bool doTest(ITokCollection*& pTc)
	{
		ITokCollection& tc = *pTc;
		if (tc[tc.length() - 1] == "{")
		{
			size_t len = tc.find("{");
			if (len < tc.length() && len == 2)
			{
				//std::cout << "\n-- Contruct Definition rule";
				if (isConstruct(tc[len - 2])){
					doActions(pTc);
					return true;
				}
			}
			if (len < tc.length() && len == 1)
			{
				if (isSplScope(tc[len - 1])){
					doActions(pTc);
					return true;
				}
			}
		}
		return false;
	}
};

///////////////////////////////////////////////////////////////
// action to push Loop name onto ScopeStack

class PushConstruct : public IAction
{
	Repository* p_Repos;
public:
	PushConstruct(Repository* pRepos)
	{
		p_Repos = pRepos;
	}
	bool isConstruct(const std::string& tok)
	{
		const static std::string keys[]
			= { "namespace", "class", "enum", "struct" };
		for (int i = 0; i < 4; i++)
		if (tok == keys[i])
			return true;
		return false;
	}
	bool isSplScope(const std::string& tok){
		const static std::string keys[]
			= { "else", "do", "try" };
		for (int i = 0; i < 3; i++)
		if (tok == keys[i])
			return true;
		return false;
	}
	void doAction(ITokCollection*& pTc)
	{
		// pop anonymous scope
		p_Repos->scopeStack().pop();
		ITokCollection& tc = *pTc;
		size_t len = tc.find("{");
		if (len < tc.length() && len == 2){
			if(isConstruct(tc[len - 2])){
				element elem;
				elem.type = tc[len - 2];
				elem.name = tc[len - 1];
				elem.lineCount = p_Repos->lineCount();
				p_Repos->scopeStack().push(elem);
			}
		}
		if (len < tc.length() && len == 1){
			if (isSplScope(tc[len - 1])){
				element elem;
				elem.type = tc[len - 1];
				elem.name = "";
				elem.lineCount = p_Repos->lineCount();
				p_Repos->scopeStack().push(elem);
			}
		}
	}
};

#endif
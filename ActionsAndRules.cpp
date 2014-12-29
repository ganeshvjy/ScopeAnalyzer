/////////////////////////////////////////////////////////////////////
//  ActionsAndRules.cpp - implements new parsing rules and actions //
//  ver 3.0                                                        //
//  Lanaguage:       Visual C++ 2013                               //
//  Platform:        Dell Inspiron, Windows 8                      //
//  Application:     CSE687 - Project #1, Spring 2014              //
//  Author:          Manjushri Thyagarajan, Syracuse University    //
//                  (315) 200-3436, mthyagar@syr.edu               //
//  Original Author: Jim Fawcett, CST 2-187, Syracuse University   //
//                  (315) 443-3948, jfawcett@twcny.rr.com          //
/////////////////////////////////////////////////////////////////////
#include "ActionsAndRules.h"

bool HandlePop::FuncCom = false;

struct tree_element HandlePop::max_node_count;

//int HandlePop::node_count = 0;

#ifdef TEST_ACTIONSANDRULES

#include <iostream>
#include "ActionsAndRules.h"
#include "Tokenizer.h"
#include "SemiExpression.h"

int main(int argc, char* argv[])
{
  std::cout << "\n  Testing ActionsAndRules class\n "
            << std::string(30,'=') << std::endl;

  try
  {
    std::queue<std::string> resultsQ;
    PreprocToQ ppq(resultsQ);
    PreprocStatement pps;
    pps.addAction(&ppq);

    FunctionDefinition fnd;
    PrettyPrintToQ pprtQ(resultsQ);
    fnd.addAction(&pprtQ);

    Toker toker("../ActionsAndRules.h");
    SemiExp se(&toker);
    Parser parser(&se);
    parser.addRule(&pps);
    parser.addRule(&fnd);
    while(se.get())
      parser.parse();
    size_t len = resultsQ.size();
    for(size_t i=0; i<len; ++i)
    {
      std::cout << "\n  " << resultsQ.front().c_str();
      resultsQ.pop();
    }
    std::cout << "\n\n";
  }
  catch(std::exception& ex)
  {
    std::cout << "\n\n  " << ex.what() << "\n\n";
  }
}
#endif

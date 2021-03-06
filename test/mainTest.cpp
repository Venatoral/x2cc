/*
 * testParseNumber.cpp
 *
 *  Created on: 2017?3?22?
 *      Author: 13774
 */

#include <LexicalParser.h>
#include <cstring>
#include <cstdio>
#include <cassert>
#include <fstream>
#include <iostream>
#include <GrammaUtils.h>
#include <sstream>
#include <MutualMap.h>
#include <FAUtils.h>
#include <StringUtils.h>
#include <GrammarTranslateUtils.h>
#include <SimpleCppTranslator.h>

#include <macros/all.h>
using namespace std;
using namespace x2;

#define BUFLEN 1024
char buffer[BUFLEN];

void testFile();
void testParseWord();
void testString();
void testNumber();
void testNote();
void testRecursiveMacro();
void testGramma();

void testGramma();
//LR(1) Grammar Only
void testLR1();
void assignment_LR();
void testAmbigous();
void testReadFromFile();
void testSimplifiedGrammar();
void testMutualMap();
void testStreamFA();
void testDFA();
void testGrammarDefineIsAdding();
void testVerifiedGrammarTranslation();
void testMutualMapInt();
void testLexicalStream();
void testCombinedLexicalGrammarTranslation();

int main()
{
//	testGramma();
//	testLR1();
//	assignment_LR();
//	testAmbigous();
//	testReadFromFile();
//	testMutualMap();
//	testStreamFA();
//	testDFA();
//	testGrammarDefineIsAdding();
//	testSimplifiedGrammar();
//	testVerifiedGrammarTranslation();
//	testMutualMapInt();
//	testLexicalStream();
	testCombinedLexicalGrammarTranslation();
}
/**
 * @brief test as an united process
 * @detail  a united test, this will test the combination/cooperation between
 * these tools.
 */
void testCombinedLexicalGrammarTranslation()
{
	//== define a grammar
	LR1Gramma lr1g(Gramma("examples/normal_test3.grammar"),
			"C","$","S'"
	);
	std::cout << lr1g.toString() <<std::endl;

	//== parse the lexical stream
	LexicalParser lp;
	std::ifstream infile("examples/normal_test3.cpp");
	auto stream=lp.parseWords(infile);
	DefaultLexcialToGrammarStream myStream(stream,lr1g);

	//== retrieve information from the grammar
	auto lr1info = lr1g.getAllClosures();
	auto lr1table = lr1g.constructAnalyzeTable(lr1info);
	auto goodTable = Gramma::convertCorruptToStandardSimply(lr1table);
//	std::cout << lr1g.toString(lr1info) << std::endl
//				<<lr1g.toString(goodTable) << std::endl;

	//== output the lexical stream
	while(!myStream.eof())
	{
		std::cout << lr1g.gsyms.getString(myStream.get()) << " ";
	}
	std::cout << std::endl;
	myStream.goHead();//reset to head

	//== a default translator
//	DemoTranslator demoTrans(lr1g, lr1info, goodTable, lr1g.gsyms.get("$"), 0);
//	demoTrans.translate(myStream);

	//== define your own translator
	myStream.goHead();
	SimpleCppTranslator simpleTrans(lr1g,lr1info,goodTable,lr1g.gsyms.get("$"),0);
	simpleTrans.translate(myStream);


}
/**
 * 测试词法流是否正确工作
 * 测试结果:修复了某些错误
 */
void testLexicalStream()
{
	LexicalParser lp;
//	std::ifstream infile("examples/normal_test1.cpp");
	std::ifstream infile("examples/normal_test3.cpp");
	/**
	 * get method of istream retreives a raw character
	 * while operator>> returns formatted.
	 * So reading the document is the most important.
	 */
//	char ch;
//	while(!infile.eof())
//	{
//		ch=infile.get();
//
//		std::cout << ch ;
//	}
//	char ch;
//	while(!infile.eof())
//	{
//		infile >> ch;
//
//		std::cout << ch ;
//	}
//	std::cout <<std::endl<<"<END>"<<std::endl;

	auto stream=lp.parseWords(infile);
	for(auto &p:stream)
	{
		std::cout << "<"<<p.first<<", "<<lp.humanInfo[p.second]<<">"<<std::endl;
	}
}
/**
 * 测试 MutualMap<int,int>	是否可用
 */
void testMutualMapInt()
{
	x2::MutualMap<int,int> imap(-1,-1);
	imap.addT1(1, 2);
	imap.addT2(2,1);
	imap.addT1(2,1);
	std::cout << imap.toString() <<std::endl;
}
/**
 * 一个已经验证过的文法的翻译
 * 将
 *    A=B + C*(D+F)
 *   翻译成三地址码的形式
 *
 *   文法:
 *    E->(E)
 *    E->id
 *    E->E+E
 *    E->E*E
 *
 *   输入流
 *   	ID = ID + ID * ( ID + ID )
 *
 */
void testVerifiedGrammarTranslation()
{
	LR1Gramma lr1g({	 "E E + E",
						 "E E * E",
						 "E ( E )",
						 "E id"
					  },
			"E","$", "E'");
//	LR1Gramma lr1g({	 "S B B",
//						 "B a B",
//						 "B b",
//					  },
//			"S","$", "S'");

	std::cout << lr1g.gsyms.toString()<<std::endl;

	std::cout << lr1g.toString()<<std::endl;
	auto lr1info=lr1g.getAllClosures();
	std::cout << lr1g.toString(lr1info) << std::endl;
	auto table=lr1g.constructAnalyzeTable(lr1info);
	std::cout << lr1g.toString(table)<<std::endl;
	Gramma::LRAnalyzeTableType goodTable;
	//<9,+>,r
	//<9,*>,s
	//<10,+>,r
	//<10,*>,r
	//<15,+> r
	//<15,*> s
	//<16,+>,r
	//<16,*>,r
//	std::map<std::pair<int,int>,int> reserve={
//			{ {9,lr1g.gsyms.get("+")},LR1Gramma::ACTION_REDUCE},
//			{ {9,lr1g.gsyms.get("*")},LR1Gramma::ACTION_SHIFT_IN},
//			{ {10,lr1g.gsyms.get("+")},LR1Gramma::ACTION_REDUCE},
//			{ {10,lr1g.gsyms.get("*")},LR1Gramma::ACTION_REDUCE},
//			{ {15,lr1g.gsyms.get("+")},LR1Gramma::ACTION_REDUCE},
//			{ {15,lr1g.gsyms.get("*")},LR1Gramma::ACTION_SHIFT_IN},
//			{ {16,lr1g.gsyms.get("+")},LR1Gramma::ACTION_REDUCE},
//			{ {16,lr1g.gsyms.get("*")},LR1Gramma::ACTION_REDUCE},
//	};
//	for(auto &k:table)
//	{
//		if(k.second.size() > 1)
//		{
//			int thisaction=reserve[k.first];
//			for(auto it=k.second.begin();it!=k.second.end();it++)
//			{
//				if(std::get<2>(*it)==thisaction)
//				{
//					goodTable[k.first]=*it;
//					break;
//				}
//			}
//		}else{
//			goodTable[k.first]=*(k.second.begin());
//		}
//	}
	goodTable = Gramma::convertCorruptToStandardSimply(table);
	std::cout << lr1g.toString(goodTable)<<std::endl;
	DemoTranslator demotrans(lr1g,lr1info,goodTable,lr1g.gsyms.get("$"),0);
	/**
	 * ( id + id ) * id + id
	 */
	LexicalToGrammarStream input({
			lr1g.gsyms.getAdd("("),
			lr1g.gsyms.getAdd("id"),
			lr1g.gsyms.getAdd("+"),
			lr1g.gsyms.getAdd("id"),
			lr1g.gsyms.getAdd(")"),
			lr1g.gsyms.getAdd("*"),
			lr1g.gsyms.getAdd("id"),
			lr1g.gsyms.getAdd("+"),
			lr1g.gsyms.getAdd("id"),
			lr1g.gsyms.getAdd("$"),
	});
//	DemoTranslator::InputStreamType input={
//			lr1g.gsyms.getAdd("b"),
//			lr1g.gsyms.getAdd("a"),
//			lr1g.gsyms.getAdd("b"),
//			lr1g.gsyms.getAdd("$"),
//	};
//	std::cout << x2::toString(input) << std::endl;
	for(int i:input)
	{
		std::cout << lr1g.gsyms.getString(i)<< " ";
	}
	std::cout << std::endl;
	demotrans.translate(input);
}
/**
 * the idea is simple, when I add a grammar rule, I don't have to define it at first.
 *
 * 之所以选择int来映射语法符号，是为了在编译时拥有确定的可处理类型
 *
 * simplified.
 *
 */
void testGrammarDefineIsAdding()
{
	Gramma g({
		{"E",{"E","+","E"}},
		{"E",{"E","*","E"}},
		{"E",{"(","E",")"}},
		{"E",{"id"}},
		{"E",{"H","X"}},
		{"H",{"EMPTY"}},
	});

	//lr1g.setExtendedStart
	//lr1g.setEndSymbol
	//lr1g.setSymbolType

	/**
	 *  lr1g.addProduction("A",{"B","C","D"});
	 *
	 *  suppose B has appeared with a know type,
	 *  		C appears after A,
	 *  		D has never appeared
	 *  No matter what happened, firstly check the symbol table if it has A,whether it has or not,adding A as variable
	 *  adding B with known type
	 *  adding C as TERMINATOR type
	 *  adding D as TERMINATOR type
	 *
	 *  the process divided into 2 parts: the head and the body.
	 *
	 */
	std::cout << g.gsyms.toString()<<std::endl;
	std::cout << g.toString() << std::endl;


	LRGramma lrg(g,"E","$","E'");
	std::cout << lrg.gsyms.toString()<<std::endl;
	std::cout << lrg.toString() << std::endl;

	LR1Gramma	lr1g(lrg);
	std::cout << lr1g.gsyms.toString()<<std::endl;
	std::cout << lr1g.toString() << std::endl;


//	lr1g.addProduction("A",{"B","C","D"});
//
//
//	lr1g.gsyms.getAdd("whatis100");
//	std::cout << lr1g.gsyms.toString()<<std::endl;
//
//	std::cout << lr1g.toString()<<std::endl;
//	auto lr1info=lr1g.getAllClosures();
//	std::cout << lr1g.toString(lr1info) << std::endl;
//	auto table=lr1g.constructAnalyzeTable(lr1info);
//	std::cout << lr1g.toString(table)<<std::endl;
}
void testDFA()
{

	/**
	 * (\w+\d)\w+\d(\w+\d)\w+\d
	 *
	 * 生成DFA状态
	 *
	 * \w+  0
	 * \d   1
	 * \w+   2
	 * \d   3
	 *
	 */
}
void testStreamFA()
{
	//===========定义一个符号管理器
	//EMPTY & UNDEFINED
//	FiniteAutomataManager<char> faman((char)-1,(char)-2,
//			{ 'a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','v','w','x','y','z',
//			'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T','U','V','W','X','Y','Z',
//				'~','`','!','@','#','$','%','^','&','*','(',')','_','+','-','=','{','}',
//				'[',']','\\','|',':',';','"','\'',',','<','.','>','/','?',
//					' ','\t','\n','\r'},
//			/**
//			 * qStart also carries single character
//			 */
//			{"qStart","qID","qStr","qChar","qHexNum","qBinNum","qDecNum","qError","qSingleLineNote","qMultiLineNote"}
//
//	);
//
//	std::cout << faman.toString() << std::endl;


	//==========定义一个FA, 当然是选择原谅这个DFA了
	//FiniteAutomata<char> fa(faman,0,{1,1});
	DeterminasticFA<char> dfa((char)-1,(char)-2,"qStart",{"qStart"});
	/*
	 * 如果一个符号不存在，就将其加入
	 *
	 * 高级自动机提议： 可以使用通配符，但在内部仍然使用原始的单个状态对应;这样做空间复杂度增加
	 * 比如使用 undefined通配符，采取默认的动作
	 * 使用组通配符 -1,-2
	 *
	 */
	dfa.addGroup("ALL", { 'a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','v','w','x','y','z',
			'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T','U','V','W','X','Y','Z',
			'0','1','2','3','4','5','6','7','8','9',
			' ','~','`','!','@','#','$','%','^','&','*','(',')','_','+','-','=','{','}',
				'[',']','\\','|',':',';','"','\'',',','<','.','>','/','?',
					' ','\t','\n','\r'});
	dfa.addGroup("DECNUM", {'0','1','2','3','4','5','6','7','8','9',});
	dfa.addGroupUnion("HEXNUM", "DECNUM",{'a','b','c','d','e','f','A','B','C','D','E','F',});
	dfa.addGroup("BINNUM", {'0','1'});

	dfa.addGroup("LOWERALPHA", { 'a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z'});
	dfa.addGroup("UPPERALPHA", { 'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T','U','V','W','X','Y','Z',});
	dfa.addGroup("UNDERSCORE", {'_'});
	dfa.addGroup("NOMACRO-SPACE",{'\n','\t','\r',' '});
	dfa.addGroupUnion("WORD",{"DECNUM","LOWERALPHA","UPPERALPHA","UNDERSCORE"});
	dfa.addGroup("SINGLE",{
			'~','!','@','#','$','%','^','&','*','-','+','=','{','}','[',']','<','>','(',')',
	});
	dfa.addGroup("TRANSFER",{'t','a','n','b','n','r','v'});
//	dfa.addGroup("TRANSFER-HEX",{'x'});



	//=====start
	dfa.addTransitionByGroup("qStart","SINGLE", "qStart");
	//======特殊起始符号
	/**
	 *   凡是特殊符号必然至少是单个符号，也必须经过单个符号引导。
	 */
	dfa.addTransition("qStart", '/', "qNoteStart");
	dfa.addTransition("qStart", '\\', "qStartTransfer");
	dfa.addTransition("qStart",'#',"qMacros");
	dfa.addTransition("qStart", '"', "qString");
	dfa.addTransition("qStart",'\'',"qChar");
	dfa.addTransition("qStart", '0', "qNumber");
	dfa.addTransitionByGroup("qStart", "WORD", "qID");
	dfa.addTransitionByGroup("qStart", "NOMACRO-SPACE", "qStart");

	//=====id
	dfa.addTransitionByGroup("qID","ALL","qStart");
	dfa.addTransitionByGroup("qID", "WORD","qID");
	dfa.addTransition("qID", '\\', "qIDTransfer");

	//===transfer: \* \x** 的类型
	dfa.addTransitionByGroup("qStartTransfer","ALL","qID");
	dfa.addTransition("qStartTransfer",'x',"qStartTransferHex1");

	dfa.addTransitionByGroup("qStartTransferHex1","HEXNUM","qStartTransferHex2");
	dfa.addTransitionByGroup("qStartTransferHex2","HEXNUM","qID");
//	dfa.addTransitionByGroup("qStartTransfer","TRANSFER","q")

	//===string
	dfa.addTransitionUndefined("qString","ALL",{},"qStart");
	dfa.addTransition("qString", '\\', "qStringTransfer");
	dfa.addTransition("qString", '"',"qStart");

	//====char
	dfa.addTransitionByGroup("qChar", "ALL","qCharEnding");
	dfa.addTransition("qChar",'\\',"qCharTransfer");

	dfa.addTransition("qCharEnding",'\'',"qStart");
	//undefined then it is an error state

	//=====number
	dfa.addTransitionByGroup("qNumber","DECNUM", "qDecNum");
	dfa.addTransitionUndefined("qNumber", "ALL","DECNUM","qStart");
	dfa.addTransition("qNumber", 'x',"qHexNum");
	dfa.addTransition("qNumber", 'b', "qBinNum");

	dfa.addTransitionByGroup("qDecNum","DECNUM","qDecNum");
	dfa.addTransitionUndefined("qDecNum", "ALL", "DECNUM","qStart");

	//====note
	dfa.addTransition("qNoteStart", '/', "qSingleLineNote");
	dfa.addTransition("qNoteStart", '*', "qMultiLineNote");

	dfa.addTransition("qSingleLineNote", '\n', "qStart");

	dfa.addTransition("qMultiLineNote", '*', "qMultiLineEnding");
	dfa.addTransition("qMultiLineEnding", '/', "qStart");
	dfa.addTransitionUndefined("qMultiLineEnding", "ALL", {'/'}, "qMultiLineNote");

	//====error state, such as unrecognised character
	//==adding states that you want the automata immediately stops if such states are entered.
	dfa.addStop({"qTransferError"});

	//====go backs
	dfa.addGoback({
		{"qNumber","qStart"},
		{"qDecNum","qStart"},
		{"qHexNum","qStart"},
		{"qBinNum","qStart"},
		{"qID","qStart"}
	});

	dfa.next('a');
	std::cout << dfa.toString() << std::endl;
	std::cout << dfa.atEnd() << std::endl;
	dfa.reset();
	std::cout << dfa.atEnd() << std::endl;

	LexicalOutputStreamProcessor<char,std::string> myLexp(dfa);
	using PA = LexicalOutputStreamProcessor<char,std::string>::POSITION_ACTION;
	myLexp.addType({"qStart",'"'}, {"TYPE_STRING",PA::POSITION_NEW});
	myLexp.addType({"qStart",'\''}, {"TYPE_CHAR",PA::POSITION_NEW});
	myLexp.addType({"qStart", 'i'},{"TYPE_ID",PA::POSITION_NEWAPPEND});
	myLexp.addType({"qStart", 'm'},{"TYPE_ID",PA::POSITION_NEWAPPEND});
	myLexp.addType({"qID",'i'},{"",PA::POSITION_APPEND});
	myLexp.addType({"qStart",' '},{"",PA::POSITION_IGNORE});
	myLexp.addType({"qStart",'('},{"TYPE_SINGLE",PA::POSITION_NEWAPPEND});
	myLexp.addType({"qStart",')'},{"TYPE_SINGLE",PA::POSITION_NEWAPPEND});
	myLexp.addType({"qStart",'{'},{"TYPE_SINGLE",PA::POSITION_NEWAPPEND});
	myLexp.addType({"qStart",'}'},{"TYPE_SINGLE",PA::POSITION_NEWAPPEND});


	DeterminasticFA<char>::InputStreamType in={'i','i',' ','m','(',')','{','}'};//ii m(){}
	dfa.getMatch(in,myLexp);
	std::cout << x2::toString(myLexp.getCachedStream())<<std::endl;
}
void testMutualMap()
{
	MutualMap<int,std::string> mm(-1,"UNDEFINED");//default value of non-existing
	mm.addT1(0,"what is the fuck");
	std::cout << mm.getT1(0) << std::endl;
	mm.addReplaceT1(0,"hello world");
	std::cout << mm.getT2("what is the fuck")<<std::endl;
	std::cout << mm.getT1(2)<<std::endl;
	std::cout << mm.getT1(0)<<std::endl;
	std::cout << mm.getT2("hello world")<<std::endl;
}
void testReadFromFile()
{
	const std::string filepath="D:\\Pool\\eclipse-workspace\\compiler-debug\\examples\\grammar_c";
	Gramma g(filepath);
//	std::cout << g.toString()<<std::endl;
	g.gsyms.addTerm("$");

	LRGramma lrg(g,g.gsyms.get("START"),g.gsyms.get("$"),"S'");
	std::cout << lrg.toString()<<std::endl;
	auto lr0info = lrg.getAllClosures();
	std::cout << lrg.toString(lr0info)<<std::endl;

}
void testSimplifiedGrammar()
{
	const std::string filepath="D:\\Pool\\eclipse-workspace\\compiler-debug\\examples\\grammar_very_simplified.g";
	Gramma g(filepath);
	g.gsyms.addTerm("$");
	g.gsyms.addEmpty("empty");

	LR1Gramma lrg(g,"C","$","S'");
	std::cout << lrg.gsyms.toString() << std::endl;
	std::cout << lrg.toString()<<std::endl;

	std::cout << lrg.Gramma::toString(lrg.calcFirst()) << std::endl;

	auto lrinfo = lrg.getAllClosures();
	std::cout << lrg.toString(lrinfo)<<std::endl;
	auto table = lrg.constructAnalyzeTable(lrinfo);
	std::cout << lrg.toString(table) << std::endl;



}
//二义性文法
void testAmbigous()
{
	LR1Gramma lr1g({	 "E E + E",
						 "E E * E",
						 "E ( E )",
						 "E id"
					  },
			"E","$", "E'");

	std::cout << lr1g.gsyms.toString()<<std::endl;

	std::cout << lr1g.toString()<<std::endl;
	auto lr1info=lr1g.getAllClosures();
	std::cout << lr1g.toString(lr1info) << std::endl;
	auto table=lr1g.constructAnalyzeTable(lr1info);
	std::cout << lr1g.toString(table)<<std::endl;
}
void assignment_LR()
{
//	LRGramma lrg(
//			 {
//				  {GrammaSymbols::TYPE_EMPTY,"<EMPTY>"},//-1
//				  {GrammaSymbols::TYPE_VAR,"S"}, //0
//				  {GrammaSymbols::TYPE_VAR,"A"}, //1
//				  {GrammaSymbols::TYPE_VAR,"B"}, //2
//				  {GrammaSymbols::TYPE_TERM,"a"}, //3
//				  {GrammaSymbols::TYPE_TERM,"b"}, //4
//				  {GrammaSymbols::TYPE_TERM,"c"},//5
//				  {GrammaSymbols::TYPE_TERM,"d"},//6
//				  {GrammaSymbols::TYPE_TERM,"$"},//7
//								  },
//								  {
//										  {0,{4,1,0,2}},//S->bASB
//										  {0,{4,1}},//S->bA
//										  {1,{6,0,3}}, //A->dSa
//										  {1,{4}},//A->b
//										  {2,{5,1,3}},//B->cAa
//										  {2,{5}},//B->c
//								  },
//								  0,
//								  7,
//								  "S'"
//			);
//	LR1Gramma lrg({
//		{"S",{"b","A","S","B"}},
//		{"S",{"b","A"}},
//		{"A",{"d","S","a"}},
//		{"A",{"b"}},
//		{"B",{"c","A","a"}},
//		{"B",{"c"}},
//	},"S","$","S'");
	LR1Gramma lrg({ //末尾不能有空格
		{"S b A S B"},
		{"S b A"},
		{"A d S A"},
		{"A b"},
		{"B c A a"},
		{"B c"},
	},"S","$","S'");

	std::cout << "LR(0):"<<std::endl;
	std::cout << lrg.gsyms.toString() << std::endl;
	std::cout << lrg.toString() << std::endl;
	auto lr0info = lrg.getAllClosures();
	std::cout << lrg.toString(lr0info)<<std::endl;
	auto lr0first = lrg.calcFirst();
	auto lr0follow = lrg.calcFollow(lr0first, lrg.gsyms.get("S'"), lrg.gsyms.get("$"));
	std::cout << "FOLLOW of LR(0):"<<std::endl<<lrg.Gramma::toString(lr0follow)<<std::endl;

	LR1Gramma lr1g(lrg);
	std::cout << "LR(1):"<<std::endl;
	std::cout << lr1g.toString()<<std::endl;
	auto lr1info=lr1g.getAllClosures();
	std::cout << lr1g.toString(lr1info) << std::endl;
	auto table=lr1g.constructAnalyzeTable(lr1info);
	std::cout << lr1g.toString(table)<<std::endl;
}
void testLR1()
{
	LR1Gramma lr1g(
					  {
						  {GrammaSymbols::TYPE_EMPTY,"<EMPTY>"},//-1
						  {GrammaSymbols::TYPE_VAR,"S"}, //0
						  {GrammaSymbols::TYPE_VAR,"L"}, //1
						  {GrammaSymbols::TYPE_VAR,"R"}, //2
						  {GrammaSymbols::TYPE_TERM,"="}, //3
						  {GrammaSymbols::TYPE_TERM,"*"}, //4
						  {GrammaSymbols::TYPE_TERM,"i"},//5
						  {GrammaSymbols::TYPE_TERM,"$"},//6
					  },
					  {
							  {0,{1,3,2}},//S->L=R
							  {0,{2}}, //S->R
							  {1,{4,2}},//L->*R
							  {1,{5}},//L->i
							  {2,{1}},//R->L
					  },
					  0,
					  6,
					  "S'"
			);
	std::cout << lr1g.toString()<<std::endl;
	auto lr1info=lr1g.getAllClosures();
	std::cout << lr1g.toString(lr1info) << std::endl;
	auto table=lr1g.constructAnalyzeTable(lr1info);
	std::cout << lr1g.toString(table)<<std::endl;

}
void testGramma()
{
  Gramma g({
    {GrammaSymbols::TYPE_EMPTY,"<EMPTY>"},
    {GrammaSymbols::TYPE_TERM,"a"},
    {GrammaSymbols::TYPE_TERM,"b"},
    {GrammaSymbols::TYPE_VAR,"S"},//2,start符号
    {GrammaSymbols::TYPE_VAR,"B"},
    {GrammaSymbols::TYPE_VAR,"C"},
    {GrammaSymbols::TYPE_VAR,"D"}, //5
    {GrammaSymbols::TYPE_TERM,"c"},//6
    {GrammaSymbols::TYPE_VAR,"F"},//7
    {GrammaSymbols::TYPE_TERM,"<END>"},//8,end符号
  },
	 {
	     {2,{3,3}}, //S->BB
	     {3,{0,3}}, //B->aB
	     {3,{1}},   //B->b
	     {3,{-1}},    //B->EMPTY
	     {4,{5,2,3,0}}, //C->D S B a
	     {4,{5,2,3,0}}, //C->D S B a
	     {4,{5,2,3,1}}, //C->D S B b
	     {4,{5,2,2,1}}, //C->D S S b
	     {5,{5,0}},//D->D a
	     {5,{5,1}},//D->D b
	     {5,{2,3}},//D->S B
	     {5,{3,2}},//D->B S
//	     {5,{5}}, //D -> D
	     {7,{6}},//F->c
	     });

  //S->BB B->aB B->b  S->BBC C->BS , 这种文法可能引起空判定无限递归,but it will not now.
//  g.syms.push_back(GrammaSymbol(GrammaSymbol::SYM_EMPTY));

  //==========打印符号表
  std::cout<< g.gsyms.toString()<<std::endl;

  //========查找'S'的下标
  int n=g.gsyms.get("S");
  std::cout<<"S is at "<<n<<std::endl;

  //=======打印语法表
  std::cout<<g.toString()<<std::endl;

  //========确定符号是否包含空集
  std::cout << "S can be empty?" << g.canSymbolEmpty(2)<<std::endl;//can S empty?

  //========替换表达式
//  std::cout << "first replacement "<<std::endl;
//  g.replaceFirstProduction(2, 3);
//  std::cout << g.toString() <<std::endl;
//  std::cout << "second replacement "<<std::endl;
//  g.replaceFirstProduction(2, 3);
//  std::cout << g.toString() <<std::endl;
//  std::cout << "third replacement "<<std::endl;
//  g.replaceFirstProduction(2, 3);
//  std::cout << g.toString() <<std::endl;

  //======提取左因子的辅助函数 ： reduce
//  std::vector<int> subset={0,1,2};
//  int newvar=g.reduce(g.prods[5],g.gsyms.getString(5), 0, 2, subset);
//  std::cout << "reduce subset"<<std::endl;
//  std::cout << g.toString() <<std::endl;
//  std::cout << "reduce C' : [1 2] at (0,1)"<<std::endl;
//  subset={1,2};
//  g.reduce(g.prods[newvar],g.gsyms.getString(newvar), 0, 1, subset);
//  std::cout << g.toString() << std::endl;

//  //=======消除左因子
//  std::cout << "reduce left factors"<<std::endl;
//  std::vector<int> subset={0,1,2,3};
//  g.reduceLeftFactor(g.prods[5],g.gsyms.getString(5),0, subset);
//  std::cout << g.toString() << std::endl;

//  //=======消除直接左递归
//  std::cout << "reduce direct left recursive"<<std::endl;
//  g.reduceDirectLeftRecursive();
//  std::cout << g.toString() << std::endl;

  //=======消除所有左因子
  std::cout << "reduce all left factors"<<std::endl;
  g.reduceLeftFactor();
  std::cout << g.toString() << std::endl;

  //===========消除左递归
  std::cout << "reduce left recursive" << std::endl;
  g.reduceLeftRecursive();
  std::cout << g.toString() << std::endl;

  //===========消除重复产生式
  std::cout << "eliminate duplication" << std::endl;
  g.eliminateDuplication();
  std::cout << g.toString() << std::endl;

  //===========计算FIRST集
  std::cout << "calculating FIRST set" <<std::endl;
  Gramma::SetsType firstset=std::move(g.calcFirst());
  std::cout << g.toString(firstset) << std::endl;

  //==========计算FOLLOW集
  std::cout << "calculating FOLLOW set" << std::endl;
  Gramma::SetsType followset=std::move(g.calcFollow(firstset,2,8));
  std::string strfollwset;
  std::for_each(followset.begin(),followset.end(),[&strfollwset,&g](const Gramma::SetsType::value_type &it){
    strfollwset += g.gsyms.getString(it.first) + "{\n\t\t";
    std::for_each(it.second.begin(),it.second.end(), [&strfollwset,&g](int i){
      strfollwset += g.gsyms.getString(i) + ", ";
    });
    strfollwset += std::string("\n}\n");
  });
  std::cout << strfollwset << std::endl;

  //========拓广的LR文法
  LRGramma lrg(
	  {
		  {GrammaSymbols::TYPE_EMPTY,"<EMPTY>"},
		  {GrammaSymbols::TYPE_VAR,"E"}, //0
		  {GrammaSymbols::TYPE_VAR,"T"}, //1
		  {GrammaSymbols::TYPE_VAR,"F"}, //2
		  {GrammaSymbols::TYPE_TERM,"("}, //3
		  {GrammaSymbols::TYPE_TERM,")"}, //4
		  {GrammaSymbols::TYPE_TERM,"+"},//5
		  {GrammaSymbols::TYPE_TERM,"*"},//6
		  {GrammaSymbols::TYPE_TERM,"id"},//7
		  {GrammaSymbols::TYPE_TERM,"$"},//8
	  },
	  {
			  {0,{0,5,1}},
			  {0,{1}},
			  {1,{1,6,2}},
			  {1,{2}},
			  {2,{3,0,4}},
			  {2,{7}}
	  },
	  0,
	  8,
	  "E'"
  );
//  LRGramma lrg(
//	  {
//		  {GrammaSymbols::TYPE_EMPTY,"<EMPTY>"},
//		  {GrammaSymbols::TYPE_VAR,"S"}, //0
//		  {GrammaSymbols::TYPE_VAR,""}, //1
//		  {GrammaSymbols::TYPE_VAR,""}, //2
//		  {GrammaSymbols::TYPE_TERM,"0"}, //3
//		  {GrammaSymbols::TYPE_TERM,"1"}, //4
//		  {GrammaSymbols::TYPE_TERM,"+"},//5
//		  {GrammaSymbols::TYPE_TERM,"*"},//6
//		  {GrammaSymbols::TYPE_TERM,"id"}//7
//	  },
//	  {
//			  {0,{3,0,4}},
//			  {0,{3,4}},
//	  },
//	  0,
//	  "S'"
//  );

  std::cout << "LRGramma(this example can be found in Aho's dragon book,page 155)" << std::endl;
  std::cout << lrg.Gramma::toString() << std::endl;
//  lrg.setDotString("<DOT>");

  //=======辅助函数 : 产生某个符号的项目集
//  int iEd=lrg.gsyms.findSymbolIndex("E'");
//  auto itemone = std::make_tuple(iEd,0,0);//E' -> .E
//  LRGramma::ClosureType C=std::move(lrg.getClosure(itemone));
//  std::cout << "Closure of E'->.E " << std::endl;
//  std::cout << lrg.toString(C)<<std::endl;
//
//  //========辅助函数：产生一个项集的GOTO集
//  int iE=lrg.gsyms.findSymbolIndex("E");
//  LRGramma::ClosureType GOTO=std::move(lrg.getGoto(C, iE));
//  std::cout << "GOTO of CLOSURE({E'->.E})" << std::endl;
//  std::cout << lrg.toString(GOTO);
//  std::cout <<"inputing '+',getting next GOTO" << std::endl;
//  int iplus=lrg.gsyms.findSymbolIndex("+");
//  GOTO=std::move(lrg.getGoto(GOTO, iplus));
//  std::cout << lrg.toString(GOTO) << std::endl;

  //=========规范集项目族
  std::cout << "CLOSURES & GOTO" << std::endl;
  auto tup= lrg.getAllClosures();
  std::cout << lrg.toString(tup) << std::endl;


  //=========LR1 文法
  LR1Gramma lr1g({

			  {GrammaSymbols::TYPE_EMPTY,"<EMPTY>"},
			  {GrammaSymbols::TYPE_VAR,"S"}, //0
			  {GrammaSymbols::TYPE_VAR,"C"}, //1
			  {GrammaSymbols::TYPE_VAR,"F"}, //2
			  {GrammaSymbols::TYPE_TERM,"c"}, //3
			  {GrammaSymbols::TYPE_TERM,"d"}, //4
			  {GrammaSymbols::TYPE_TERM,"+"},//5
			  {GrammaSymbols::TYPE_TERM,"*"},//6
			  {GrammaSymbols::TYPE_TERM,"id"},//7
			  {GrammaSymbols::TYPE_TERM,"$"},//8
		  },
		  {
				  {0,{1,1}},//S->CC
				  {1,{3,1}}, //C->cC
				  {1,{4}},//C->d
		  },
		  0,
		  8,
		  "S'"
  );
  std::cout << "LR1 Gramma(this example can be found in Aho's dragon book,page 169)" << std::endl;
  std::cout << lr1g.toString() << std::endl;
  //=======辅助函数 : 产生某个符号的项目集
//  int i1Ed=lr1g.gsyms.findSymbolIndex("S'");
//  int i1End=lr1g.gsyms.findSymbolIndex("$");
//  int i1E=lr1g.gsyms.findSymbolIndex("S");
//  auto i1temone = std::make_tuple(i1Ed,0,0,i1End);//E' -> .E
//  LR1Gramma::ClosureType C=std::move(lr1g.getClosure(i1temone));
//  std::cout << "Closure of " <<lr1g.toString(i1temone) << std::endl;
//  std::cout << lr1g.toString(C)<<std::endl;
//
//
//  i1temone=std::make_tuple(i1E,0,0,i1End);
//  std::cout << "Closure of " << lr1g.toString(i1temone) << std::endl;//E->
//  std::cout << "<end" << std::endl;
//  C=lr1g.getClosure(i1temone);
//  std::cout << lr1g.toString(C) << std::endl;

  //=========规范集项目族
  auto lr1info=lr1g.getAllClosures();
  std::cout << lr1g.toString(lr1info) << std::endl;

  //===========do you agree with me?
  std::cout << "ENDED" << std::endl;




}
//#define A(i,j) i() k
//void testRecursiveMacro()
//{
//
//}
//void testNote()
//{
//  LexicalParser lp;
//  const char *wordbuf="//0x4e5fz*\n0x\\/";
//  size_t index=0;
//  int type;
//  std::string s;
////  lp.parseNote(wordbuf, index, strlen(wordbuf),s,type);
////  cout << LexicalParser::isInWord(' ')<<endl;
////  return;
//  auto hmap=LexicalParser::getHumanReadable();
//
//  cout << "("<<hmap[type]<<","<<s<<")"<<endl;
//}
//
//void testNumber()
//{
//  LexicalParser lp;
//  const char *wordbuf="0x4e5fz0x";
//  size_t index=0;
//  int type;
//  std::string s;
////  lp.parseNumber(wordbuf, index, strlen(wordbuf),s,type);
////  cout << LexicalParser::isInWord(' ')<<endl;
////  return;
//  auto hmap=LexicalParser::getHumanReadable();
//
//  cout << "("<<hmap[type]<<","<<s<<")"<<endl;
//}
//void testString()
//{
//  LexicalParser lp;
//  const char *wordbuf="\"eu\\x68asdf \" d";
//  size_t index=0;
//  int type;
//  std::string s;
////  lp.parseString(wordbuf, index, strlen(wordbuf),s,type);
////  cout << LexicalParser::isInWord(' ')<<endl;
////  return;
//  auto hmap=LexicalParser::getHumanReadable();
//
//  cout << "("<<hmap[type]<<","<<s<<")"<<endl;
//}
//
//void testParseWord()
//{
//  LexicalParser lp;
//  const char *wordbuf="xU_hand23ed ll0";
//  size_t index=0;
//  int type;
//  std::string s;
////  lp.parseId(wordbuf, index, strlen(wordbuf),s,type);
////  cout << LexicalParser::isInWord(' ')<<endl;
////  return;
//  auto hmap=LexicalParser::getHumanReadable();
//
//  cout << "("<<hmap[type]<<","<<s<<")"<<endl;
//}
//
//void testFile()
//{
//	LexicalParser lp;
////	const char* str="0x899aad";
////	size_t n=strlen(str);
////	lp.parseNumber(str,n);
////	const char* strbuf="\"ab\\b\\\nadafd\"";
////	size_t index=0;
////	size_t len=strlen(strbuf);
////	lp.parseString(strbuf, index, len);
////	const char *file="examples/normal_test1.cpp";
//	const char *file="examples/define_test1.cpp";
//	FILE *fp=fopen(file,"r+");
//	assert(fp!=NULL);
//	size_t readlen=fread(buffer,sizeof(char),BUFLEN,fp);
////	std::string t(buffer,readlen);
//	size_t index=0;
//	LexicalParser::WordStream stream;
//	LexicalParser::TransMap		humanInfo=LexicalParser::getHumanReadable();
//	lp.parseWords(stream,buffer, index, BUFLEN);
//	printf("As a result,we get stream :\n<start>\n");
//	LexicalParser::WordStream::iterator it=stream.begin(),itend=stream.end();
//	while(it!=itend)
//	{
////	    if(it->second!=LexicalParser::TYPE_SPACE && it->first.size()==1 && LexicalParser::isInSpace(it->first[0]))
////	      printf("%s ",it->first.c_str());
////	    else
//	      printf("%s ",humanInfo[it->second].c_str());
//	    it++;
//	}
//	printf("\n<end>\n");
//	printf("is that proper for you to do syntax analyze?\n");
//
//	//===Do define macro expansions
//	LL1Macro llm(stream,0);
//	llm.S();
//
//}
//
//
//
//
//

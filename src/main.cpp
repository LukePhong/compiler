#include <iostream>
#include <string.h>
#include <unistd.h>
#include <vector>
#include "Ast.h"
#include "Unit.h"
#include "SymbolTable.h"
using namespace std;

Ast ast;
Unit unit;
extern FILE *yyin;
extern FILE *yyout;

extern SymbolTable *identifiers;

int yyparse();

char outfile[256] = "a.out";
bool dump_tokens;
bool dump_ast;
bool dump_ir;

int main(int argc, char *argv[])
{
    int opt;
    while ((opt = getopt(argc, argv, "iato:")) != -1)
    {
        switch (opt)
        {
        case 'o':
            strcpy(outfile, optarg);
            break;
        case 'a':
            dump_ast = true;
            break;
        case 't':
            dump_tokens = true;
            break;
        case 'i':
            dump_ir = true;
            break;
        default:
            fprintf(stderr, "Usage: %s [-o outfile] infile\n", argv[0]);
            exit(EXIT_FAILURE);
            break;
        }
    }
    if (optind >= argc)
    {
        fprintf(stderr, "no input file\n");
        exit(EXIT_FAILURE);
    }
    if (!(yyin = fopen(argv[optind], "r")))
    {
        fprintf(stderr, "%s: No such file or directory\nno input file\n", argv[optind]);
        exit(EXIT_FAILURE);
    }
    if (!(yyout = fopen(outfile, "w")))
    {
        fprintf(stderr, "%s: fail to open output file\n", outfile);
        exit(EXIT_FAILURE);
    }

    //q13系统函数调用
    vector<Type*> tempParaType;
    auto funcType = new FunctionType(TypeSystem::intType, tempParaType);
    auto id = new IdentifierSymbolEntry(funcType, "getint", 0);
    identifiers->installFunc("getint", id);

    auto funcType1 = new FunctionType(TypeSystem::intType, tempParaType);
    auto id1 = new IdentifierSymbolEntry(funcType1, "getch", 0);
    identifiers->installFunc("getch", id1);

    auto funcType2 = new FunctionType(TypeSystem::intType, tempParaType);
    auto id2 = new IdentifierSymbolEntry(funcType2, "getfloat", 0);
    identifiers->installFunc("getfloat", id2);

    auto funcType3 = new FunctionType(TypeSystem::voidType, tempParaType);
    auto id3 = new IdentifierSymbolEntry(funcType3, "starttime", 0);
    identifiers->installFunc("starttime", id3);

    auto funcType4 = new FunctionType(TypeSystem::voidType, tempParaType);
    auto id4 = new IdentifierSymbolEntry(funcType4, "stoptime", 0);
    identifiers->installFunc("stoptime", id4);

    tempParaType.push_back(TypeSystem::arrayIntType);
    auto funcType5 = new FunctionType(TypeSystem::intType, tempParaType);
    auto id5 = new IdentifierSymbolEntry(funcType5, "getarray", 0);
    identifiers->installFunc("getarray", id5);

    tempParaType[0] = TypeSystem::arrayFloatType;
    auto funcType6 = new FunctionType(TypeSystem::intType, tempParaType);
    auto id6 = new IdentifierSymbolEntry(funcType6, "getfarray", 0);
    identifiers->installFunc("getfarray", id6);

    tempParaType[0] = TypeSystem::intType;
    auto funcType7 = new FunctionType(TypeSystem::voidType, tempParaType);
    auto id7 = new IdentifierSymbolEntry(funcType7, "putint", 0);
    identifiers->installFunc("putint", id7);

    auto funcType8 = new FunctionType(TypeSystem::voidType, tempParaType);
    auto id8 = new IdentifierSymbolEntry(funcType8, "putch", 0);
    identifiers->installFunc("putch", id8);

    tempParaType.push_back(TypeSystem::arrayIntType);
    auto funcType9 = new FunctionType(TypeSystem::voidType, tempParaType);
    auto id9 = new IdentifierSymbolEntry(funcType9, "putarray", 0);
    identifiers->installFunc("putarray", id9);

    tempParaType[1] = TypeSystem::arrayFloatType;
    auto funcType10 = new FunctionType(TypeSystem::voidType, tempParaType);
    auto id10 = new IdentifierSymbolEntry(funcType10, "putfarray", 0);
    identifiers->installFunc("putfarray", id10);

    vector<Type*>().swap(tempParaType);
    tempParaType.push_back(TypeSystem::floatType);
    auto funcType11 = new FunctionType(TypeSystem::voidType, tempParaType);
    auto id11 = new IdentifierSymbolEntry(funcType11, "putfloat", 0);
    identifiers->installFunc("putfloat", id11);
    //TODO: putf

    yyparse();
    if(dump_ast)
        ast.output();
    ast.typeCheck();
    ast.genCode(&unit);
    if(dump_ir)
        unit.output();
    return 0;
}

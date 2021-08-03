#include "parser.h"

const char *RBS_TOKENTYPE_NAMES[] = {
  "NullType",
  "pEOF",

  "pLPAREN",
  "pRPAREN",
  "pCOLON",
  "pCOLON2",
  "pLBRACKET",
  "pRBRACKET",
  "pLBRACE",
  "pRBRACE",
  "pHAT",
  "pARROW",
  "pFATARROW",
  "pCOMMA",
  "pBAR",
  "pAMP",
  "pSTAR",
  "pSTAR2",
  "pDOT",
  "pDOT3",
  "pMINUS",
  "pPLUS",
  "pSLASH",
  "pEQ2",
  "pEQ3",
  "pEQT",
  "pBANG",
  "pQUESTION",

  "kBOOL",            /* bool */
  "kBOT",             /* bot */
  "kCLASS",           /* class */
  "kFALSE",           /* kFALSE */
  "kINSTANCE",        /* instance */
  "kINTERFACE",       /* interface */
  "kNIL",             /* nil */
  "kSELF",            /* self */
  "kSINGLETON",       /* singleton */
  "kTOP",             /* top */
  "kTRUE",            /* true */
  "kVOID",            /* void */

  "tLIDENT",          /* Identifiers starting with lower case */
  "tUIDENT",          /* Identifiers starting with upper case */
  "tULIDENT",         /* Identifiers starting with `_` */
  "tGIDENT",          /* Identifiers starting with `$` */
  "tAIDENT",          /* Identifiers starting with `@` */
  "tA2IDENT",         /* Identifiers starting with `@@` */
  "tBANGIDENT",
  "tEQIDENT",
  "tQIDENT",          /* Quoted identifier */

  "tCOMMENT",
  "tLINECOMMENT",

  "tDQSTRING",        /* Double quoted string */
  "tSQSTRING",        /* Single quoted string */
  "tINTEGER",         /* Integer */
  "tSYMBOL",          /* Symbol */
};


VALUE RBSParser;
VALUE rbsparser_Keywords;

ID id_RBS;
VALUE sym_class;
VALUE sym_alias;
VALUE sym_interface;

VALUE RBS;
VALUE RBS_AST;
VALUE RBS_AST_Comment;

VALUE RBS_AST_Declarations;
VALUE RBS_AST_Declarations_Constant;
VALUE RBS_AST_Declarations_Global;

VALUE RBS_Location;
VALUE RBS_Namespace;
VALUE RBS_TypeName;

VALUE RBS_Types_Alias;
VALUE RBS_Types_Bases_Any;
VALUE RBS_Types_Bases_Bool;
VALUE RBS_Types_Bases_Bottom;
VALUE RBS_Types_Bases_Class;
VALUE RBS_Types_Bases_Instance;
VALUE RBS_Types_Bases_Nil;
VALUE RBS_Types_Bases_Self;
VALUE RBS_Types_Bases_Top;
VALUE RBS_Types_Bases_Void;
VALUE RBS_Types_Bases;
VALUE RBS_Types_Block;
VALUE RBS_Types_ClassInstance;
VALUE RBS_Types_ClassSingleton;
VALUE RBS_Types_Function_Param;
VALUE RBS_Types_Function;
VALUE RBS_Types_Interface;
VALUE RBS_Types_Intersection;
VALUE RBS_Types_Literal;
VALUE RBS_Types_Optional;
VALUE RBS_Types_Proc;
VALUE RBS_Types_Record;
VALUE RBS_Types_Tuple;
VALUE RBS_Types_Union;
VALUE RBS_Types_Variable;
VALUE RBS_Types;
VALUE RBS_MethodType;

static VALUE
rbsparser_parse_type(VALUE self, VALUE buffer, VALUE line, VALUE column)
{
  VALUE string = rb_funcall(buffer, rb_intern("content"), 0);
  lexstate lex = { string };
  lex.current.line = NUM2INT(line);
  lex.current.column = NUM2INT(column);
  lex.first_token_of_line = lex.current.column == 0;

  parserstate parser = { &lex };
  parser.buffer = buffer;
  parser.current_token = NullToken;
  parser.next_token = NullToken;
  parser.next_token2 = NullToken;

  parser_advance(&parser);
  parser_advance(&parser);
  return parse_type(&parser);
}

static VALUE
rbsparser_parse_method_type(VALUE self, VALUE buffer, VALUE line, VALUE column)
{
  VALUE string = rb_funcall(buffer, rb_intern("content"), 0);
  lexstate lex = { string };
  lex.current.line = NUM2INT(line);
  lex.current.column = NUM2INT(column);
  lex.first_token_of_line = lex.current.column == 0;

  parserstate parser = { &lex };
  parser.buffer = buffer;
  parser.current_token = NullToken;
  parser.next_token = NullToken;
  parser.next_token2 = NullToken;

  parser_advance(&parser);
  parser_advance(&parser);
  return parse_method_type(&parser);
}

static VALUE
rbsparser_parse_signature(VALUE self, VALUE buffer, VALUE line, VALUE column)
{
  VALUE string = rb_funcall(buffer, rb_intern("content"), 0);
  lexstate lex = { string };
  lex.current.line = NUM2INT(line);
  lex.current.column = NUM2INT(column);
  lex.first_token_of_line = lex.current.column == 0;

  parserstate parser = { &lex };
  parser.buffer = buffer;
  parser.current_token = NullToken;
  parser.next_token = NullToken;
  parser.next_token2 = NullToken;

  parser_advance(&parser);
  parser_advance(&parser);
  return parse_signature(&parser);
}
void
Init_parser(void)
{
  id_RBS = rb_intern_const("RBS");

  RBS = rb_const_get(rb_cObject, id_RBS);
  RBS_AST = rb_const_get(RBS, rb_intern("AST"));
  RBS_AST_Comment = rb_const_get(RBS_AST, rb_intern("Comment"));

  RBS_AST_Declarations = rb_const_get(RBS_AST, rb_intern("Declarations"));
  RBS_AST_Declarations_Constant = rb_const_get(RBS_AST_Declarations, rb_intern("Constant"));
  RBS_AST_Declarations_Global = rb_const_get(RBS_AST_Declarations, rb_intern("Global"));

  RBS_Location = rb_const_get(RBS, rb_intern("Location"));
  RBS_Namespace = rb_const_get(RBS, rb_intern("Namespace"));
  RBS_TypeName = rb_const_get(RBS, rb_intern("TypeName"));
  RBS_Types = rb_const_get(RBS, rb_intern("Types"));
  RBS_Types_Alias = rb_const_get(RBS_Types, rb_intern("Alias"));
  RBS_Types_Bases = rb_const_get(RBS_Types, rb_intern("Bases"));
  RBS_Types_Bases_Any = rb_const_get(RBS_Types_Bases, rb_intern("Any"));
  RBS_Types_Bases_Bool = rb_const_get(RBS_Types_Bases, rb_intern("Bool"));
  RBS_Types_Bases_Bottom = rb_const_get(RBS_Types_Bases, rb_intern("Bottom"));
  RBS_Types_Bases_Class = rb_const_get(RBS_Types_Bases, rb_intern("Class"));
  RBS_Types_Bases_Instance = rb_const_get(RBS_Types_Bases, rb_intern("Instance"));
  RBS_Types_Bases_Nil = rb_const_get(RBS_Types_Bases, rb_intern("Nil"));
  RBS_Types_Bases_Self = rb_const_get(RBS_Types_Bases, rb_intern("Self"));
  RBS_Types_Bases_Top = rb_const_get(RBS_Types_Bases, rb_intern("Top"));
  RBS_Types_Bases_Void = rb_const_get(RBS_Types_Bases, rb_intern("Void"));
  RBS_Types_Block = rb_const_get(RBS_Types, rb_intern("Block"));
  RBS_Types_ClassInstance = rb_const_get(RBS_Types, rb_intern("ClassInstance"));
  RBS_Types_ClassSingleton = rb_const_get(RBS_Types, rb_intern("ClassSingleton"));
  RBS_Types_Function = rb_const_get(RBS_Types, rb_intern("Function"));
  RBS_Types_Function_Param = rb_const_get(RBS_Types_Function, rb_intern("Param"));
  RBS_Types_Interface = rb_const_get(RBS_Types, rb_intern("Interface"));
  RBS_Types_Intersection = rb_const_get(RBS_Types, rb_intern("Intersection"));
  RBS_Types_Literal = rb_const_get(RBS_Types, rb_intern("Literal"));
  RBS_Types_Optional = rb_const_get(RBS_Types, rb_intern("Optional"));
  RBS_Types_Proc = rb_const_get(RBS_Types, rb_intern("Proc"));
  RBS_Types_Record = rb_const_get(RBS_Types, rb_intern("Record"));
  RBS_Types_Tuple = rb_const_get(RBS_Types, rb_intern("Tuple"));
  RBS_Types_Union = rb_const_get(RBS_Types, rb_intern("Union"));
  RBS_Types_Variable = rb_const_get(RBS_Types, rb_intern("Variable"));
  RBS_MethodType = rb_const_get(RBS, rb_intern("MethodType"));

  sym_class = ID2SYM(rb_intern_const("class"));
  sym_interface = ID2SYM(rb_intern_const("interface"));
  sym_alias = ID2SYM(rb_intern_const("alias"));

  RBSParser = rb_define_class_under(RBS, "Parser", rb_cObject);

  rbsparser_Keywords = rb_hash_new();
  rb_hash_aset(rbsparser_Keywords, rb_str_new_literal("bool"), INT2FIX(kBOOL));
  rb_hash_aset(rbsparser_Keywords, rb_str_new_literal("bot"), INT2FIX(kBOT));
  rb_hash_aset(rbsparser_Keywords, rb_str_new_literal("class"), INT2FIX(kCLASS));
  rb_hash_aset(rbsparser_Keywords, rb_str_new_literal("instance"), INT2FIX(kINSTANCE));
  rb_hash_aset(rbsparser_Keywords, rb_str_new_literal("interface"), INT2FIX(kINTERFACE));
  rb_hash_aset(rbsparser_Keywords, rb_str_new_literal("nil"), INT2FIX(kNIL));
  rb_hash_aset(rbsparser_Keywords, rb_str_new_literal("self"), INT2FIX(kSELF));
  rb_hash_aset(rbsparser_Keywords, rb_str_new_literal("singleton"), INT2FIX(kSINGLETON));
  rb_hash_aset(rbsparser_Keywords, rb_str_new_literal("top"), INT2FIX(kTOP));
  rb_hash_aset(rbsparser_Keywords, rb_str_new_literal("void"), INT2FIX(kVOID));

  rb_define_const(RBSParser, "KEYWORDS", rbsparser_Keywords);

  rb_define_singleton_method(RBSParser, "_parse_type", rbsparser_parse_type, 3);
  rb_define_singleton_method(RBSParser, "_parse_method_type", rbsparser_parse_method_type, 3);
  rb_define_singleton_method(RBSParser, "_parse_signature", rbsparser_parse_signature, 3);
}
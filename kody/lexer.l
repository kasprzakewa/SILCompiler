%{
    #include "parser.hpp"
    #include <iostream>
    #include <cstdlib>
    using namespace std;
    extern int yylex();
%}

%option yylineno
%option noyywrap

%%
"#".*       ;
[ \t\n]+    ;
[0-9]+      {   
                yylval.ival = std::atoi(yytext); return NUM; 
            }
[_a-z]+     {
                yylval.sval = new std::string(yytext);
                return PID;
            }

":="        {return ASSIGN;}
";"         {return ';';}
"("         {return '(';}
")"         {return ')';}
"["         {return '[';}
"]"         {return ']';}
":"         {return ':';}
","         {return ',';}
"+"         {return '+';}
"-"         {return '-';}   
"*"         {return '*';}
"/"         {return '/';}
"%"         {return '%';}
"="         {return '=';}
"!="        {return NEQ;}
">"         {return '>';}
"<"         {return '<';}
">="        {return GE;}
"<="       {return LE;}
"PROCEDURE" {return PROCEDURE;}
"IS"        {return IS;}
"BEGIN"     {return BEG;}
"END"       {return END;}
"PROGRAM"   {return PROGRAM;}
"IF"        {return IF;}
"THEN"      {return THEN;}
"ELSE"      {return ELSE;}
"ENDIF"     {return ENDIF;}
"WHILE"     {return WHILE;}
"DO"        {return DO;}
"ENDWHILE"  {return ENDWHILE;}
"REPEAT"    {return REPEAT;}
"UNTIL"     {return UNTIL;}
"FOR"       {return FOR;}
"FROM"      {return FROM;}
"TO"        {return TO;}
"ENDFOR"    {return ENDFOR;}
"DOWNTO"    {return DOWNTO;}
"READ"      {return READ;}
"WRITE"     {return WRITE;}
"T"         {return T;}
.
%%
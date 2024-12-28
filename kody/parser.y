%{
   #include <iostream>
   #include <vector>
   #include <string>
   using namespace std;

   int yywrap() { return 1; }
   int yylex();
   void yyerror(const char *s);

   int tempCounter = 0;
   std::vector<std::string> tac;

   void printTAC() {
         std::cout << "Three Address Code:" << std::endl;
      for (const auto& code : tac) {
         std::cout << code << std::endl;
      }
   }
%}

%define parse.error verbose

%code requires {
   #include <iostream>
   #include <vector>
   #include <string>
   using namespace std;
}

%union {
   int ival;
   std::string *sval;
}

%type <sval> value identifier expression condition command args_decl args proc_head proc_call declarations commands main procedures program_all

%token <ival> NUM
%token <sval> PID

%token ASSIGN NEQ GE LE
%token PROCEDURE IS BEG END PROGRAM IF THEN ELSE ENDIF WHILE ENDWHILE REPEAT UNTIL FOR FROM TO DO ENDFOR DOWNTO READ WRITE T

%%

program_all:  procedures main

procedures:  procedures PROCEDURE proc_head IS declarations BEG commands END { 
                  $$ = nullptr; 
               }
             | procedures PROCEDURE proc_head IS BEG commands END 
               { 
                  $$ = nullptr; 
               }
             | { 
                  $$ = nullptr; 
               }
             ;

main:        PROGRAM IS declarations BEG commands END { 
                  $$ = nullptr; 
               }
             | PROGRAM IS BEG commands END { 
                  $$ = nullptr; 
               }
             ;

commands:    commands command { 
                  $$ = nullptr; 
               }
             | command { 
                  $$ = nullptr; 
               }
             ;

command:     identifier ASSIGN expression';' {
                  tac.push_back(*$1 + " := " + *$3);
                  $$ = $1;
               }
             | IF condition THEN commands ELSE commands ENDIF { 
                  $$ = nullptr; 
               }
             | IF condition THEN commands ENDIF { 
                  $$ = nullptr; 
               }
             | WHILE condition DO commands ENDWHILE { 
                  $$ = nullptr; 
               }
             | REPEAT commands UNTIL condition';' { 
                  $$ = nullptr; 
               }
             | FOR PID FROM value TO value DO commands ENDFOR { 
                  $$ = nullptr; 
               }
             | FOR PID FROM value DOWNTO value DO commands ENDFOR { 
                  $$ = nullptr; 
               }
             | proc_call';' { 
                  $$ = nullptr; 
               }
             | READ identifier';' { 
                  $$ = nullptr; 
               }
             | WRITE value';' { 
                  $$ = nullptr; 
               }
             ;

proc_head:   PID '(' args_decl ')' { 
                  $$ = nullptr; 
               }
            ;

proc_call:   PID '(' args ')' { 
                  $$ = nullptr; 
               }
            ;

declarations:declarations',' PID { $$ = $3; }
             | declarations',' PID'['NUM':'NUM']' { 
                  $$ = nullptr; 
               }
             | PID { $$ = $1; }
             | PID'['NUM':'NUM']' { 
                  $$ = nullptr; 
               } 
             ;

args_decl:   args_decl',' PID { 
                  $$ = nullptr; 
               }
             | args_decl',' T PID { 
                  $$ = nullptr; 
               }
             | PID { 
                  $$ = nullptr; 
               }
             | T PID { 
                  $$ = nullptr; 
               }
             ;

args:        args',' PID { 
                  $$ = nullptr; 
               }
             | PID { 
                  $$ = nullptr; 
               }
             ;

expression:  value {
                  $$ = $1;
               }
             | value '+' value {
                  std::string *temp = new std::string("t" + std::to_string(tempCounter++));
                  tac.push_back(*temp + " := " + *$1 + " + " + *$3);
                  $$ = temp;
               }
             | value '-' value {
                  std::string *temp = new std::string("t" + std::to_string(tempCounter++));  
                  tac.push_back(*temp + " := " + *$1 + " - " + *$3);
                  $$ = temp;
               }
             | value '*' value {
                  std::string *temp = new std::string("t" + std::to_string(tempCounter++));
                  tac.push_back(*temp + " := " + *$1 + " * " + *$3);
                  $$ = temp;
               }
             | value '/' value {
                  std::string *temp = new std::string("t" + std::to_string(tempCounter++));
                  tac.push_back(*temp + " := " + *$1 + " / " + *$3);
                  $$ = temp;
               }
             | value '%' value {
                  std::string *temp = new std::string("t" + std::to_string(tempCounter++));
                  tac.push_back(*temp + " := " + *$1 + " % " + *$3);
                  $$ = temp;
               }
               ;

condition:   value '=' value { 
                  $$ = nullptr; 
               }
             | value NEQ value { 
                  $$ = nullptr; 
               }
             | value '>' value { 
                  $$ = nullptr; 
               }
             | value '<' value { 
                  $$ = nullptr; 
               }
             | value GE value { 
                  $$ = nullptr; 
               }
             | value LE value { 
                  $$ = nullptr; 
               }
               ;

value:       NUM {
                  $$ = new std::string(std::to_string($1));
               }
             | identifier {
                  $$ = $1;
               }
               ;

identifier:  PID {
                  $$ = $1;
               } 
             | PID'['PID']' { 
                  $$ = nullptr; 
               }
             | PID'['NUM']' { 
                  $$ = nullptr; 
               }
               ;
%%

int main() {
   yyparse();
   printTAC();
   return 0;
}

void yyerror(const char *s) {
   cerr << s << endl;
}

%{
   #include <iostream>
   #include <vector>
   #include <string>
   #include "symbol_table.hpp"
   
   using namespace std;

   int yywrap() { return 1; }
   int yylex();
   void yyerror(const char *s);

   int tempCounter = 0;
   vector<string> tac;

   void printTAC() {
      cout << "Three Address Code:" << endl;
      for (const auto& code : tac) {
         cout << code << endl;
      }
   }

   bool isNumber(const string& s) {
      return !s.empty() && all_of(s.begin(), s.end(), ::isdigit);
   }

   void initialize_temp(int id, string value1, string value3) {
      if (isNumber(value1)) {

         if (isNumber(value3)) {

            initialize_symbol(id);

         } else {

            int id3 = find_symbol(value3);

            if (id3 != -1) {

               if (is_initialized(id3)) {
                  initialize_symbol(id);
               } else {
                  yyerror(("Variable " + value3 + " not initialized").c_str());
               }

            } else {
               yyerror(("Variable " + value3 + " not declared").c_str());
            }
         }

      } else if (isNumber(value3)) {

         int id1 = find_symbol(value1);
         
         if (id1 != -1) {

            if (is_initialized(id1)) {
               initialize_symbol(id);
            } else {
               yyerror(("Variable " + value1 + " not initialized").c_str());
            }

         } else {
            yyerror(("Variable " + value1 + " not declared").c_str());
         }
         
      } else {

         int id1 = find_symbol(value1);
         int id3 = find_symbol(value3);

         if (id1 != -1 && id3 != -1) {

            if (is_initialized(id1) && is_initialized(id3)) {
               initialize_symbol(id);
            } else {
               if (!is_initialized(id1)) {
                  yyerror(("Variable " + value1 + " not initialized").c_str());
               } else {
                  yyerror(("Variable " + value3 + " not initialized").c_str());
               }
            }

         } else {
            if (id1 == -1) {
               yyerror(("Variable " + value1 + " not declared").c_str());
            } else {
               yyerror(("Variable " + value3 + " not declared").c_str());
            }
         }   
      }
   }

   void initialize_assign(int id,  string value, string value2) {
      if (id != -1) {
         if (isNumber(value2)) {
            initialize_symbol(id);
         } else {
            int id2 = find_symbol(value2);

            if (id2 != -1) {
               if (is_initialized(id2)) {
                  initialize_symbol(id);
               } else {
                  yyerror(("Variable " + value2 + " not initialized").c_str());
               }
            } else {
               yyerror(("Variable " + value2 + " not declared").c_str());
            }
         }
      } else {
         yyerror(("Variable " + value + " not declared").c_str());
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
   string *sval;
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
                  int id = find_symbol(*$1);
                  initialize_assign(id, *$1, *$3);
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

declarations:declarations',' PID { 
                  $$ = $3;
                  if (find_symbol(*$3) == -1) {
                     add_symbol(*$3);
                  } else {
                     yyerror(("Variable " + *$3 + " already declared").c_str());
                  }
               }
             | declarations',' PID'['NUM':'NUM']' { 
                  $$ = nullptr; 
               }
             | PID { 
                  $$ = $1;
                  if (find_symbol(*$1) == -1) {
                     add_symbol(*$1);
                  } else {
                     yyerror(("Variable " + *$1 + " already declared").c_str());
                  }
               }
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
                  string *temp = new string("t" + to_string(tempCounter++));
                  tac.push_back(*temp + " := " + *$1 + " + " + *$3);
                  add_symbol(*temp);
                  initialize_temp(last_symbol - 1, *$1, *$3);
                  $$ = temp;
               }
             | value '-' value {
                  string *temp = new string("t" + to_string(tempCounter++));  
                  tac.push_back(*temp + " := " + *$1 + " - " + *$3);
                  add_symbol(*temp);
                  initialize_temp(last_symbol - 1, *$1, *$3);
                  $$ = temp;
               }
             | value '*' value {
                  string *temp = new string("t" + to_string(tempCounter++));
                  tac.push_back(*temp + " := " + *$1 + " * " + *$3);
                  add_symbol(*temp);
                  initialize_temp(last_symbol - 1, *$1, *$3);
                  $$ = temp;
               }
             | value '/' value {
                  string *temp = new string("t" + to_string(tempCounter++));
                  tac.push_back(*temp + " := " + *$1 + " / " + *$3);
                  add_symbol(*temp);
                  initialize_temp(last_symbol - 1, *$1, *$3);
                  $$ = temp;
               }
             | value '%' value {
                  string *temp = new string("t" + to_string(tempCounter++));
                  tac.push_back(*temp + " := " + *$1 + " % " + *$3);
                  add_symbol(*temp);
                  initialize_temp(last_symbol - 1, *$1, *$3);
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
                  $$ = new string(to_string($1));
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
   cout << endl;
   print_symbol_table();
   cout << endl;
   printTAC();
   return 0;
}

void yyerror(const char *s) {
   cerr << s << endl;
}

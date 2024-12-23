%{
   #include <iostream>
   using namespace std;

   int yywrap() { return 1; }
   int yylex();
   void yyerror(const char *s);
%}

%define parse.error verbose

%union {
   int ival;
   char *sval;
}

%token <ival> NUM
%token <sval> PID

%token ASSIGN NEQ GE LE
%token PROCEDURE IS BEG END PROGRAM IF THEN ELSE ENDIF WHILE ENDWHILE REPEAT UNTIL FOR FROM TO DO ENDFOR DOWNTO READ WRITE T

%%

program_all:  procedures main

procedures:  procedures PROCEDURE proc_head IS declarations BEG commands END
             | procedures PROCEDURE proc_head IS BEG commands END
             | 

main:        PROGRAM IS declarations BEG commands END
             | PROGRAM IS BEG commands END

commands:    commands command
             | command

command:     identifier ASSIGN expression';'
             | IF condition THEN commands ELSE commands ENDIF
             | IF condition THEN commands ENDIF
             | WHILE condition DO commands ENDWHILE
             | REPEAT commands UNTIL condition';'
             | FOR PID FROM value TO value DO commands ENDFOR
             | FOR PID FROM value DOWNTO value DO commands ENDFOR
             | proc_call';'
             | READ identifier';'
             | WRITE value';'

proc_head:   PID '(' args_decl ')'

proc_call:   PID '(' args ')'

declarations:declarations',' PID
             | declarations',' PID'['NUM':'NUM']'
             | PID
             | PID'['NUM':'NUM']'

args_decl:   args_decl',' PID
             | args_decl',' T PID
             | PID
             | T PID

args:        args',' PID
             | PID

expression:  value
             | value '+' value
             | value '-' value
             | value '*' value
             | value '/' value
             | value '%' value

condition:   value '=' value
             | value NEQ value
             | value '>' value
             | value '<' value
             | value GE value
             | value LE value

value:       NUM
             | identifier

identifier:  PID
             | PID'['PID']'
             | PID'['NUM']'
%%

int main() {
   yyparse();
   return 0;
}

void yyerror(const char *s) {
   cerr << s << endl;
}

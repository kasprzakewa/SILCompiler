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
// {cout << "1" << endl;}

procedures:  procedures PROCEDURE proc_head IS declarations BEG commands END {cout << "2: PROCEDURE" << endl;}
             | procedures PROCEDURE proc_head IS BEG commands END {cout << "3: PROCEDURE" << endl;}
             | 
            //  {cout << 4 << endl;}

main:        PROGRAM IS declarations BEG commands END {cout << "5: PROGRAM" << endl;}
             | PROGRAM IS BEG commands END {cout << "6: PROGRAM" << endl;}

commands:    commands command 
// {cout << "7" << endl;}
             | command 
            //  {cout << "8" << endl;}

command:     identifier ASSIGN expression';' {cout << "9: :=" << endl;}
             | IF condition THEN commands ELSE commands ENDIF {cout << "10: IF THEN ELSE" << endl;}
             | IF condition THEN commands ENDIF {cout << "11: IF THEN" << endl;}
             | WHILE condition DO commands ENDWHILE 
            //  {cout << "12: WHILE condition DO commands ENDWHILE" << endl;}
             | REPEAT commands UNTIL condition';' 
            //  {cout << "13: REPEAT commands UNTIL condition';'" << endl;}
             | FOR PID FROM value TO value DO commands ENDFOR 
            //  {cout << "14: FOR PID FROM value TO value DO commands ENDFOR" << endl;}
             | FOR PID FROM value DOWNTO value DO commands ENDFOR 
            //  {cout << "15: FOR PID FROM value DOWNTO value DO commands ENDFOR" << endl;}
             | proc_call';' 
            //  {cout << "16: proc_call';'" << endl;}
             | READ identifier';' {cout << "17: READ" << endl;}
             | WRITE value';' {cout << "18: WRITE" << endl;}

proc_head:   PID '(' args_decl ')' 
// {cout << "19: PID '(' args_decl ')'" << endl;}
proc_call:   PID '(' args ')' 
// {cout << "20: PID '(' args ')'" << endl;}

declarations:declarations',' PID  {cout << "21: PID(" << $3 << ")" << endl;}
             | declarations',' PID'['NUM':'NUM']' 
            //  {cout << "22: declarations',' PID'['NUM':'NUM']'" << endl;}
             | PID {cout << "23: PID(" << $1 << ")" << endl;}
             | PID'['NUM':'NUM']'   
            //  {cout << "24: PID'['NUM':'NUM']'" << endl;}

args_decl:   args_decl',' PID 
// {cout << "25: args_decl',' PID" << endl;}
             | args_decl',' T PID 
            //  {cout << "26: args_decl',' T PID" << endl;}
             | PID 
            //  {cout << "27: PID" << endl;}
             | T PID 
            //  {cout << "28: T PID" << endl;}

args:        args',' PID 
// {cout << "29: args',' PID" << endl;}
             | PID 
            //  {cout << "30: PID" << endl;}

expression:  value 
// {cout << "31" << endl;}
             | value '+' value {cout << "32: +" << endl;}
             | value '-' value {cout << "33: -" << endl;}
             | value '*' value {cout << "34: *" << endl;}
             | value '/' value {cout << "35: /" << endl;}
             | value '%' value {cout << "36: %" << endl;}

condition:   value '=' value {cout << "37: =" << endl;}
             | value NEQ value {cout << "38: !=" << endl;}
             | value '>' value {cout << "39: >" << endl;}
             | value '<' value {cout << "40: <" << endl;}
             | value GE value {cout << "41: >=" << endl;}
             | value LE value {cout << "42: <=" << endl;}

value:       NUM {cout <<"43: NUM(" << $1 << ")" << endl;}
             | identifier 
            //  {cout << "44" << endl;}

identifier:  PID {cout << "45: PID(" << $1 << ")" << endl;}
             | PID'['PID']' 
            //  {cout << "46: PID'['PID']'" << endl;}
             | PID'['NUM']' 
            //  {cout << "47: PID'['NUM']'" << endl;}
%%

int main() {
   yyparse();
   return 0;
}

void yyerror(const char *s) {
   cerr << s << endl;
}

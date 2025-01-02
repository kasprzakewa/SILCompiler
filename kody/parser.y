%{
   extern int yylineno;

   int yywrap() { return 1; }
   int yylex();
   void yyerror(const char *s);
%}

%locations
%define parse.error verbose

%code requires {
   #include <iostream>
   #include <vector>
   #include <string>
   #include "nodes.hpp"
   using namespace std;
}

%union {
   int ival;
   string *sval;
   MainNode* main;
   CommandsNode* commands;
   CommandNode* command;
   ConditionNode* condition;
   ValueNode* value;
}

%type <sval> args_decl args proc_head proc_call procedures program_all declarations
%type <main> main
%type <commands> commands
%type <command> command expression
%type <condition> condition
%type <value> value identifier 
%token <ival> NUM
%token <sval> PID

%token ASSIGN NEQ GE LE
%token PROCEDURE IS BEG END PROGRAM IF THEN ELSE ENDIF WHILE ENDWHILE REPEAT UNTIL FOR FROM TO DO ENDFOR DOWNTO READ WRITE T

%%

program_all:  procedures main {
                  $2->translate();
               }

procedures:  procedures PROCEDURE proc_head IS declarations BEG commands END { 
                  $$ = nullptr; 
               }
             | procedures PROCEDURE proc_head IS BEG commands END { 
                  $$ = nullptr; 
               }
             | { 
                  $$ = nullptr; 
               }
             ;

main:        PROGRAM IS declarations BEG commands END { 
                  $$ = new MainNode($5, true); 
               }
             | PROGRAM IS BEG commands END { 
                  $$ = new MainNode($4, false); 
               }
             ;

commands:    commands command {
                  $$ = $1;
                  $$->add_child($2);
               }
             | command {
                  $$ = new CommandsNode();
                  $$->add_child($1); 
               }
             ;

command:     identifier ASSIGN expression ';' {
                  $1->initialize();
                  $$ = $3;
                  $$->set_x($1);
               }
             | IF condition THEN commands ELSE commands ENDIF {
                  $$ = new IfElseNode($2, $4, $6); 
               }
             | IF condition THEN commands ENDIF {
                  $$ = new IfNode($2, $4);
               }
             | WHILE condition DO commands ENDWHILE { 
                  $$ = new WhileNode($2, $4);
               }
             | REPEAT commands UNTIL condition ';' { 
                  $$ = new RepeatNode($4, $2); 
               }
             | FOR PID FROM value TO value DO commands ENDFOR {
                  ValueNode* one = find_node("1");
                  ValueNode* i = find_node(*$2);
                  $$ = new ForNode(i, $4, $6, one, $8, "JPOS"); 
               }
             | FOR PID FROM value DOWNTO value DO commands ENDFOR {
                  ValueNode* one = find_node("1");
                  ValueNode* i = find_node(*$2);
                  $$ = new ForNode(i, $4, $6, one, $8, "JNEG");
               }
             | proc_call ';' { 
                  $$ = nullptr; 
               }
             | READ identifier ';' { 
                  $2->initialize();
                  $$ = new IONode("GET", $2);
               }
             | WRITE value ';' { 
                  $$ = new IONode("PUT", $2);
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

declarations:declarations ',' PID {
                  ValueNode* val = new ValueNode(*$3, false, 0);
                  add_to_memory(val);
               }
             | PID {
                  ValueNode* val = new ValueNode(*$1, false, 0);
                  add_to_memory(val);
               }
             ;

args_decl:   args_decl ',' PID { 
                  $$ = nullptr; 
               }
             | PID { 
                  $$ = nullptr; 
               }
             ;

args:        args ',' PID { 
                  $$ = nullptr; 
               }
             | PID { 
                  $$ = nullptr; 
               }
             ;

expression:  value {
                  $$ = new AssignNode("ASSIGN", nullptr, $1, nullptr);
               }
             | value '+' value {
                  $$ = new AssignNode("ADD", nullptr, $1, $3);
               }
             | value '-' value {
                  $$ = new AssignNode("SUB", nullptr, $1, $3);
               }
             | value '*' value {
                  $$ = new AssignNode("MUL", nullptr, $1, $3);
               }
             | value '/' value {
                  $$ = new AssignNode("DIV", nullptr, $1, $3);
               }
             | value '%' value {
                  $$ = new AssignNode("MOD", nullptr, $1, $3);
               }
               ;

condition:   value '=' value { 
                  $$ = new ConditionNode("JZERO", $1, $3, false);
               }
             | value NEQ value { 
                  $$ = new ConditionNode("JZERO", $1, $3, true); 
               }
             | value '>' value { 
                  $$ = new ConditionNode("JPOS", $1, $3, false);
               }
             | value '<' value {
                  $$ = new ConditionNode("JNEG", $1, $3, false);
               }
             | value GE value { 
                  $$ = new ConditionNode("JNEG", $1, $3, true);
               }
             | value LE value { 
                  $$ = new ConditionNode("JPOS", $1, $3, true);
               }
               ;

value:       NUM { 
                  $$ = find_node(to_string($1));

                  if ($$ == nullptr) {
                     $$ = new ValueNode(to_string($1), true, 2);
                     add_to_memory($$);
                  }
               }
             | identifier {
                  $$ = $1;
               }
               ;

identifier:  PID {
                  $$ = find_node(*$1);
               }
             ;
%%

int main() {
   yyparse();
   print_memory();
   cout << endl;
   print_assembly();
   return 0;
}

void yyerror(const char *str)
{
   fprintf(stderr, "\033[0;31mERROR AT LINE %d: %s\033[0m\n", yylineno - 1, str);
   exit(EXIT_FAILURE);
}

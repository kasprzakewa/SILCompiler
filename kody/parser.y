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
   #include "ast_nodes.hpp"
   using namespace std;
}

%union {
   int ival;
   string *sval;
   ASTNode *node;
   CommandsNode *commands;
   CommandNode *command;
   DeclarationsNode *declarations;
   ExpressionNode *expression;
   ConditionNode *condition;
   ValueNode *value;
   IdentifierNode *identifier;
}

%type <sval> args_decl args proc_head proc_call procedures program_all
%type <node> main
%type <commands> commands
%type <command> command
%type <declarations> declarations
%type <expression> expression
%type <condition> condition
%type <value> value
%type <identifier> identifier 
%token <ival> NUM
%token <sval> PID

%token ASSIGN NEQ GE LE
%token PROCEDURE IS BEG END PROGRAM IF THEN ELSE ENDIF WHILE ENDWHILE REPEAT UNTIL FOR FROM TO DO ENDFOR DOWNTO READ WRITE T

%%

program_all:  procedures main {
                  $2->print();
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
                  $$ = new MainDecNode($3, $5); 
               }
             | PROGRAM IS BEG commands END { 
                  $$ = new MainNode($4); 
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
                  if (!$3->is_initialized().first) {
                     yyerror(("Variable " + $3->is_initialized().second + " not initialized").c_str());
                  }
                  initialize_node($1);
                  $$ = new AssignNode($1, $3);
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
                  $$ = new RepeatUntilNode($2, $4); 
               }
             | FOR PID FROM value TO value DO commands ENDFOR { 
                  $$ = new ForToNode(new IdentifierNode(*$2, true), $4, $6, $8); 
               }
             | FOR PID FROM value DOWNTO value DO commands ENDFOR { 
                  $$ = new ForDownToNode(new IdentifierNode(*$2, true), $4, $6, $8);
               }
             | proc_call ';' { 
                  $$ = nullptr; 
               }
             | READ identifier ';' { 
                  $$ = new ReadNode($2);
                  initialize_node($2);
               }
             | WRITE value ';' { 
                  $$ = new WriteNode($2);
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
                  $$ = $1;
                  IdentifierNode* id = new IdentifierNode(*$3, false);
                  if (find_node(id->get_name()) == nullptr) {
                     add_node(id);
                     $$->add_child(id);
                  } else {
                     yyerror(("Variable " + id->get_name() + " already declared").c_str());
                  }
               }
             | PID {
                  $$ = new DeclarationsNode();
                  IdentifierNode* id = new IdentifierNode(*$1, false);
                  if (find_node(id->get_name()) == nullptr) {
                     add_node(id);
                     $$->add_child(id);
                  } else {
                     yyerror(("Variable " + id->get_name() + " already declared").c_str());
                  }
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
                  $$ = new ExpressionNode($1);
               }
             | value '+' value {
                  $$ = new ExpressionNode($1, $3, "+");
               }
             | value '-' value {
                  $$ = new ExpressionNode($1, $3, "-");
               }
             | value '*' value {
                  $$ = new ExpressionNode($1, $3, "*");
               }
             | value '/' value {
                  $$ = new ExpressionNode($1, $3, "/");
               }
             | value '%' value {
                  $$ = new ExpressionNode($1, $3, "%");
               }
               ;

condition:   value '=' value { 
                  $$ = new ConditionNode($1, $3, "=");
               }
             | value NEQ value { 
                  $$ = new ConditionNode($1, $3, "!="); 
               }
             | value '>' value { 
                  $$ = new ConditionNode($1, $3, ">");
               }
             | value '<' value {
                  $$ = new ConditionNode($1, $3, "<");
               }
             | value GE value { 
                  $$ = new ConditionNode($1, $3, ">=");
               }
             | value LE value { 
                  $$ = new ConditionNode($1, $3, "<=");
               }
               ;

value:       NUM { 
                  $$ = new ValueNode(to_string($1));
               }
             | identifier {
                  if ($1 == nullptr) {
                     $$ = nullptr;
                  } else {
                     $$ = new ValueNode($1);
                  }
               }
               ;

identifier:  PID {
                  IdentifierNode* id = find_node(*$1);
                  if (id == nullptr) {
                     yyerror(("Variable " + *$1 + " not declared").c_str());
                  } else {
                     $$ = id;
                  }
               }
             ;
%%

int main() {
   yyparse();
   cout << endl;
   print_tac();
   return 0;
}

void yyerror(const char *str)
{
   fprintf(stderr, "\033[0;31mERROR AT LINE %d: %s\033[0m\n", yylineno - 1, str);
   exit(EXIT_FAILURE);
}

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
   long long ival;
   string *sval;
   MainNode* main;
   CommandsNode* commands;
   CommandNode* command;
   ConditionNode* condition;
   ValueNode* value;
   ProceduresNode* procedures;
   ProgramNode* program;
}

%type <sval> args_decl args proc_head proc_call declarations
%type <main> main
%type <commands> commands
%type <command> command expression
%type <condition> condition
%type <value> value identifier number
%type <procedures> procedures
%type <program> program_all
%token <ival> NUM
%token <sval> PID

%token ASSIGN ADD SUB MUL DIV MOD EQ NEQ GT LT GE LE
%token PROCEDURE IS BEG END PROGRAM IF THEN ELSE ENDIF WHILE ENDWHILE REPEAT UNTIL FOR FROM TO DO ENDFOR DOWNTO READ WRITE T
%left ADD SUB
%left MUL DIV MOD
%left EQ NEQ GT LT GE LE
%right UMINUS

%%

program_all:  procedures main {
                  $$ = new ProgramNode($1, $2);
                  $$->analyze(); 
                  $$->translate();
               }

procedures:  procedures PROCEDURE proc_head IS declarations BEG commands END { 
                  $$ = nullptr; 
               }
             | procedures PROCEDURE proc_head IS BEG commands END { 
                  $$ = $1;
                  $$->add_procedure(new ProcedureNode(*$3, $6));
               }
             | { 
                  $$ = new ProceduresNode(); 
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
                  if (dynamic_cast<ArrayElem*>($1) != nullptr) {
                     cout << "ARRAY ELEMENT at 1" << endl;
                  }
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
                  // cout << $<value>4->name << endl;
                  // $<commands>8->translate();

                  ValueNode* one = find_node("1");

                  if (one == nullptr) {
                     one = new ValueNode("1", false, 0, true);
                     add_to_memory(one);
                  }

                  ValueNode* i = find_node(*$2);
                  

                  if (i == nullptr) {
                     i = new ValueNode(*$2, false, 0, true);
                     add_to_memory(i);
                     cout << "I: " << i->name << " added to memeory" << endl;
                  }
                  
                  $$ = new ForNode(i, $4, $6, one, $8, "JPOS"); 
               }
             | FOR PID FROM value DOWNTO value DO commands ENDFOR {
                  ValueNode* one = find_node("1");

                  if (one == nullptr) {
                     one = new ValueNode("1", false, 0, true);
                     add_to_memory(one);
                  }

                  ValueNode* i = find_node(*$2);
                  

                  if (i == nullptr) {
                     i = new ValueNode(*$2, false, 0, true);
                     add_to_memory(i);
                     cout << "I: " << i->name << " added to memeory" << endl;
                  }

                  

                  $$ = new ForNode(i, $4, $6, one, $8, "JNEG");
                  
               }
             | proc_call ';' { 
                  cout << "PROCEDURE CALL  " << *$1 << endl;
                  ProcedureNode* proc = find_procedure(*$1);
                  $$ = new ProcedureCallNode(proc); 
               }
             | READ identifier ';' { 
                  $2->initialize();
                  $$ = new IONode("GET", $2);
               }
             | WRITE value ';' { 
                  if (dynamic_cast<ArrayElem*>($2) != nullptr) {
                     cout << "ARRAY ELEMENT at write" << endl;
                  }
                  $$ = new IONode("PUT", $2);
               }
             ;

proc_head:   PID '(' args_decl ')' { 
                  $$ = $1; 
               }
            ;

proc_call:   PID '(' args ')' { 
                  $$ = $1; 
               }
            ;

declarations:declarations ',' PID {
                  ValueNode* val = find_node(*$3);

                  if (val == nullptr) {
                     val = new ValueNode(*$3, false, 0, true);
                     add_to_memory(val);
                  } else {
                     std::string error_msg = "Variable " + val->name + " already declared";
                     yyerror(error_msg.c_str());
                  }
               }
             | PID {
                  ValueNode* val = find_node(*$1);

                  if (val == nullptr) {
                     val = new ValueNode(*$1, false, 0, true);
                     add_to_memory(val);
                  } else {
                     std::string error_msg = "Variable " + val->name + " already declared";
                     yyerror(error_msg.c_str());
                  }
               }
            | declarations ',' PID'['number':'number']' {
               ArrayNode* arr = new ArrayNode(*$3, stoll($5->name), stoll($7->name));
               add_to_memory(arr);

               ArrayNode* arr2 = find_array(*$3);
               cout << "ARRAY: " << arr2->name << " SIZE: " << arr2->size << " register num,: " << arr2->register_number <<  endl;
            }
            | PID'['number':'number']' {
               ArrayNode* arr = new ArrayNode(*$1, stoll($3->name), stoll($5->name));
               add_to_memory(arr);

               ArrayNode* arr2 = find_array(*$1);
               cout << "ARRAY: " << arr2->name << " SIZE: " << arr2->size << " register num,: " << arr2->register_number << endl;
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
             | value ADD value {
                  $$ = new AssignNode("ADD", nullptr, $1, $3);
               }
             | value SUB value {
                  $$ = new AssignNode("SUB", nullptr, $1, $3);
               }
             | value MUL value {
                  $$ = new AssignNode("MUL", nullptr, $1, $3);

                  ValueNode* zero = find_node("0");
                  if (zero == nullptr) {
                        zero = new ValueNode("0", false, 2, true);
                        add_to_memory(zero);
                  }

                  ValueNode* one = find_node("1");
                  if (one == nullptr) {
                        one = new ValueNode("1", false, 2, true);
                        add_to_memory(one);
                  }
               }
             | value DIV value {
                  $$ = new AssignNode("DIV", nullptr, $1, $3);
               }
             | value MOD value {
                  $$ = new AssignNode("MOD", nullptr, $1, $3);
               }
               ;

condition:   value EQ value { 
                  $$ = new ConditionNode("JZERO", $1, $3, false);
               }
             | value NEQ value { 
                  $$ = new ConditionNode("JZERO", $1, $3, true); 
               }
             | value GT value { 
                  $$ = new ConditionNode("JPOS", $1, $3, false);
               }
             | value LT value {
                  $$ = new ConditionNode("JNEG", $1, $3, false);
               }
             | value GE value { 
                  $$ = new ConditionNode("JNEG", $1, $3, true);
               }
             | value LE value { 
                  $$ = new ConditionNode("JPOS", $1, $3, true);
               }
               ;

value:       number {
                  $$ = $1;
               }
             | identifier {
                  $$ = $1;
               }
               ;

number:      NUM {
                  $$ = find_node(to_string($1));

                  if ($$ == nullptr) {
                     $$ = new ValueNode(to_string($1), true, 2, true);
                     $$->initialize();
                     add_to_memory($$);
                     $$->set_value($1);
                  }
               }
            | SUB NUM %prec UMINUS {
                  $$ = find_node(to_string(-$2));

                  if ($$ == nullptr) {
                     $$ = new ValueNode(to_string(-$2), true, 2, true);
                     $$->initialize();
                     add_to_memory($$);
                     $$->set_value(-$2);
                  }
               }

identifier:  PID {
                  $$ = find_node(*$1);

                  if ($$ == nullptr) {
                     $$ = new ValueNode(*$1, true, 0, true);
                     add_to_memory($$);
                  }
               }
               | PID'['PID']' {

                  ArrayNode* arr = find_array(*$1);
                  ValueNode* index = find_node(*$3);

                  cout << "ARRAY: " << arr->name << " INDEX: " << index->name << endl;

                  string arr_name = arr->name + "[" + index->name + "]";

                  $$ = new ArrayElem(arr_name, arr, index);
                  
               }
               | PID'['number']' {
                  ArrayNode* arr = find_array(*$1);

                  long long reg = stoll($3->name) - arr->start + arr->register_number;

                  cout << "arr start " << arr->start << " reg num " << arr->register_number << endl;
                  cout << "ARRAY: " << arr->name << " REG: " << reg << endl;

                  string arr_name = arr->name + "[" + $3->name + "]";

                  $$ = $3;
               }
             ;
%%

int main() {
   yyparse();
   print_memory();
   cout << endl;
   print_assembly();
   save_assembly_to_file("output.txt");
   return 0;
}

void yyerror(const char *str)
{
   fprintf(stderr, "\033[0;31mERROR AT LINE %d: %s\033[0m\n", yylineno - 1, str);
   exit(EXIT_FAILURE);
}
%{
   #include <iostream>
   #include <vector>
   #include <string>
   #include "nodes.hpp"
   using namespace std;

   extern int yylineno;

   int yywrap() { return 1; }
   int yylex();
   void yyerror(const char *s);

   // Declare the global variable here
   bool is_in_procedure = false;
   string procedure_name = "";
%}


%locations
%define parse.error verbose

%code requires {
   #include <iostream>
   #include <vector>
   #include <string>
   #include "nodes.hpp"
   using namespace std;
   extern bool is_in_procedure;
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
   vector<ValueNode*>* proc_arguments;
   ProcedureHeader* proc_header;
}

%type <sval> declarations
%type <proc_arguments> args_decl args
%type <proc_header> proc_head
%type <main> main
%type <commands> commands
%type <command> command expression proc_call
%type <condition> condition
%type <value> value identifier number
%type <procedures> procedures
%type <program> program_all
%token <ival> NUM
%token <sval> PID

%token ASSIGN ADD SUB MUL DIV MOD EQ NEQ GT LT GE LE
%token T
%token PROCEDURE IS BEG END PROGRAM IF THEN ELSE ENDIF WHILE ENDWHILE REPEAT UNTIL FOR FROM TO DO ENDFOR DOWNTO READ WRITE
%left ADD SUB
%left MUL DIV MOD
%left EQ NEQ GT LT GE LE
%right UMINUS

%%

program_all:  procedures main {

                  $$ = new ProgramNode($1, $2);
                  $$->analyze(false); 
                  $$->translate(false);
               }

procedures:  procedures PROCEDURE proc_head IS declarations BEG commands END { 
                  $$ = nullptr; 
               }
             | procedures PROCEDURE proc_head IS BEG commands END { 
                  $$ = $1;
                  $$->add_procedure(new ProcedureNode($3, $6));
                  is_in_procedure = false;
                  procedure_name = "";
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
             | FOR PID FROM value TO value DO {
                  if (is_in_procedure) {
                     ValueNode* i = find_node(procedure_name + *$2);

                     if (i == nullptr) {
                        i = new ValueNode(procedure_name + *$2, false, 0, true, false);
                        add_to_memory(i);
                     }
                  } else {
                     ValueNode* i = find_node(*$2);

                     if (i == nullptr) {
                        i = new ValueNode(*$2, false, 0, true, false);
                        add_to_memory(i);
                     }
                  }

               } commands ENDFOR {
                  
                  ValueNode* one = find_node("1");

                  if (one == nullptr) {
                     one = new ValueNode("1", false, 2, true, true);
                     add_to_memory(one);
                  }

                  ValueNode* i = find_node(*$2);
                  
                  $$ = new ForNode(i, $4, $6, one, $9, "JPOS"); 
               }
             | FOR PID FROM value DOWNTO value DO commands ENDFOR {
                  ValueNode* one = find_node("1");

                  if (one == nullptr) {
                     one = new ValueNode("1", false, 2, true, true);
                     add_to_memory(one);
                  }

                  ValueNode* i = find_node(*$2);

                  if (i == nullptr) {
                     i = new ValueNode(*$2, false, 0, true, false);
                     add_to_memory(i);
                  }

                  $$ = new ForNode(i, $4, $6, one, $8, "JNEG");
                  
               }
             | proc_call ';' { 
                  $$ = $1;
               }
             | READ identifier ';' { 
                  $2->initialize();
                  $$ = new IONode("GET", $2);
               }
             | WRITE value ';' {
                  $$ = new IONode("PUT", $2);
               }
             ;

proc_head_begin: PID '(' {
                  is_in_procedure = true;
                  procedure_name = *$1;
               }
               ;

proc_head:
            proc_head_begin args_decl ')'
            {
               auto args = $2;
               $$ = new ProcedureHeader(procedure_name, args);
            }
            ;

proc_call:   proc_head_begin args ')' {
                  ProcedureNode* proc = find_procedure(procedure_name);
                  $$ = new ProcedureCallNode(proc, $2);
                  is_in_procedure = false;
                  procedure_name = "";
               }
            ;

declarations:declarations ',' PID {
                  ValueNode* val = find_node(*$3);

                  if (val == nullptr) {
                     val = new ValueNode(*$3, false, 0, true, false);
                     add_to_memory(val);
                  } else {
                     std::string error_msg = "Variable " + val->name + " already declared";
                     yyerror(error_msg.c_str());
                  }
               }
             | PID {
                  ValueNode* val = find_node(*$1);

                  if (val == nullptr) {
                     val = new ValueNode(*$1, false, 0, true, false);
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
            }
            | PID'['number':'number']' {
               ArrayNode* arr = new ArrayNode(*$1, stoll($3->name), stoll($5->name));
               add_to_memory(arr);

               ArrayNode* arr2 = find_array(*$1);
            }
             ;

args_decl:   args_decl ',' PID {
                  ValueNode* val = new ValueNode(procedure_name + *$3, false, 0, true, false); 
                  add_to_memory(val);
                  $$ = $1;
                  $$->push_back(val);
               }
             | PID {
                  ValueNode* val = new ValueNode(procedure_name + *$1, false, 0, true, false); 
                  add_to_memory(val);
                  $$ = new vector<ValueNode*>();
                  $$->push_back(val);
               }
            | args_decl ',' T PID {
               cout << "args_decl, T PID" << endl;
               ArrayNode* arr = new ArrayNode(procedure_name + *$4);
               add_to_memory(arr);
               $$ = $1;
               $$->push_back(arr);
            }
            | T PID {
               cout << "T PID" << endl;
               ArrayNode* arr = new ArrayNode(procedure_name + *$2);
               add_to_memory(arr);
               $$ = new vector<ValueNode*>();
               $$->push_back(arr);
            }
             ;

args:        args ',' PID { 
                  ValueNode* val = find_node(*$3);
                  $$ = $1;
                  $$->push_back(val); 
               }
             | PID { 
                  ValueNode* val = find_node(*$1);
                  $$ = new vector<ValueNode*>();
                  $$->push_back(val); 
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
                        zero = new ValueNode("0", false, 2, true, true);
                        add_to_memory(zero);
                  }

                  ValueNode* one = find_node("1");
                  if (one == nullptr) {
                        one = new ValueNode("1", false, 2, true, true);
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
                     $$ = new ValueNode(to_string($1), true, 2, true, true);
                     $$->initialize();
                     add_to_memory($$);
                     $$->set_value($1);
                  }
               }
            | SUB NUM %prec UMINUS {
                  $$ = find_node(to_string(-$2));

                  if ($$ == nullptr) {
                     $$ = new ValueNode(to_string(-$2), true, 2, true, true);
                     $$->initialize();
                     add_to_memory($$);
                     $$->set_value(-$2);
                  }
               }

identifier:  PID {

                  if (is_in_procedure) {
                     $$ = find_node(procedure_name + *$1);

                     if ($$ == nullptr) {
                        $$ = new ValueNode(procedure_name + *$1, true, 0, true, false);
                        add_to_memory($$);
                     }
                  } else {
                     $$ = find_node(*$1);

                     if ($$ == nullptr) {
                        $$ = new ValueNode(*$1, true, 0, true, false);
                        add_to_memory($$);
                     }
                  } 

               }
               | PID'['PID']' {

                  ArrayNode* arr = find_array(*$1);
                  ValueNode* index = find_node(*$3);

                  string arr_name = arr->name + "[" + index->name + "]";

                  $$ = new ArrayElem(arr_name, arr, index);
                  
               }
               | PID'['number']' {
                  ArrayNode* arr = find_array(*$1);

                  long long reg = stoll($3->name) - arr->start + arr->register_number;

                  string arr_name = arr->name + "[" + $3->name + "]";

                  $$ = new ArrayElem(arr_name, arr, $3);
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
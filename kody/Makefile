main: parser.y lexer.l
	bison -d -o parser.cpp parser.y
	flex -o lexer.cpp lexer.l
	g++ -o parse parser.cpp lexer.cpp -lfl

clean:
	rm lexer.cpp parser.cpp parser.hpp parse
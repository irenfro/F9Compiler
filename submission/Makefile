all: gencode parserCGEN

gencode: genfsm fsm-specification
	./genfsm fsm-specification > tmp && grep "#" tmp > fsm-defs && grep -v "#" tmp > fsm-code && rm tmp

parserCGEN: parserCGEN.c parser.h lexan.c lexan.h defs.h  gencode
	gcc -Wall -Werror -g -o parseC parserCGEN.c lexan.c

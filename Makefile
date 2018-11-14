all: gencode parser

gencode: genfsm fsm-specification
	./genfsm fsm-specification > tmp && grep "#" tmp > fsm-defs && grep -v "#" tmp > fsm-code && rm tmp

parser: parser.c parser.h lexan.c lexan.h defs.h  gencode
	gcc -Wall -Werror -g -o parse parser.c lexan.c

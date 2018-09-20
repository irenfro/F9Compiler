all: gencode

gencode: genfsm fsm-specification
	./genfsm fsm-specification > tmp && grep "#" tmp > fsm-defs && grep -v "#" tmp > fsm-code && rm tmp

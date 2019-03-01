#########################################################################
#									#
#		    Makefile for an f9cc compiler			#
#									#
#########################################################################

all:		FSM.ps test-1  test-2 test-3

f9cc:		main.o lexan.o parser.o
		gcc -o f9cc main.o lexan.o parser.o

main.o:		main.c defs.h
		gcc -c main.c

lexan.o:	lexan.c defs.h fsm-code fsm-defs
		gcc -c lexan.c

parser.o:	parser.c defs.h
		gcc -c parser.c

fsm-raw:	fsm-specification bin/genfsm
		genfsm fsm-specification > fsm-raw

fsm-defs:	fsm-raw
		grep define fsm-raw > fsm-defs

fsm-code:	fsm-raw
		grep -v define fsm-raw > fsm-code

test-1:		prog1
		@echo; echo '*****  testing prog1  *****'; echo
		@./prog1

prog1:		prog1.f9 f9cc
		@echo;echo '*****  compiling prog1.f9  *****'; echo
		@./f9cc -o prog1 prog1.f9

test-2:		prog2
		@echo; echo '*****  testing prog2  *****'; echo
		@./prog2

prog2:		prog2.f9 f9cc
		@echo;echo '*****  compiling prog2.f9  *****'; echo
		@./f9cc -o prog2 prog2.f9


test-3:		prog3
		@echo; echo '*****    Now try prog3    *****'; echo

prog3:		prog3.f9 f9cc
		@echo;echo '*****  compiling prog3.f9  *****'; echo
		@./f9cc -o prog3 prog3.f9

FSM.ps:		FSM.pic
		@echo making FSM.ps
		@case $(MACHINE) in xinu*) pic FSM.pic | groff > FSM.ps;;\
		 *) pic FSM.pic | troff -pFSM.ps;; esac

clean:
		@rm -f f9cc .tmp* *.o prog1 prog2 prog3 ,jed* .,*


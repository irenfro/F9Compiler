section .bss
	printStack resb 100
	printStackPointer resb 8
section .data
	SYS_EXIT  equ 1
	SYS_READ  equ 3
	SYS_WRITE equ 4
	STD_IN    equ 0
	STD_OUT   equ 1
	EXIT_CODE equ 0
	NEW_LINE  db 0xa
	tmp000    dd 0
	a dd 0
	start dd 0
	limit dd 0
	x dd 0
	sum dd 0

section .text
	global _start
_printINT:
	mov rax, rdi
	mov rcx, printStack
	mov rbx, 10
	mov [rcx], rbx
	inc rcx
	mov [printStackPointer], rcx

_printINTLoop:
	mov rdx, 0
	mov rbx, 10
	div rbx
	push rax
	add rdx, 48

	mov rcx, [printStackPointer]
	mov [rcx], dl
	inc rcx
	mov [printStackPointer], rcx

	pop rax
	cmp rax, 0
	jne _printINTLoop

_printINTLoop2:
	mov rcx, [printStackPointer]

	mov rax, 1
	mov rdi, 1
	mov rsi, rcx
	mov rdx, 1
	syscall

	mov rcx, [printStackPointer]
	dec rcx
	mov [printStackPointer], rcx

	cmp rcx, printStack
	jge _printINTLoop2
	ret
_start:
section .data
	msg001 db '"\nThis is prog1\n\nTesting the basics of comparisons\n\n"', 0xa
	len001 equ $ - msg001
section .text
mov eax, SYS_WRITE
mov ebx, STD_OUT
mov ecx, msg001
mov edx, len001
int 0x80
section .data
	tmp001 dd 0
section .text
section .data
	tmp002 dd 0
section .text
section .data
	tmp003 dd 0
section .text
section .data
	tmp004 dd 0
section .text
section .data
	tmp005 dd 0
section .text
mov dword [tmp005], 2
mov eax, [tmp005]
mov dword [tmp004], eax
mov eax, [tmp004]
mov dword [tmp003], eax
section .data
	tmp006 dd 0
section .text
section .data
	tmp007 dd 0
section .text
section .data
	tmp008 dd 0
section .text
mov dword [tmp008], 5
mov eax, [tmp008]
mov dword [tmp007], eax
mov eax, [tmp007]
mov dword [tmp006], eax
mov eax, [tmp003]
mov ebx, [tmp006]
cmp eax, ebx
jg LAB002
mov eax, 0
jmp LAB003
LAB002:
mov eax, 1
LAB003:
mov dword [tmp003], eax
mov eax, [tmp003]
mov dword [tmp002], eax
mov eax, [tmp002]
mov dword [tmp001], eax
cmp dword [tmp001], 0
je LAB001
section .data
	msg002 db '"error: 2 compares greater than 5!\n\n"', 0xa
	len002 equ $ - msg002
section .text
mov eax, SYS_WRITE
mov ebx, STD_OUT
mov ecx, msg002
mov edx, len002
int 0x80
section .data
	tmp009 dd 0
section .text
section .data
	tmp010 dd 0
section .text
section .data
	tmp011 dd 0
section .text
section .data
	tmp012 dd 0
section .text
section .data
	tmp013 dd 0
section .text
mov dword [tmp013], 1
mov eax, [tmp013]
mov dword [tmp012], eax
mov eax, [tmp012]
mov dword [tmp011], eax
mov eax, [tmp011]
mov dword [tmp010], eax
mov eax, [tmp010]
mov dword [tmp009], eax
mov eax, SYS_EXIT
mov ebx, [tmp009]
int 0x80
LAB001: 
section .data
	tmp014 dd 0
section .text
section .data
	tmp015 dd 0
section .text
section .data
	tmp016 dd 0
section .text
section .data
	tmp017 dd 0
section .text
section .data
	tmp018 dd 0
section .text
mov dword [tmp018], 5
mov eax, [tmp018]
mov dword [tmp017], eax
mov eax, [tmp017]
mov dword [tmp016], eax
mov eax, [tmp016]
mov dword [tmp015], eax
mov eax, [tmp015]
mov dword [tmp014], eax
mov eax, [tmp014]
mov dword [a], eax
section .data
	tmp019 dd 0
section .text
section .data
	tmp020 dd 0
section .text
section .data
	tmp021 dd 0
section .text
section .data
	tmp022 dd 0
section .text
section .data
	tmp023 dd 0
section .text
mov dword [tmp023], 3
mov eax, [tmp023]
mov dword [tmp022], eax
mov eax, [tmp022]
mov dword [tmp021], eax
section .data
	tmp024 dd 0
section .text
section .data
	tmp025 dd 0
section .text
section .data
	tmp026 dd 0
section .text
mov dword [tmp026], 1
mov eax, [tmp026]
mov dword [tmp025], eax
mov eax, [tmp025]
mov dword [tmp024], eax
mov eax, [tmp021]
mov ebx, [tmp024]
cmp eax, ebx
jg LAB005
mov eax, 0
jmp LAB006
LAB005:
mov eax, 1
LAB006:
mov dword [tmp021], eax
mov eax, [tmp021]
mov dword [tmp020], eax
mov eax, [tmp020]
mov dword [tmp019], eax
cmp dword [tmp019], 0
je LAB004
section .data
	msg003 db '" 3 is indeed greater than 1\n\n"', 0xa
	len003 equ $ - msg003
section .text
mov eax, SYS_WRITE
mov ebx, STD_OUT
mov ecx, msg003
mov edx, len003
int 0x80
LAB004: 
section .data
	tmp027 dd 0
section .text
section .data
	tmp028 dd 0
section .text
section .data
	tmp029 dd 0
section .text
section .data
	tmp030 dd 0
section .text
section .data
	tmp031 dd 0
section .text
mov dword [tmp031], 1
mov eax, [tmp031]
mov dword [tmp030], eax
section .data
	tmp032 dd 0
section .text
section .data
	tmp033 dd 0
section .text
mov dword [tmp033], 2
mov eax, [tmp033]
mov dword [tmp032], eax
mov eax, [tmp030]
mov ebx, [tmp032]
add eax, ebx
mov dword [tmp030], eax
mov eax, [tmp030]
mov dword [tmp029], eax
section .data
	tmp034 dd 0
section .text
section .data
	tmp035 dd 0
section .text
section .data
	tmp036 dd 0
section .text
mov dword [tmp036], 3
mov eax, [tmp036]
mov dword [tmp035], eax
mov eax, [tmp035]
mov dword [tmp034], eax
mov eax, [tmp029]
mov ebx, [tmp034]
cmp eax, ebx
je LAB008
mov eax, 0
jmp LAB009
LAB008:
mov eax, 1
LAB009:
mov dword [tmp029], eax
mov eax, [tmp029]
mov dword [tmp028], eax
mov eax, [tmp028]
mov dword [tmp027], eax
cmp dword [tmp027], 0
je LAB007
section .data
	msg004 db '" 1 + 2 is indeed equal to 3\n\n"', 0xa
	len004 equ $ - msg004
section .text
mov eax, SYS_WRITE
mov ebx, STD_OUT
mov ecx, msg004
mov edx, len004
int 0x80
jmp LAB010
LAB007: 
section .data
	msg005 db '" 1 + 2 != 3\n\n"', 0xa
	len005 equ $ - msg005
section .text
mov eax, SYS_WRITE
mov ebx, STD_OUT
mov ecx, msg005
mov edx, len005
int 0x80
section .data
	tmp037 dd 0
section .text
section .data
	tmp038 dd 0
section .text
section .data
	tmp039 dd 0
section .text
section .data
	tmp040 dd 0
section .text
section .data
	tmp041 dd 0
section .text
mov dword [tmp041], 1
mov eax, [tmp041]
mov dword [tmp040], eax
mov eax, [tmp040]
mov dword [tmp039], eax
mov eax, [tmp039]
mov dword [tmp038], eax
mov eax, [tmp038]
mov dword [tmp037], eax
mov eax, SYS_EXIT
mov ebx, [tmp037]
int 0x80
LAB010: 
section .data
	tmp042 dd 0
section .text
section .data
	tmp043 dd 0
section .text
section .data
	tmp044 dd 0
section .text
section .data
	tmp045 dd 0
section .text
section .data
	tmp046 dd 0
section .text
section .data
	tmp047 dd 0
section .text
section .data
	tmp048 dd 0
section .text
section .data
	tmp049 dd 0
section .text
section .data
	tmp050 dd 0
section .text
section .data
	tmp051 dd 0
section .text
mov dword [tmp051], 1
mov eax, [tmp051]
mov dword [tmp050], eax
section .data
	tmp052 dd 0
section .text
section .data
	tmp053 dd 0
section .text
mov dword [tmp053], 2
mov eax, [tmp053]
mov dword [tmp052], eax
mov eax, [tmp050]
mov ebx, [tmp052]
add eax, ebx
mov dword [tmp050], eax
mov eax, [tmp050]
mov dword [tmp049], eax
section .data
	tmp054 dd 0
section .text
section .data
	tmp055 dd 0
section .text
section .data
	tmp056 dd 0
section .text
mov dword [tmp056], 3
mov eax, [tmp056]
mov dword [tmp055], eax
mov eax, [tmp055]
mov dword [tmp054], eax
mov eax, [tmp049]
mov ebx, [tmp054]
cmp eax, ebx
je LAB012
mov eax, 0
jmp LAB013
LAB012:
mov eax, 1
LAB013:
mov dword [tmp049], eax
mov eax, [tmp049]
mov dword [tmp048], eax
mov eax, [tmp048]
mov dword [tmp047], eax
mov eax, [tmp047]
mov dword [tmp046], eax
mov eax, [tmp046]
mov dword [tmp045], eax
mov eax, [tmp045]
mov dword [tmp044], eax
mov eax, [tmp044]
mov dword [tmp043], eax
section .data
	tmp057 dd 0
section .text
section .data
	tmp058 dd 0
section .text
section .data
	tmp059 dd 0
section .text
section .data
	tmp060 dd 0
section .text
section .data
	tmp061 dd 0
section .text
section .data
	tmp062 dd 0
section .text
section .data
	tmp063 dd 0
section .text
section .data
	tmp064 dd 0
section .text
section .data
	tmp065 dd 0
section .text
mov dword [tmp065], 5
mov eax, [tmp065]
mov dword [tmp064], eax
mov eax, [tmp064]
mov dword [tmp063], eax
section .data
	tmp066 dd 0
section .text
section .data
	tmp067 dd 0
section .text
section .data
	tmp068 dd 0
section .text
mov dword [tmp068], 4
mov eax, [tmp068]
mov dword [tmp067], eax
mov eax, [tmp067]
mov dword [tmp066], eax
mov eax, [tmp063]
mov ebx, [tmp066]
cmp eax, ebx
jne LAB014
mov eax, 0
jmp LAB015
LAB014:
mov eax, 1
LAB015:
mov dword [tmp063], eax
mov eax, [tmp063]
mov dword [tmp062], eax
mov eax, [tmp062]
mov dword [tmp061], eax
mov eax, [tmp061]
mov dword [tmp060], eax
mov eax, [tmp060]
mov dword [tmp059], eax
mov eax, [tmp059]
mov dword [tmp058], eax
mov eax, [tmp058]
mov dword [tmp057], eax
mov eax, [tmp043]
mov ebx, [tmp057]
and eax, ebx
mov dword [tmp043], eax
mov eax, [tmp043]
mov dword [tmp042], eax
cmp dword [tmp042], 0
je LAB011
section .data
	msg006 db '" (1+2 == 3) && (5 != 4) is true \n\n"', 0xa
	len006 equ $ - msg006
section .text
mov eax, SYS_WRITE
mov ebx, STD_OUT
mov ecx, msg006
mov edx, len006
int 0x80
jmp LAB016
LAB011: 
section .data
	msg007 db '" (1+2 == 3) && (5 != 4)  fails\n"', 0xa
	len007 equ $ - msg007
section .text
mov eax, SYS_WRITE
mov ebx, STD_OUT
mov ecx, msg007
mov edx, len007
int 0x80
section .data
	tmp069 dd 0
section .text
section .data
	tmp070 dd 0
section .text
section .data
	tmp071 dd 0
section .text
section .data
	tmp072 dd 0
section .text
section .data
	tmp073 dd 0
section .text
mov dword [tmp073], 1
mov eax, [tmp073]
mov dword [tmp072], eax
mov eax, [tmp072]
mov dword [tmp071], eax
mov eax, [tmp071]
mov dword [tmp070], eax
mov eax, [tmp070]
mov dword [tmp069], eax
mov eax, SYS_EXIT
mov ebx, [tmp069]
int 0x80
LAB016: 
section .data
	tmp074 dd 0
section .text
section .data
	tmp075 dd 0
section .text
section .data
	tmp076 dd 0
section .text
section .data
	tmp077 dd 0
section .text
section .data
	tmp078 dd 0
section .text
mov dword [tmp078], 1
mov eax, [tmp078]
mov dword [tmp077], eax
mov eax, [tmp077]
mov dword [tmp076], eax
mov eax, [tmp076]
mov dword [tmp075], eax
mov eax, [tmp075]
mov dword [tmp074], eax
mov eax, [tmp074]
mov dword [start], eax
section .data
	tmp079 dd 0
section .text
section .data
	tmp080 dd 0
section .text
section .data
	tmp081 dd 0
section .text
section .data
	tmp082 dd 0
section .text
section .data
	tmp083 dd 0
section .text
mov dword [tmp083], 10
mov eax, [tmp083]
mov dword [tmp082], eax
mov eax, [tmp082]
mov dword [tmp081], eax
mov eax, [tmp081]
mov dword [tmp080], eax
mov eax, [tmp080]
mov dword [tmp079], eax
mov eax, [tmp079]
mov dword [limit], eax
section .data
	tmp084 dd 0
section .text
section .data
	tmp085 dd 0
section .text
section .data
	tmp086 dd 0
section .text
section .data
	tmp087 dd 0
section .text
section .data
	tmp088 dd 0
section .text
mov dword [tmp088], 0
mov eax, [tmp088]
mov dword [tmp087], eax
mov eax, [tmp087]
mov dword [tmp086], eax
mov eax, [tmp086]
mov dword [tmp085], eax
mov eax, [tmp085]
mov dword [tmp084], eax
mov eax, [tmp084]
mov dword [sum], eax
section .data
	tmp089 dd 0
section .text
section .data
	tmp090 dd 0
section .text
section .data
	tmp091 dd 0
section .text
section .data
	tmp092 dd 0
section .text
section .data
	tmp093 dd 0
section .text
mov eax, [start]
mov dword [tmp093], eax
mov eax, [tmp093]
mov dword [tmp092], eax
mov eax, [tmp092]
mov dword [tmp091], eax
mov eax, [tmp091]
mov dword [tmp090], eax
mov eax, [tmp090]
mov dword [tmp089], eax
mov eax, [tmp089]
mov dword [x], eax
section .data
	tmp094 dd 0
section .text
LAB017: 
section .data
	tmp095 dd 0
section .text
section .data
	tmp096 dd 0
section .text
section .data
	tmp097 dd 0
section .text
section .data
	tmp098 dd 0
section .text
mov eax, [x]
mov dword [tmp098], eax
mov eax, [tmp098]
mov dword [tmp097], eax
mov eax, [tmp097]
mov dword [tmp096], eax
section .data
	tmp099 dd 0
section .text
section .data
	tmp100 dd 0
section .text
section .data
	tmp101 dd 0
section .text
mov eax, [limit]
mov dword [tmp101], eax
mov eax, [tmp101]
mov dword [tmp100], eax
mov eax, [tmp100]
mov dword [tmp099], eax
mov eax, [tmp096]
mov ebx, [tmp099]
cmp eax, ebx
jle LAB019
mov eax, 0
jmp LAB020
LAB019:
mov eax, 1
LAB020:
mov dword [tmp096], eax
mov eax, [tmp096]
mov dword [tmp095], eax
mov eax, [tmp095]
mov dword [tmp094], eax
cmp dword [tmp094], 0
je LAB018
section .data
	tmp102 dd 0
section .text
section .data
	tmp103 dd 0
section .text
section .data
	tmp104 dd 0
section .text
section .data
	tmp105 dd 0
section .text
section .data
	tmp106 dd 0
section .text
mov eax, [sum]
mov dword [tmp106], eax
mov eax, [tmp106]
mov dword [tmp105], eax
section .data
	tmp107 dd 0
section .text
section .data
	tmp108 dd 0
section .text
mov eax, [x]
mov dword [tmp108], eax
mov eax, [tmp108]
mov dword [tmp107], eax
mov eax, [tmp105]
mov ebx, [tmp107]
add eax, ebx
mov dword [tmp105], eax
mov eax, [tmp105]
mov dword [tmp104], eax
mov eax, [tmp104]
mov dword [tmp103], eax
mov eax, [tmp103]
mov dword [tmp102], eax
mov eax, [tmp102]
mov dword [sum], eax
section .data
	tmp109 dd 0
section .text
section .data
	tmp110 dd 0
section .text
section .data
	tmp111 dd 0
section .text
section .data
	tmp112 dd 0
section .text
section .data
	tmp113 dd 0
section .text
mov dword [tmp113], 1
mov eax, [tmp113]
mov dword [tmp112], eax
mov eax, [tmp112]
mov dword [tmp111], eax
mov eax, [tmp111]
mov dword [tmp110], eax
mov eax, [tmp110]
mov dword [tmp109], eax
mov eax, [x]
mov ebx, [tmp109]
add eax, ebx
mov dword [x], eax
jmp LAB017
LAB018: 
section .data
	msg008 db '"\nThe sum of integers from "', 0xa
	len008 equ $ - msg008
section .text
mov eax, SYS_WRITE
mov ebx, STD_OUT
mov ecx, msg008
mov edx, len008
int 0x80
section .data
	tmp114 dd 0
section .text
section .data
	tmp115 dd 0
section .text
section .data
	tmp116 dd 0
section .text
section .data
	tmp117 dd 0
section .text
section .data
	tmp118 dd 0
section .text
mov eax, [start]
mov dword [tmp118], eax
mov eax, [tmp118]
mov dword [tmp117], eax
mov eax, [tmp117]
mov dword [tmp116], eax
mov eax, [tmp116]
mov dword [tmp115], eax
mov eax, [tmp115]
mov dword [tmp114], eax
mov edi, [tmp114]
call _printINT
section .data
	msg009 db '" to "', 0xa
	len009 equ $ - msg009
section .text
mov eax, SYS_WRITE
mov ebx, STD_OUT
mov ecx, msg009
mov edx, len009
int 0x80
section .data
	tmp119 dd 0
section .text
section .data
	tmp120 dd 0
section .text
section .data
	tmp121 dd 0
section .text
section .data
	tmp122 dd 0
section .text
section .data
	tmp123 dd 0
section .text
mov eax, [limit]
mov dword [tmp123], eax
mov eax, [tmp123]
mov dword [tmp122], eax
mov eax, [tmp122]
mov dword [tmp121], eax
mov eax, [tmp121]
mov dword [tmp120], eax
mov eax, [tmp120]
mov dword [tmp119], eax
mov edi, [tmp119]
call _printINT
section .data
	msg010 db '" is: "', 0xa
	len010 equ $ - msg010
section .text
mov eax, SYS_WRITE
mov ebx, STD_OUT
mov ecx, msg010
mov edx, len010
int 0x80
section .data
	tmp124 dd 0
section .text
section .data
	tmp125 dd 0
section .text
section .data
	tmp126 dd 0
section .text
section .data
	tmp127 dd 0
section .text
section .data
	tmp128 dd 0
section .text
mov eax, [sum]
mov dword [tmp128], eax
mov eax, [tmp128]
mov dword [tmp127], eax
mov eax, [tmp127]
mov dword [tmp126], eax
mov eax, [tmp126]
mov dword [tmp125], eax
mov eax, [tmp125]
mov dword [tmp124], eax
mov edi, [tmp124]
call _printINT
section .data
	msg011 db '"\n\n"', 0xa
	len011 equ $ - msg011
section .text
mov eax, SYS_WRITE
mov ebx, STD_OUT
mov ecx, msg011
mov edx, len011
int 0x80
section .data
	tmp129 dd 0
section .text
section .data
	tmp130 dd 0
section .text
section .data
	tmp131 dd 0
section .text
section .data
	tmp132 dd 0
section .text
section .data
	tmp133 dd 0
section .text
mov dword [tmp133], 0
mov eax, [tmp133]
mov dword [tmp132], eax
mov eax, [tmp132]
mov dword [tmp131], eax
mov eax, [tmp131]
mov dword [tmp130], eax
mov eax, [tmp130]
mov dword [tmp129], eax
mov eax, SYS_EXIT
mov ebx, [tmp129]
int 0x80
mov eax, SYS_EXIT
mov ebx, EXIT_CODE
int 0x80

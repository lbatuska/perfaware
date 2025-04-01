
bits 16
mov al,ah
mov cx,ax
MOV AX, [1234h]
MOV [1234h], AX
MOV [1234], AL
MOV AL, [1234]
MOV DX, [BX+SI+1]
MOV DX, [BP+DI+2]
MOV DX, [BP+DI+256]
MOV DX, [BX+SI+257]

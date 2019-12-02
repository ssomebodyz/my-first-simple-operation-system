use16
org 0x7C00
start:
mov ax, cs
mov ds, ax
mov ss, ax
mov sp, start
mov ah, 0x0e

mov bx, gray
call puts
call move_down
mov bx, white
call puts
call move_down
mov bx, yellow
call puts
call move_down
mov bx, blue
call puts
call move_down
mov bx, red
call puts
call move_down
mov bx, green
call puts
call move_down

call inf

inf:
xor ah,ah
mov ah, 0x00
int 0x16
;mov ah, 0x0e
;int 0x10
cmp ah,48h
je up
cmp ah,50h; down
je down
cmp al,0Dh
je choose

back_inf:
call clear_screen
cmp dl,1
je set_gray
cmp dl,2
je set_white
cmp dl,3
je set_yellow
cmp dl,4
je set_blue
cmp dl,5
je set_red
cmp dl,6
je set_green
jmp inf

down:
inc dl
jmp back_inf

up:
dec dl
jmp back_inf

set_gray:
mov bx,gray
call puts
jmp inf

set_white:
mov bx,white
call puts
jmp inf

set_blue:
mov bx,blue
call puts
jmp inf

set_red:
mov bx,red
call puts
jmp inf

set_green:
mov bx,green
call puts
jmp inf

set_yellow:
mov bx,yellow
call puts
jmp inf

choose :
cmp dl,1
je gray_color
cmp dl,2
je white_color
cmp dl,3
je yellow_color
cmp dl,4
je blue_color
cmp dl,5
je red_color
cmp dl,6
je green_color
ret

gray_color:
mov ax, 03
int 10h
;mov ah, 06h
;xor al, al
;xor cx, cx
;mov dx, 184Fh
;mov bh, 8
;int 0x10
mov bx,grayy
call puts
jmp exit

white_color:
mov ax, 03
int 10h
;mov ah, 06h
;xor al, al
;xor cx, cx
;mov dx, 184Fh
;mov bh, 7
;int 0x10
mov bx,whitee
call puts
jmp exit

blue_color:
mov ax, 03
int 10h
;mov ah, 06h
;xor al, al
;xor cx, cx
;mov dx, 184Fh
;mov bh, 1
;int 0x10
mov bx,bluee
call puts
jmp exit

red_color:
mov ax, 03
int 10h
;mov ah, 06h
;xor al, al
;xor cx, cx
;mov dx, 184Fh
;mov bh, 4
;int 0x10
mov bx,redd
call puts
jmp exit

yellow_color:
mov ax, 03
int 10h
;mov ah, 06h
;xor al, al
;xor cx, cx
;mov dx, 184Fh
;mov bh, 6
;int 0x10
mov bx,yelloww
call puts
jmp exit

green_color:
mov ax, 03
int 10h
;mov ah, 06h
;xor al, al
;xor cx, cx
;mov dx, 184Fh
;mov bh, 2
;int 0x10
mov bx,greenn
call puts
jmp exit

clear_screen:
mov ax, 03
int 10h
ret

puts:
mov al, [bx]
test al,al
je end_puts
mov ah, 0x0e
int 0x10
add bx, 1
jmp puts
end_puts:
ret

gray:
db "gray", 0
white:
db "white", 0
yellow:
db "yellow", 0
blue:
db "blue", 0
red:
db "red", 0
green:
db "green", 0
output:
db "Correrc input!",0
exit_status:
db "GJ!",0

greenn:
db "2",0

bluee:
db "1", 0

redd:
db "5", 0

yelloww:
db "6", 0

grayy:
db "8", 0

whitee:
db "7", 0

move_down:
mov al, 0x0A
int 0x10
mov al, 0x0D
int 0x10
ret


exit:

mov ax, 0x1100
mov es, ax
mov bx, 0
mov ah, 0x2
mov dl, 1  ;nomer doska
mov dh, 0  ;nomer golovki
mov ch, 0
mov cl, 2 ; nomer sektora
mov al, 16   ;kolichestro sektorov
int 0x13

mov ax, 0x1300
mov es, ax
mov bx, 0
mov ah, 0x2
mov dl, 1  ;nomer doska
mov dh, 0  ;nomer golovki
mov ch, 0
mov cl, 18 ; nomer sektora
mov al, 7   ;kolichestro sektorov
int 0x13


cli
lgdt[gdt_info]
in al, 0x92      
or al, 2
out 0x92, al
mov eax, cr0
or al, 1
mov cr0, eax
jmp 0x8:protected_mode  


;YADRO

;YADRO



gdt:
db 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
db 0xff, 0xff, 0x00, 0x00, 0x00, 0x9A, 0xCF, 0x00
db 0xff, 0xff, 0x00, 0x00, 0x00, 0x92, 0xCF, 0x00

gdt_info:
dw gdt_info - gdt 
dw gdt, 0


use32
protected_mode:
mov ax, 0x10
mov es, ax
mov ds, ax
mov ss, ax
call 0x11000

times (512 - ($ - start) - 2) db 0
db 0x55, 0xAA ;

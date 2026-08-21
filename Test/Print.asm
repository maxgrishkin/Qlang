global _3_int5_print
section .text

_3_int5_print:
    ; Сохраняем регистры
    push rbp
    mov rbp, rsp
    push rbx
    push rcx
    push rdx
    push rsi
    push rdi

    ; Выделяем место под буфер в стеке (32 байта)
    sub rsp, 32

    ; Проверка на ноль
    cmp rdi, 0
    jne .not_zero

    ; Если число равно 0, выводим '0' и '\n'
    mov byte [rsp], '0'
    mov byte [rsp + 1], 10
    mov rax, 1                  ; sys_write
    mov rdi, 1                  ; stdout
    mov rsi, rsp                ; указатель на буфер
    mov rdx, 2                  ; длина: '0' + '\n'
    syscall
    jmp .end

.not_zero:
    ; Флаг отрицательности в rsi (0 - положительное, 1 - отрицательное)
    xor rsi, rsi
    test rdi, rdi
    jns .positive

    ; Отрицательное число
    mov rsi, 1
    neg rdi                     ; делаем положительным

.positive:
    ; Преобразуем число в строку (в обратном порядке)
    mov rbx, 10                 ; основание системы счисления
    lea rcx, [rsp + 31]         ; конец буфера (32 байта)
    mov byte [rcx], 0           ; null-терминатор

.convert_loop:
    dec rcx
    xor rdx, rdx
    div rbx                     ; rdx:rax / 10
    add dl, '0'                 ; преобразуем в ASCII
    mov [rcx], dl
    test rax, rax
    jnz .convert_loop

    ; Добавляем знак минус если нужно
    test rsi, rsi
    jz .no_neg
    dec rcx
    mov byte [rcx], '-'

.no_neg:
    ; Вычисляем длину строки
    mov rax, rsp                ; начало буфера
    mov rdx, rsp
    add rdx, 32                 ; конец буфера
    sub rdx, rcx                ; длина строки с числом

    ; Добавляем символ новой строки
    mov byte [rcx + rdx], 10    ; помещаем '\n' после числа
    inc rdx                     ; увеличиваем длину на 1

    ; Системный вызов write
    push rcx                    ; сохраняем указатель на начало строки
    mov rax, 1                  ; sys_write
    mov rdi, 1                  ; stdout
    mov rsi, rcx                ; указатель на строку
    ; rdx уже содержит длину
    syscall
    pop rcx

.end:
    ; Освобождаем место в стеке
    add rsp, 32

    ; Восстанавливаем регистры
    pop rdi
    pop rsi
    pop rdx
    pop rcx
    pop rbx
    mov rsp, rbp
    pop rbp
    ret
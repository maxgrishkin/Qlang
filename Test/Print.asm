section .text
global _3_int5_print

; Функция print(number)
; Выводит число на экран
_3_int5_print:
    call toStr          ; Преобразуем число в строку
    ; rax = указатель на строку
    ; rbx = длина строки

    ; Выводим строку
    mov rsi, rax        ; Указатель на строку
    mov rdx, rbx        ; Длина строки
    mov rax, 1          ; sys_write
    mov rdi, 1          ; stdout
    syscall

    ; Освобождаем память
    mov rdi, rax        ; Указатель на строку (сохранить перед syscall!)
    mov rax, 11         ; munmap
    mov rsi, rbx        ; Размер (длина + 1 для нуль-терминатора)
    inc rsi
    syscall

    ret

; Функция toStr(number)
; Преобразует число в строку
; Вход: rdi = число для преобразования
; Выход: rax = указатель на строку, rbx = длина строки
toStr:
    push rbp
    mov rbp, rsp

    ; Проверка на 0
    cmp rdi, 0
    jne .not_zero
    ; Если число 0, возвращаем "0"
    mov rax, 9          ; mmap
    xor rdi, rdi
    mov rsi, 2          ; 2 байта (цифра + нуль)
    mov rdx, 3          ; PROT_READ | PROT_WRITE
    mov r10, 0x22       ; MAP_ANONYMOUS | MAP_PRIVATE
    mov r8, -1
    xor r9, r9
    syscall

    mov byte [rax], '0'
    mov byte [rax+1], 0
    mov rbx, 1          ; длина = 1
    pop rbp
    ret

.not_zero:
    ; Сохраняем исходное число
    mov r12, rdi        ; r12 = исходное число

    ; Подсчитываем количество цифр
    mov rax, rdi        ; копируем число
    xor rcx, rcx        ; rcx = счётчик цифр

.count_digits:
    inc rcx             ; увеличиваем счётчик
    xor rdx, rdx
    mov rbx, 10
    div rbx             ; rax = rax / 10
    test rax, rax
    jnz .count_digits

    ; rcx = количество цифр
    mov r13, rcx        ; сохраняем длину

    ; Выделяем память под строку (длина + 1 для нуль-терминатора)
    mov rax, 9          ; mmap
    xor rdi, rdi
    mov rsi, rcx
    inc rsi             ; +1 для нуль-терминатора
    mov rdx, 3          ; PROT_READ | PROT_WRITE
    mov r10, 0x22       ; MAP_ANONYMOUS | MAP_PRIVATE
    mov r8, -1
    xor r9, r9
    syscall

    ; rax = указатель на выделенную память
    mov r14, rax        ; сохраняем указатель
    mov r15, rax        ; r15 будет двигаться по строке
    add r15, r13        ; идём с конца
    dec r15             ; указываем на последний символ

    ; Заполняем строку цифрами (с конца)
    mov rax, r12        ; восстанавливаем число

.fill_digits:
    xor rdx, rdx
    mov rbx, 10
    div rbx             ; rax = число / 10, rdx = остаток (цифра)

    add dl, '0'         ; преобразуем в ASCII
    mov [r15], dl       ; записываем цифру
    dec r15             ; двигаемся влево

    test rax, rax
    jnz .fill_digits

    ; Добавляем нуль-терминатор
    mov byte [r14 + r13], 0

    ; Возвращаем результат
    mov rax, r14        ; указатель на строку
    mov rbx, r13        ; длина строки

    pop rbp
    ret
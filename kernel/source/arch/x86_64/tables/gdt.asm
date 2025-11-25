global __gdt_load

__gdt_load:
    lgdt    [rdi]
    push    rsi
    lea     rax, [rel .load_code]
    push    rax
    retfq
.load_code:
    mov     ss, rdx
    xor     rax, rax
    mov     ds, rax
    mov     es, rax
    ret

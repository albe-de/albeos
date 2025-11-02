.intel_syntax noprefix
.text

; Heap storage - 2MB aligned on 32-byte boundary
.bss
.align 32
heap:
    .skip 2097152

free_list:
    .skip 4

.text

; Internal helper - align size to 8-byte boundary
align_up:
    mov eax, [esp+4]
    add eax, 7
    and eax, ~7
    ret

; Initialize the kernel memory allocator
.globl kmalloc_init
kmalloc_init:
    ; Set up initial free block spanning entire heap
    mov eax, offset heap
    mov [free_list], eax

    ; Block header: size=2097140, free=1, next=NULL
    mov dword ptr [eax], 2097140
    mov dword ptr [eax+4], 1
    mov dword ptr [eax+8], 0
    ret

; Allocate memory block of requested size
; Args: size (stack)
; Returns: pointer to allocated memory or NULL
.globl kmalloc
kmalloc:
    push ebx
    push esi

    ; Align requested size
    push dword ptr [esp+12]
    call align_up
    add esp, 4
    mov ebx, eax                ; ebx = aligned_size

    ; Walk the free list
    mov esi, [free_list]
.search_loop:
    test esi, esi
    jz .no_memory

    ; Check if block is free and large enough
    cmp dword ptr [esi+4], 0    ; is_free?
    je .next_block
    cmp dword ptr [esi], ebx    ; size >= requested?
    jb .next_block

    ; Can we split this block?
    mov eax, [esi]
    sub eax, ebx
    cmp eax, 12                 ; enough room for new header?
    jb .no_split

    ; Split the block
    lea edx, [esi+ebx+12]       ; new_block = current + size + header
    sub dword ptr [esi], ebx
    sub dword ptr [esi], 12

    mov eax, [esi]
    mov [edx], eax              ; new_block->size
    mov dword ptr [edx+4], 1    ; new_block->free = true
    mov eax, [esi+8]
    mov [edx+8], eax            ; new_block->next = current->next

    mov [esi], ebx              ; current->size = requested
    mov [esi+8], edx            ; current->next = new_block

.no_split:
    ; Mark block as used
    mov dword ptr [esi+4], 0
    lea eax, [esi+12]           ; return pointer past header
    jmp .done

.next_block:
    mov esi, [esi+8]
    jmp .search_loop

.no_memory:
    xor eax, eax

.done:
    pop esi
    pop ebx
    ret

; Free allocated memory block
; Args: ptr (stack)
.globl kfree
kfree:
    push ebx

    mov eax, [esp+8]
    test eax, eax
    jz .done

    ; Get block header
    sub eax, 12
    mov ebx, eax

    ; Mark as free
    mov dword ptr [ebx+4], 1

    ; Try to coalesce with next block
    mov eax, [ebx+8]            ; next block
    test eax, eax
    jz .done
    cmp dword ptr [eax+4], 0    ; is next free?
    je .done

    ; Merge with next block
    mov edx, [eax]              ; next->size
    add edx, [ebx]              ; current->size + next->size
    add edx, 12                 ; + header size
    mov [ebx], edx

    mov edx, [eax+8]            ; next->next
    mov [ebx+8], edx            ; current->next = next->next

.done:
    pop ebx
    ret

section .text
    global HalWritePort8
    global HalReadPort8
    global HalWritePort16
    global HalReadPort16

HalWritePort8:
    mov  dx, di
    mov  al, sil
    out  dx, al
    ret

HalReadPort8:
    mov  dx, di
    in   al, dx
    ret

HalWritePort16:
    mov  dx, di
    mov  ax, si
    out  dx, ax
    ret

HalReadPort16:
    mov  dx, di
    in   ax, dx
    ret

#ifndef FYNAOS_SYMBOLS_H
#define FYNAOS_SYMBOLS_H

extern unsigned char _kernel_phys_start;
extern unsigned char _init_phys_start;
extern unsigned char _init_phys_end;
extern unsigned char _text_phys_start;
extern unsigned char _text_phys_end;
extern unsigned char _rodata_phys_start;
extern unsigned char _rodata_phys_end;
extern unsigned char _data_phys_start;
extern unsigned char _data_phys_end;
extern unsigned char _bss_phys_start;
extern unsigned char _bss_phys_end;
extern unsigned char _kernel_phys_end;

extern unsigned char _kernel_virt_start;
extern unsigned char _init_virt_start;
extern unsigned char _init_virt_end;
extern unsigned char _text_virt_start;
extern unsigned char _text_virt_end;
extern unsigned char _rodata_virt_start;
extern unsigned char _rodata_virt_end;
extern unsigned char _data_virt_start;
extern unsigned char _data_virt_end;
extern unsigned char _bss_virt_start;
extern unsigned char _bss_virt_end;
extern unsigned char _kernel_virt_end;

#endif

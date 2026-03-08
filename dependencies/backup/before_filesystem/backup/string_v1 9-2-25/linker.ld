/* the industry standard is 'start', or atleast thats what I tell myself... */
/* dont forget that in our instance, $HOME = /Desktop/Albe_OS */
/* im top dog. */

ENTRY(start)

SECTIONS
{
    . = 0x100000;  /* Load address */
    .text :
    {
        *(.multiboot)
        *(.text)
    }
    .rodata :
    {
        *(.rodata)
    }
    .data :
    {
        *(.data)
    }
    .bss :
    {
        *(.bss)
    }
}

/* the industry standard is 'start', or atleast thats what I tell myself... */
/* dont forget that in our instance, $HOME = /Desktop/Albe_OS */
/* im top dog. */

ENTRY(_start)

PHDRS
{
    code PT_LOAD FLAGS(5);  /* Read + Execute */
    data PT_LOAD FLAGS(6);  /* Read + Write */
}

SECTIONS
{
    . = 0x100000;
    
    .text :
    {
        *(.multiboot)
        *(.text)
    } :code
    
    .rodata :
    {
        *(.rodata)
    } :code
    
    . = ALIGN(0x1000);
    
    .data :
    {
        *(.data)
    } :data
    
    .bss :
    {
        *(.bss)
    } :data
}
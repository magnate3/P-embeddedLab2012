ENTRY(main)
MEMORY
{
  FLASH (rx) : ORIGIN = 0x00000000, LENGTH = 128K
  RAM (rwx) : ORIGIN = 0x20000000, LENGTH = 20K

}

SECTIONS
{
	.text :
	{
		KEEP(*(.isr_vector))
 		*(.text)
 		*(.text.*)
		*(.rodata)
		_sidata = .;
	} >FLASH

	/* Initialized data will initially be loaded in FLASH at the end of the .text section. */
    .data : AT (_sidata) /* mapping to flash */
    {
		_sdata = .;
		*(.data)		/* Initialized data */
		_edata = .;
		_sromfs = .;
		*(.romfs.data)
		_eromfs = .;
	} >RAM

	.bss : {
		_sbss = .;
		*(.bss)         /* Zero-filled run time allocate data memory */
		_ebss = .;
	} >RAM
    
	_sromfs_at_fl = _sromfs - ORIGIN(RAM) + _sidata;
    _estack = ORIGIN(RAM) + LENGTH(RAM);
 }  

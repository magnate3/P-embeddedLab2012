	.equ	STACK_TOP, 0x20000800	/* Equates symbol to value */
	.text				/* Tells AS to assemble region */
	.syntax	unified			/* Means language is ARM UAL */
	.thumb				/* Means ARM ISA is Thumb */
	.global	_start			/* .global exposes symbol */
					/* _start label is the beginning */
					/* ...of the program region */
	.type	start, %function	/* Specifies start is a function */
					/* start label is reset handler */
_start:					
	.word	STACK_TOP, start	/* Inserts word 0x20000800 */
					/* Inserts word (start) */
start:					
	movs r0, #10			/* We’ve seen the rest ... */
	movs r1, #0			
loop:					
	adds r1, r0			
	subs r0, #1			
	bne  loop			
deadloop:				
	b    deadloop			
	.end

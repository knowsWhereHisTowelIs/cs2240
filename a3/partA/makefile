# Project name
SOURCE          = main.c error.c
OUTPUT		    = a.out
# || InteractiveLED1-1.S NOTE MUST B CAPITAL S
# ASM NOTES {
# mov.b  #1, &__P1OUT //move bit to location
# .cpu 430 // assebly directive -helps complie not lego
# if want to use line numbers change [.file "blink.c"] to blink.S or just delete line
# 2 kinds of instructions [real -have a curcuit (add), emulated -use something else (cmp equivalent to something else)]
# }
ADDITIONAL      = -lm
# Get base name so we can create .elf file
NAME            = $(OUTPUT)
# Optimisation level
OPTIMIZATION    = -O0
# Extra variables
CFLAGS          = -Wall -g

# Build and link executable
make:
	gcc $(CFLAGS) $(SOURCE) -o $(OUTPUT) $(ADDITIONAL)

# Clean up temporary files
clean:
	rm -f *.out

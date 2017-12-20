# Project name
SOURCE          = shell.c makeargv.c error.c apue.h
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
CFLAGS          = -Wall -g -std=c99 -Wno-implicit
FILE_TO_FORMAT = $(FILE)
# Build and link executable
make:
	gcc $(CFLAGS) $(SOURCE) -o $(OUTPUT) $(ADDITIONAL)

run:
	gcc $(CFLAGS) $(SOURCE) -o $(OUTPUT) $(ADDITIONAL)
	./$(OUTPUT)

format:
	indent $(FILE_TO_FORMAT) -o $(FILE_TO_FORMAT) -nbad -bap -bbo -nbc -br -brs -c33 -cd33 -ncdb -ce -ci4 -cli0 -cp33 -cs -d0 -di1 -nfc1 -nfca -hnl -i4 -ip0 -l75 -lp -npcs -nprs -npsl -saf -sai -saw -nsc -nsob -nss -i8

# Clean up temporary files
clean:
	rm -f *.out

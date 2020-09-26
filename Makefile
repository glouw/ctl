# FOR TEMPLATES, GIVE UP AFTER 1 ERROR ELSE STDOUT IS SPAMMED.
#   SEE -Wfatal-errors

all:
	gcc main.c -fsanitize=address -g -Wfatal-errors

.PHONY: run
run: aubatchSep
		./aubatchSep
		
aubatchSep : aubatchSep.c scheduler1.c dispatcher.c
	gcc -o aubatchSep aubatchSep.c scheduler1.c dispatcher.c -I. -lpthread -lm


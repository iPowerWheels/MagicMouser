all:
	gcc -Wall -Wextra -ggdb -fno-omit-frame-pointer -O2 -ljack -lsndfile -lsamplerate -lm -lglfw -lpthread -std=gnu99 -o magicmouser main.c core.c
	
run: magicmouser
	./magicmouser

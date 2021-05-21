CFLAGS=$(shell freetype-config --cflags) -g -Wall -std=gnu99 -O2
LIBS=$(shell freetype-config --libs) -lm

PROGS=ttf2svg

all: $(PROGS)

%.o: %.c
	gcc $(CFLAGS) -o $@ -c $<

ttf2svg: ttf2svg.o
	gcc -o $@ $< $(LIBS)

clean:
	rm -f $(PROGS) $(PROGS).o

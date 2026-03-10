.PHONY: all clean

PROJECT=sbca

all: $(PROJECT)

$(PROJECT): main.c
	gcc -g -o $@ $^

clean:
	-rm -rf $(PROJECT)

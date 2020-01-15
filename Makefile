CC= gcc
ALL= waiter
OBJECTS =
LIBFLAGS=
FLAGS= -Wall -O3 -g

all:    $(ALL)

clean:
	rm -f *.o $(ALL)
	rm -f `find . -name "*~"`

%.o:    %.c
	$(CC) -c $(FLAGS) -o $@ $<

$(ALL) : %: %.o Makefile $(OBJECTS)
	$(CC) $< $(OBJECTS) -o $@ $(LIBFLAGS)

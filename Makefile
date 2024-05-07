LDLIBS := -lz
OBJS := fast_fwd_crc.o demo.o
PROG := demo

.PHONY: clean run

$(PROG): $(OBJS)

clean:
	rm -f $(PROG) $(OBJS)

run:
	./demo "some prefix" ff 1234
	./demo "another prefix" 5a 5678
	./demo "and yet another prefix" 00 12345678

CFLAGS ?= -Wall -O3 -Wpedantic
CFLAGS += -std=c99
LIBS = cmdline.o filebuff.o fqgrep.o qseqs.o pherror.o seqparse.o targets.o
PROGS = fqgrep

.c .o:
	$(CC) $(CFLAGS) -c -o $@ $<

all: $(PROGS)

fqgrep: main.c libfqgrep.a
	$(CC) $(CFLAGS) -o $@ main.c libfqgrep.a -lz $(LDFLAGS)

libfqgrep.a: $(LIBS)
	$(AR) -csr $@ $(LIBS)

clean:
	$(RM) $(LIBS) $(PROGS) libfqgrep.a


cmdline.o: cmdline.h
filebuff.o: filebuff.h pherror.h qseqs.h
fqgrep.o: fqgrep.h filebuff.h pherror.h seqparse.h targets.h
qseqs.o: qseqs.h pherror.h
pherror.o: pherror.h
seqparse.o: seqparse.h filebuff.h qseqs.h
targets.o: targets.h filebuff.h pherror.h qseqs.h

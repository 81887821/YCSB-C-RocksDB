CC=g++
CFLAGS=-std=c++11 -g -Wall -pthread -I./ -I./db_impl/rocksdb/include
LDFLAGS= -lpthread -ltbb ./db_impl/rocksdb/build/librocksdb.a -lzstd -llz4 -lbz2 -lz -ldl -lsnappy
SUBDIRS=core db
SUBSRCS=$(wildcard core/*.cc) $(wildcard db/*.cc)
OBJECTS=$(SUBSRCS:.cc=.o)
EXEC=ycsbc

all: $(SUBDIRS) $(EXEC)

$(SUBDIRS):
	$(MAKE) -C $@

$(EXEC): $(wildcard *.cc) $(OBJECTS)
	$(CC) $(CFLAGS) $^ $(LDFLAGS) -o $@

clean:
	for dir in $(SUBDIRS); do \
		$(MAKE) -C $$dir $@; \
	done
	$(RM) $(EXEC)

.PHONY: $(SUBDIRS) $(EXEC)


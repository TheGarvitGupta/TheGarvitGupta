#TARGETS = master-storage quorum-server quorum-client html-server smtp+ heartbeat load-balancer

TARGETS = html-server

all: $(TARGETS)

master-storage: master-storage.cc
	g++ $^ -lpthread -std=c++11 -g -o $@

quorum-server: quorum-server.o
	g++ $^ -lpthread -lresolv -o $@
	
quorum-client: quorum-client.o
	g++ $^ -lpthread -lresolv -o $@	
	
html-server: html-server.o
	g++ $^ -lpthread -lresolv -o $@

smtp+: smtp+.o
	g++ $^ -lpthread -lresolv -o $@
	
heartbeat: heartbeat.o
	g++ $^ -lpthread -lresolv -o $@

load-balancer: load-balancer.o
	g++ $^ -lpthread -lresolv -o $@

pack:
	rm -f submit-project.zip
	zip -r submit-project.zip *.cc *.h Makefile

clean::
	rm -fv $(TARGETS) submit-project.zip *.o

PROGS=myhttpd

MUDUO_DIRECTORY = ${HOME}/build/release-install
MUDUO_INCLUDE = $(MUDUO_DIRECTORY)/include
MUDUO_LIBRARY = $(MUDUO_DIRECTORY)/lib

all : ${PROGS}
CXXFLAGS+=-g -std=c++11 -I${MUDUO_INCLUDE} 
LDFLAGS+=-L${MUDUO_LIBRARY} -lmuduo_net -lmuduo_base -lpthread -lrt

myhttpd:myhttpd.o HttpServer.o HttpRequest.o HttpResponse.o HttpContext.o Util.o
	@${CXX} ${CXXFLAGS}  -o $@   $^  ${LDFLAGS}


clean:
	@rm -rf ${PROGS} *.o 


.PHONY: all clean

COMPILER = g++
CCFLAGS = -O3 -std=c++11 -I hnswlib/ -pthread
DEBUG_FLAG = -DDEBUG=0
METHOD = verify

ifeq ($(DEBUG), 1)
    DEBUG_FLAG = -DDEBUG=1
endif

CCFLAGS += $(DEBUG_FLAG)

main: main.cc verification.hpp filter.hpp io.hpp
	${COMPILER} ${CCFLAGS} -o $@ $<

test: test.cc filter.hpp io.hpp
	${COMPILER} ${CCFLAGS} -o $@ $<

search: search.cc filter.hpp io.hpp
	${COMPILER} ${CCFLAGS} -o $@ $<
	
search_delta: search_delta.cc filter.hpp io.hpp
	${COMPILER} ${CCFLAGS} -o $@ $<

baseline: baseline.cc
	${COMPILER} ${CCFLAGS} -o $@ $<

sen: sen.cc filter.hpp io.hpp
	${COMPILER} ${CCFLAGS} -o $@ $<

search_nlp: search_nlp.cc filter.hpp io.hpp
	${COMPILER} ${CCFLAGS} -o $@ $<

search_nlp_delta: search_nlp_delta.cc filter.hpp io.hpp
	${COMPILER} ${CCFLAGS} -o $@ $<
vec_video: vec_video.cc filter.hpp io.hpp
	${COMPILER} ${CCFLAGS} -o $@ $<
vec_nlp: vec_nlp.cc filter.hpp io.hpp
	${COMPILER} ${CCFLAGS} -o $@ $<
clean: 
	rm -f $(METHOD) main test search *.o
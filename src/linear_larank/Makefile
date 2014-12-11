SHELL=/bin/sh
CFLAGS= -O3 -march=native -mtune=native -Wall
TOOLS= LaRank.cpp vectors.cpp
TOOLSINCL= LaRank.h vectors.h wrapper.h 
LARANKSRC= LaRankLearn.cpp
LARANKTESTSRC= LaRankClassify.cpp

all: la_rank_learn la_rank_classify

la_rank_learn:	$(LARANKSRC) $(TOOLS) $(TOOLSINCL)
	      	$(CXX) $(CFLAGS) -o la_rank_learn $(LARANKSRC) $(TOOLS)  -lm

la_rank_classify: 	$(LARANKTESTSRC) $(TOOLS) $(TOOLSINCL)
		  	$(CXX) $(CFLAGS) -o la_rank_classify $(LARANKTESTSRC) $(TOOLS)  -lm

clean: FORCE
	rm 2>/dev/null la_rank_learn la_rank_classify

FORCE:

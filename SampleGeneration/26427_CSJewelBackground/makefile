
default: TestRun

TestRun: Execute
	./Execute

Execute: MergeEvents.cpp
	g++ MergeEvents.cpp `fastjet-config --cxxflags --libs` -lConstituentSubtractor -std=c++11 -o Execute


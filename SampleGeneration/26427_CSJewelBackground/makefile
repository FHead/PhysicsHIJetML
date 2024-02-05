
default: TestRun

TestRun: Execute
	./Execute --Jewel test_jet.hepmc --Background test_background.hepmc --Output merged.hepmc

Execute: MergeEvents.cpp
	g++ MergeEvents.cpp `fastjet-config --cxxflags --libs` -lConstituentSubtractor -std=c++11 -o Execute


#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
using namespace std;

#include "fastjet/ClusterSequence.hh"
#include "fastjet/contrib/ConstituentSubtractor.hh"
#include "fastjet/ClusterSequenceArea.hh"
using namespace fastjet;

#include "CommandLine.h"

struct Particle
{
public:
   PseudoJet P;
   int PID;
   int Status;
};

int main(int argc, char *argv[]);
vector<Particle> ReadNextJewel(ifstream &in);
vector<Particle> ReadNextBackground(ifstream &in);
vector<Particle> ReadNextHepMC2(ifstream &in);
void WriteHepMC2(ofstream &out, vector<PseudoJet> &P);

int main(int argc, char *argv[])
{
   CommandLine CL(argc, argv);

   string JewelFileName      = CL.Get("Jewel");
   string BackgroundFileName = CL.Get("Background");
   string OutputFileName     = CL.Get("Output");

   ifstream in_jewel(JewelFileName);
   ifstream in_background(BackgroundFileName);
   ofstream out(OutputFileName);

   out << "HepMC::Version 2.06.05" << endl;
   out << "HepMC::IO_GenEvent-START_EVENT_LISTING" << endl;

   vector<Particle> Jewel = ReadNextJewel(in_jewel);
   vector<Particle> Background = ReadNextBackground(in_background);

   while(in_jewel)
   {
      Jewel = ReadNextJewel(in_jewel);
      Background = ReadNextBackground(in_background);

      vector<PseudoJet> Particles;
      for(Particle &P : Jewel)
         if(P.Status == 1)
            Particles.push_back(P.P);
      for(Particle &P : Background)
         if(P.Status == 1)
            Particles.push_back(P.P);

      vector<PseudoJet> Ghosts;
      for(Particle &P : Jewel)
         if(P.Status == 3)
            Ghosts.push_back(P.P);
      for(Particle &P : Background)
         if(P.Status == 3)
            Ghosts.push_back(P.P);

      // cout << Jewel.size() << " " << Background.size() << endl;
      // cout << Particles.size() << " " << Ghosts.size() << endl;

      /*
      JetDefinition Definition(fastjet::antikt_algorithm, 999);
      GhostedAreaSpec GhostArea(6.5, 1, 0.01);
      AreaDefinition AreaDefinition(fastjet::active_area_explicit_ghosts, GhostArea);
      ClusterSequenceArea Sequence(Particles, Definition, AreaDefinition);

      vector<PseudoJet> TempJets = Sequence.exclusive_jets(1);

      if(TempJets.size() == 0)   // what
         continue;

      vector<PseudoJet> FJGhost, FJParticle;
      SelectorIsPureGhost().sift(TempJets[0], FJGhost, FJParticle);

      for(int i = 0; i < (int)min(Ghosts.size(), FJGhost.size()); i++)
      {
         FJGhost
      }
      */

      contrib::ConstituentSubtractor Subtractor;
      Subtractor.set_distance_type(contrib::ConstituentSubtractor::deltaR);
      Subtractor.set_max_distance(999);
      Subtractor.set_alpha(1);
      // Subtractor.set_do_mass_subtraction(true);
      Subtractor.set_remove_all_zero_pt_particles(true);
      PseudoJet Subtracted = join(Subtractor.do_subtraction(Particles, Ghosts));

      vector<PseudoJet> FJGhost, FJParticle;
      SelectorIsPureGhost().sift(Subtracted.constituents(), FJGhost, FJParticle);

      WriteHepMC2(out, FJParticle);
   }

   out << "HepMC::IO_GenEvent-END_EVENT_LISTING" << endl;

   out.close();
   in_background.close();
   in_jewel.close();

   return 0;
}

vector<Particle> ReadNextJewel(ifstream &in)
{
   vector<Particle> Result;

   if(in)
      Result = ReadNextHepMC2(in);

   return Result;
}

vector<Particle> ReadNextBackground(ifstream &in)
{
   vector<Particle> Result;

   if(in)
      Result = ReadNextHepMC2(in);

   return Result;
}

vector<Particle> ReadNextHepMC2(ifstream &in)
{
   vector<Particle> Result;

   while(in)
   {
      char ch[10240];
      ch[0] = '\0';
      in.getline(ch, 10239, '\n');

      if(ch[0] == '\0')
         continue;

      stringstream str(ch);

      string Type = "";
      str >> Type;

      if(Type == "E")
         break;
      if(Type != "P")
         continue;

      int ID, PID;
      double X, Y, Z, E, M;
      int Status;

      str >> ID >> PID >> X >> Y >> Z >> E >> M >> Status;

      Particle P;
      P.PID = PID;
      P.Status = Status;
      P.P.reset(X, Y, Z, E);
      P.P.set_user_index(PID);
      Result.push_back(P);
   }

   return Result;
}

void WriteHepMC2(ofstream &out, vector<PseudoJet> &P)
{
   static int EventNumber = 0;
   EventNumber = EventNumber + 1;

   out << "E   " << EventNumber << " -1 0.00000E+00 0.00000E+00 0.00000E+00 0 0  1 1 2 0 1 0.227892E-11" << endl;
   out << "N  1 \"0\"" << endl;
   out << "U GEV MM" << endl;
   out << "C 0.15E+06 0.00E+00" << endl;
   out << "H  0  0  0  0  0   0 0 0   0  0.00E+00 0.00E+00 0.00E+00 0.00E+00" << endl;
   out << "F  0 0 -0.100000E+01 -0.100000E+01 -0.100000E+01 -0.100000E+01 -0.100000E+01 0 0" << endl;
   out << "V  -1 0 0 0 0 0 2 " << P.size() << " 0" << endl;
   out << "P  1  2212  0.000000E+00  0.000000E+00  0.251000E+04  0.251000E+04  0.938300E+00 2 0 0  -1 0" << endl;
   out << "P  2  2212  0.000000E+00  0.000000E+00 -0.251000E+04  0.251000E+04  0.938300E+00 2 0 0  -1 0" << endl;

   for(int i = 0; i < (int)P.size(); i++)
   {
      PseudoJet &p = P[i];
      out << "P " << i + 3 << " " << p.user_index() << " " << p.px() << " " << p.py() << " " << p.pz()
         << " " << p.e() << " " << p.m() << " 1 0 0   0 0" << endl;
   }
}





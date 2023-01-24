#ifndef EVENT_HH
#define EVENT_HH

#include "G4UserEventAction.hh"
#include "G4Event.hh"
#include "globals.hh"
#include "G4RunManager.hh"
#include "G4UnitsTable.hh"
#include "G4SystemOfUnits.hh"


#include "Analysis.hh"

#include "hit.hh"
#include "run.hh"

class MyEventAction : public G4UserEventAction
{
public:
        MyEventAction();
        ~MyEventAction();

        virtual void BeginOfEventAction(const G4Event*);
        virtual void EndOfEventAction(const G4Event*);
        void AddEdep(G4double edep) { fEdep += edep; }
        void AddCounter(G4int counter) { fCounter += counter; }
        void AddEdepQuenched(G4double edepQ) { fEdepQuenched += edepQ; }
        void AddEdepQuenchedIon(G4double edepQIon) { fEdepQuenchedIon += edepQIon; }
private:

        G4double   fEdep;
        G4int      fCounter;
        G4double   fEdepQuenched;
        G4double   fEdepQuenchedIon;

};


#endif
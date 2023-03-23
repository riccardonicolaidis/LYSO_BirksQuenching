#ifndef EVENT_HH
#define EVENT_HH

#include "G4UserEventAction.hh"
#include "G4Event.hh"
#include "globals.hh"
#include "G4RunManager.hh"
#include "G4UnitsTable.hh"
#include "G4SystemOfUnits.hh"
#include "G4GenericMessenger.hh"
#include "G4AnalysisManager.hh"


#include "LYSO_BIRKS_HitClass.hh"
#include "LYSO_BIRKS_RunAction.hh"
#include "LYSO_BIRKS_Globals.hh"

class LYSO_BIRKS_EventAction : public G4UserEventAction
{
public:
        LYSO_BIRKS_EventAction();
        ~LYSO_BIRKS_EventAction();

        virtual void BeginOfEventAction(const G4Event*);
        virtual void EndOfEventAction(const G4Event*);
        void AddEdep(G4double edep) { fEdep += edep; }
        void AddCounter(G4int counter) { fCounter += counter; }
        void AddEdepQuenched(G4double edepQ) { fEdepQuenched += edepQ; }

        std::vector <G4double> & GetfEdepQuenchedArray(){ return fEdepQuenchedArray;}

private:

        G4double   fEdep;
        G4int      fCounter;
        G4double   fEdepQuenched;


        std::vector <G4double> fEdepQuenchedArray;
        
        G4int      NumberOfElements = pow(NUMBER_ELEMENT_GRID,4);

        G4GenericMessenger *fMessenger;





};


#endif
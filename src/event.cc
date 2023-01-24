#include "event.hh"
#include "myglobals.hh"

MyEventAction::MyEventAction()
{}

MyEventAction::~MyEventAction()
{}



void MyEventAction::BeginOfEventAction(const G4Event*)
{
  fEdep = 0.;
  fCounter = 0;
  fEdepQuenched = 0.;
  fEdepQuenchedIon = 0.;
}


void MyEventAction::EndOfEventAction(const G4Event* event)
{
  G4AnalysisManager *man = G4AnalysisManager::Instance();

  man -> FillNtupleDColumn(0, 8, fEdep);
  man -> FillNtupleDColumn(0, 9, fCounter);
  man -> FillNtupleDColumn(0, 10, fEdepQuenched);
  man -> FillNtupleDColumn(0, 11, fEdepQuenchedIon);
  man -> AddNtupleRow(0);


}

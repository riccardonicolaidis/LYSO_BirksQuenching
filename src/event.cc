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
}


void MyEventAction::EndOfEventAction(const G4Event* event)
{
  G4AnalysisManager *man = G4AnalysisManager::Instance();

  man -> FillNtupleDColumn(8, fEdep);
  man -> FillNtupleDColumn(9, fCounter);
  man -> FillNtupleDColumn(10, fEdepQuenched);
  man -> AddNtupleRow(0);


}

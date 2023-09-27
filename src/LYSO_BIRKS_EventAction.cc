#include "LYSO_BIRKS_EventAction.hh"

LYSO_BIRKS_EventAction::LYSO_BIRKS_EventAction()
{
}

LYSO_BIRKS_EventAction::~LYSO_BIRKS_EventAction()
{}



void LYSO_BIRKS_EventAction::BeginOfEventAction(const G4Event*)
{
  fEdep = 0.;
  fCounter = 0;
  fEdepQuenched = 0.;


  NumberOfElements = pow(fRunAction -> GetNumberElementsGrid(), 4); // Initial value to be recomputed
  fEdepQuenchedArray.resize(NumberOfElements);


  for (G4int i = 0; i < NumberOfElements; i++)
  {
    fEdepQuenchedArray[i] = 0.;
  }
  
}


void LYSO_BIRKS_EventAction::EndOfEventAction(const G4Event* event)
{
  G4AnalysisManager *man = G4AnalysisManager::Instance();

  man -> FillNtupleDColumn(0, 8, fEdep);
  man -> FillNtupleDColumn(0, 9, fEdepQuenched);
  man -> AddNtupleRow(0);


}

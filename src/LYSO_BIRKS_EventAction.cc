#include "LYSO_BIRKS_EventAction.hh"

LYSO_BIRKS_EventAction::LYSO_BIRKS_EventAction()
{
  fMessenger = new G4GenericMessenger(this, "/NumberOfItems/","NUmber of items to save");
  fMessenger -> DeclareProperty("NumberOfItems", NumberOfItems, "Number if items to save");
}

LYSO_BIRKS_EventAction::~LYSO_BIRKS_EventAction()
{}



void LYSO_BIRKS_EventAction::BeginOfEventAction(const G4Event*)
{
  fEdep = 0.;
  fCounter = 0;
  fEdepQuenched = 0.;



  fEdep2.resize(NumberOfItems);
  fEdepQuenchedArray.resize(NumberOfElements);

  for (G4int i = 0; i < NumberOfItems; i++)
  {
    fEdep2[i] = 0. + i;
  }

  for (G4int i = 0; i < NumberOfElements; i++)
  {
    fEdepQuenchedArray[i] = 0. + i;
  }
  
}


void LYSO_BIRKS_EventAction::EndOfEventAction(const G4Event* event)
{
  G4AnalysisManager *man = G4AnalysisManager::Instance();

  man -> FillNtupleDColumn(0, 8, fEdep);
  man -> FillNtupleDColumn(0, 9, fEdepQuenched);
  man -> AddNtupleRow(0);


}
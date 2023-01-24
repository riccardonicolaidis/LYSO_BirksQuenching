#include "stepping.hh"


MySteppingAction::MySteppingAction(MyDetectorConstruction* det, MyEventAction* event)
: G4UserSteppingAction(), fDetector(det), fEventAction(event)
{
  kBirks = 0.;
  nH = 0.;
  nEH = 0.;
  dEdxO = 0.;
  
  fMessenger = new G4GenericMessenger(this, "/MyLysoBirksStepping/", "Control Birks quenching parameters etc. In the STEPPING action");
  fMessenger -> DeclareProperty("kBirks", kBirks, "kBirks in mm/MeV");
  fMessenger -> DeclareProperty("nH", nH, "nH");
  fMessenger -> DeclareProperty("nEH", nEH, "nEH");
  fMessenger -> DeclareProperty("dEdxO", dEdxO, "dEdxO");


}

MySteppingAction::~MySteppingAction()
{}


void MySteppingAction::UserSteppingAction(const G4Step *step)
{
    if (!fScoringVolume) 
    {
    const MyDetectorConstruction* detConstruction = static_cast<const MyDetectorConstruction*>(G4RunManager::GetRunManager()->GetUserDetectorConstruction());
    fScoringVolume = detConstruction->GetScoringVolume();
    }

  // get volume of the current step
  G4LogicalVolume* volume = step -> GetPreStepPoint() -> GetTouchableHandle() -> GetVolume() -> GetLogicalVolume();

  // check if we are in scoring volume
  if (volume != fScoringVolume) return;

  // collect energy deposited in this step
  G4double edepStep = step->GetTotalEnergyDeposit();
  
  // Compute the length of the step
  G4double stepLength = step -> GetStepLength();

  // Compute the energy stopping power
  G4double dEdx    = (stepLength > 0.) ? (edepStep/stepLength / (MeV/mm)) : 0.;
  

  G4AnalysisManager *man = G4AnalysisManager::Instance();
  //man -> FillNtupleDColumn(1, 0, step->GetPreStepPoint()->GetKineticEnergy() - 0.5*edepStep);
  //man -> FillNtupleDColumn(1, 1, stepLength);
  //man -> FillNtupleDColumn(1, 2, dEdx);

  // Particle definition electron
  //if(step -> GetTrack() -> GetDefinition() -> GetParticleName() == "e-")
  //{
  //  man -> AddNtupleRow(1);
  // }
  

  if(!(step->GetTrack()->GetDefinition()->GetParticleName() == "opticalphoton"))
  {
    
    // Compute the Birks Quenching
    G4double LightYield = (1 - nEH * exp(- dEdx/dEdxO)) * ((1 - nH)/(1 + kBirks * (1 - nH)* dEdx) + nH);
    //if(kBirks != 0.222)
    //  G4cout << "nH: " << nH << " nEH: " << nEH << " kBirks: " << kBirks << " dEdxO: " << dEdxO << G4endl;

    fEventAction->AddEdep(edepStep);
    fEventAction->AddEdepQuenched(edepStep*LightYield);

    if(!(step->GetTrack()->GetDefinition()->GetParticleName() == "gamma"))
      fEventAction->AddEdepQuenchedIon(edepStep*LightYield);
  }
  
}
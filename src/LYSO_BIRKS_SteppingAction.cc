#include "LYSO_BIRKS_SteppingAction.hh"


LYSO_BIRKS_SteppingAction::LYSO_BIRKS_SteppingAction(LYSO_BIRKS_DetectorConstruction* det, LYSO_BIRKS_EventAction* event)
: G4UserSteppingAction(), fDetector(det), fEventAction(event)
{
  kBirks = 0.;
  nH = 0.;
  nEH = 0.;
  dEdxO = 0.;
  
  fMessenger = new G4GenericMessenger(this, "/MyLysoBirksStepping/", "Control Birks quenching parameters etc. In the STEPPING action");
  fMessenger -> DeclareProperty("kBirks", kBirks, "kBirks in mm/MeV");
  fMessenger -> DeclareProperty("nH",     nH,     "nH");
  fMessenger -> DeclareProperty("nEH",    nEH,    "nEH");
  fMessenger -> DeclareProperty("dEdxO",  dEdxO,  "dEdxO");

  fMessenger -> DeclareProperty("kBirks_min", kBirks_min, "kBirks_min in mm/MeV");
  fMessenger -> DeclareProperty("nH_min",     nH_min,     "nH_min");
  fMessenger -> DeclareProperty("nEH_min",    nEH_min,    "nEH_min");
  fMessenger -> DeclareProperty("dEdx0_min",  dEdxO_min,  "dEdxO_min");

  fMessenger -> DeclareProperty("kBirks_max", kBirks_max, "kBirks_max in mm/MeV");
  fMessenger -> DeclareProperty("nH_max", nH_max, "nH_max");
  fMessenger -> DeclareProperty("nEH_max", nEH_max, "nEH_max");
  fMessenger -> DeclareProperty("dEdx0_max", dEdxO_max, "dEdxO_max");  

  fMessenger -> DeclareProperty("kBirks_nominal", kBirks_nominal, "kBirks_nominal in mm/MeV");
  fMessenger -> DeclareProperty("nH_nominal", nH_nominal, "nH_nominal");
  fMessenger -> DeclareProperty("nEH_nominal", nEH_nominal, "nEH_nominal");
  fMessenger -> DeclareProperty("dEdx0_nominal", dEdxO_nominal, "dEdxO_nominal");

}

LYSO_BIRKS_SteppingAction::~LYSO_BIRKS_SteppingAction()
{}


void LYSO_BIRKS_SteppingAction::UserSteppingAction(const G4Step *step)
{
  nH_step = (nH_max - nH_min)/NUMBER_ELEMENT_GRID;
  nEH_step = (nEH_max - nEH_min)/NUMBER_ELEMENT_GRID;
  kBirks_step = (kBirks_max - kBirks_min)/NUMBER_ELEMENT_GRID;
  dEdxO_step = (dEdxO_max - dEdxO_min)/NUMBER_ELEMENT_GRID;


  if (!fScoringVolume) 
  {
    const LYSO_BIRKS_DetectorConstruction* detConstruction = static_cast<const LYSO_BIRKS_DetectorConstruction*>(G4RunManager::GetRunManager()->GetUserDetectorConstruction());
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
    G4double LightYield = (1 - nEH_nominal * exp(- dEdx/dEdxO_nominal)) * ((1 - nH_nominal)/(1 + kBirks_nominal * (1 - nH_nominal)* dEdx) + nH_nominal);
    //if(kBirks != 0.222)
    //  G4cout << "nH: " << nH << " nEH: " << nEH << " kBirks: " << kBirks << " dEdxO: " << dEdxO << G4endl;
    fEventAction->AddEdep(edepStep);
    fEventAction->AddEdepQuenched(edepStep*LightYield);


    std::vector <G4double> & fEdepQuenchedArray = fEventAction->GetfEdepQuenchedArray();


    G4int ConfigurationNumber = 0;
    for(G4int i = 0; i < NUMBER_ELEMENT_GRID; ++i)
    {
        for(G4int j = 0; j < NUMBER_ELEMENT_GRID; ++j)
        {
          for(G4int k = 0; k < NUMBER_ELEMENT_GRID; ++k)
          {
              for(G4int l = 0; l < NUMBER_ELEMENT_GRID; ++l)
              {
                  ++ConfigurationNumber;
                  G4double kBirks_i = kBirks_min + kBirks_step * i;
                  G4double nH_j = nH_min + nH_step * j;
                  G4double nEH_k = nEH_min + nEH_step * k;
                  G4double dEdxO_l = dEdxO_min + dEdxO_step * l;
                  LightYield = (1 - nEH_k * exp(- dEdx/dEdxO_l)) * ((1 - nH_j)/(1 + kBirks_i * (1 - nH_j)* dEdx) + nH_j);

                  fEdepQuenchedArray[ConfigurationNumber] += edepStep*LightYield;
              }
          }
        }
    }
    



  }
  
}
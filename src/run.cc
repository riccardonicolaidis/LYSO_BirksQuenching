#include "run.hh"
#include "myglobals.hh"



MyRunAction::MyRunAction(MyDetectorConstruction* det, MyPrimaryGenerator* prim)
  : G4UserRunAction(),
    fDetector(det), fPrimary(prim)
{
    
    fMessenger = new G4GenericMessenger(this, "/NameOfFile/","Name of the file to save data");
    fMessenger -> DeclareProperty("NameOfFile", TotalFileName, "Name of the file to save data");

}


MyRunAction::~MyRunAction()
{}

void MyRunAction::BeginOfRunAction(const G4Run* run)
{
   
   
   
    G4AnalysisManager *man = G4AnalysisManager::Instance();    

    TotalFileNameFinal = TotalFileName + ".root";
    man -> SetVerboseLevel(6);
    man -> OpenFile(TotalFileNameFinal); 

    // Ntuple particle generator
    man -> CreateNtuple("Event","Event");
    man -> CreateNtupleDColumn("IncidentEnergy");   // 0
    man -> CreateNtupleDColumn("Xgen");             // 1
    man -> CreateNtupleDColumn("Ygen");             // 2 
    man -> CreateNtupleDColumn("Zgen");             // 3 
    man -> CreateNtupleDColumn("pDirX");            // 4 
    man -> CreateNtupleDColumn("pDirY");            // 5 
    man -> CreateNtupleDColumn("pDirZ");            // 6
    man -> CreateNtupleIColumn("event");            // 7
    man -> CreateNtupleDColumn("EdepScint");        // 8
    man -> CreateNtupleDColumn("DetectedPhotons");  // 9
    man -> CreateNtupleDColumn("EdepQuenched");     // 10
    man -> FinishNtuple(0);
   
}

void MyRunAction::EndOfRunAction(const G4Run* )
{
    
    G4AnalysisManager *man = G4AnalysisManager::Instance();
    man -> Write();
    man -> CloseFile(TotalFileNameFinal);
    
}
#include "LYSO_BIRKS_RunAction.hh"

LYSO_BIRKS_RunAction::LYSO_BIRKS_RunAction(LYSO_BIRKS_DetectorConstruction *det, LYSO_BIRKS_PrimaryGenerator *prim, LYSO_BIRKS_EventAction *event, LYSO_BIRKS_SteppingAction *stepping)
    : G4UserRunAction(),
      fDetector(det), fPrimary(prim), fEvent(event), fStepping(stepping)
{
    fStepping -> SetRunAction(this);
    fEvent    -> SetRunAction(this);

    fMessenger = new G4GenericMessenger(this, "/NameOfFile/", "Name of the file to save data");
    fMessenger->DeclareProperty("NameOfFile", TotalFileName, "Name of the file to save data");
    fMessenger->DeclareProperty("NameOfConfigFile", FileNameConfig, "Name of the file to save data configuration");

    fMessenger2 = new G4GenericMessenger(this, "/NumberElementsGrid/", "Number of elements in the grid");
    fMessenger2->DeclareProperty("NumberElementsGrid", NumberElementsGrid, "Number of elements in the grid");

    /*
    man -> CreateNtuple("dEdX", "dEdX");
    man -> CreateNtupleDColumn("E");
    man -> CreateNtupleDColumn("dX");
    man -> CreateNtupleDColumn("dEdX");
    man -> FinishNtuple(1);
   */
}

LYSO_BIRKS_RunAction::~LYSO_BIRKS_RunAction()
{
}

void LYSO_BIRKS_RunAction::BeginOfRunAction(const G4Run *run)
{

    G4AnalysisManager *man = G4AnalysisManager::Instance();

    // Ntuple particle generator
    man->CreateNtuple("Event", "Event");
    man->CreateNtupleDColumn("IncidentEnergy");                                     // 0
    man->CreateNtupleDColumn("Xgen");                                               // 1
    man->CreateNtupleDColumn("Ygen");                                               // 2
    man->CreateNtupleDColumn("Zgen");                                               // 3
    man->CreateNtupleDColumn("pDirX");                                              // 4
    man->CreateNtupleDColumn("pDirY");                                              // 5
    man->CreateNtupleDColumn("pDirZ");                                              // 6
    man->CreateNtupleIColumn("event");                                              // 7
    man->CreateNtupleDColumn("EdepScint");                                          // 8
    man->CreateNtupleDColumn("EdepQuenched");                                       // 9
    fEvent -> ResizefEdepQuenchedArray((G4int) pow(NumberElementsGrid,4));
    man->CreateNtupleDColumn("EdepQuenchedArray", fEvent->GetfEdepQuenchedArray()); // 11
    man->FinishNtuple(0);


    TotalFileNameFinal = TotalFileName + ".root";
    man->SetVerboseLevel(6);
    man->OpenFile(TotalFileNameFinal);
}

void LYSO_BIRKS_RunAction::EndOfRunAction(const G4Run *)
{

    cout << "End of Run Action" << endl;
    cout << "Printing configuration file" << endl
         << endl
         << endl;
    ofstream ConfigurationFile;
    G4String FileNameConfigFinal = FileNameConfig + ".txt";

    ConfigurationFile.open(FileNameConfigFinal);

    G4double kBirks_min = fStepping->GetkBirks_min();
    G4double nH_min = fStepping->GetnH_min();
    G4double nEH_min = fStepping->GetnEH_min();
    G4double dEdxO_min = fStepping->GetdEdxO_min();

    G4double kBirks_max = fStepping->GetkBirks_max();
    G4double nH_max = fStepping->GetnH_max();
    G4double nEH_max = fStepping->GetnEH_max();
    G4double dEdxO_max = fStepping->GetdEdxO_max();

    G4double kBirks_step = fStepping->GetkBirks_step();
    G4double nH_step = fStepping->GetnH_step();
    G4double nEH_step = fStepping->GetnEH_step();
    G4double dEdxO_step = fStepping->GetdEdxO_step();

    ConfigurationFile << "kBirks_min = " << kBirks_min << endl;
    ConfigurationFile << "nH_min = " << nH_min << endl;
    ConfigurationFile << "nEH_min = " << nEH_min << endl;
    ConfigurationFile << "dEdxO_min = " << dEdxO_min << endl;

    ConfigurationFile << "kBirks_max = " << kBirks_max << endl;
    ConfigurationFile << "nH_max = " << nH_max << endl;
    ConfigurationFile << "nEH_max = " << nEH_max << endl;
    ConfigurationFile << "dEdxO_max = " << dEdxO_max << endl;

    ConfigurationFile << "kBirks_step = " << kBirks_step << endl;
    ConfigurationFile << "nH_step = " << nH_step << endl;
    ConfigurationFile << "nEH_step = " << nEH_step << endl;
    ConfigurationFile << "dEdxO_step = " << dEdxO_step << endl
                      << endl
                      << endl;

    int ConfigurationNumber = 0;

    ConfigurationFile << "#\tkBirks\tnH\tnEH\tdEdxO" << endl;
    for (int i = 0; i < NumberElementsGrid; ++i)
    {
        for (int j = 0; j < NumberElementsGrid; ++j)
        {
            for (int k = 0; k < NumberElementsGrid; ++k)
            {
                for (int l = 0; l < NumberElementsGrid; ++l)
                {
                    ConfigurationFile << ConfigurationNumber << "\t" << kBirks_min + kBirks_step * i << "\t" << nH_min + nH_step * j << "\t" << nEH_min + nEH_step * k << "\t" << dEdxO_min + dEdxO_step * l << endl;
                    ++ConfigurationNumber;
                }
            }
        }
    }

    ConfigurationFile.close();
    cout << "End of printing configuration file" << endl
         << endl
         << endl;

    G4AnalysisManager *man = G4AnalysisManager::Instance();
    man->Write();
    man->CloseFile();
}
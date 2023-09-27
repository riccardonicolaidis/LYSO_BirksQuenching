#ifndef RUN_HH
#define RUN_HH


#include "G4UserRunAction.hh"
#include "G4Run.hh"
#include "G4GenericMessenger.hh"
#include "G4AnalysisManager.hh"

#include "LYSO_BIRKS_DetectorConstruction.hh"
#include "LYSO_BIRKS_PrimaryGenerator.hh"
#include "LYSO_BIRKS_EventAction.hh"
#include "LYSO_BIRKS_Globals.hh"
#include "LYSO_BIRKS_SteppingAction.hh"

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

using namespace std;


class LYSO_BIRKS_DetectorConstruction;
class LYSO_BIRKS_PrimaryGenerator;
class LYSO_BIRKS_EventAction;
class LYSO_BIRKS_SteppingAction;


class LYSO_BIRKS_RunAction : public G4UserRunAction
{
public:
    LYSO_BIRKS_RunAction(LYSO_BIRKS_DetectorConstruction*, LYSO_BIRKS_PrimaryGenerator*, LYSO_BIRKS_EventAction*, LYSO_BIRKS_SteppingAction*);
    ~LYSO_BIRKS_RunAction();

    virtual void BeginOfRunAction(const G4Run* );
    virtual void EndOfRunAction(const G4Run* );

    G4int GetNumberElementsGrid() {return NumberElementsGrid;}

private:
    LYSO_BIRKS_DetectorConstruction *fDetector;
    LYSO_BIRKS_PrimaryGenerator *fPrimary;
    LYSO_BIRKS_EventAction *fEvent;
    LYSO_BIRKS_SteppingAction *fStepping;
    G4GenericMessenger *fMessenger;
    G4GenericMessenger *fMessenger2;
    G4String TotalFileName;
    G4String TotalFileNameFinal;

    G4String FileNameConfig;
    G4int NumberElementsGrid = 3;

};

#endif
#ifndef STEPPING_HH
#define STEPPING_HH

#include "G4UserSteppingAction.hh"
#include "G4Step.hh"
#include "globals.hh"
#include "G4SystemOfUnits.hh"
#include "G4RunManager.hh"
#include "G4UnitsTable.hh"
#include "G4GenericMessenger.hh"
#include "G4AnalysisManager.hh"



#include "LYSO_BIRKS_DetectorConstruction.hh"
#include "LYSO_BIRKS_EventAction.hh"
#include "LYSO_BIRKS_Globals.hh"
#include "LYSO_BIRKS_RunAction.hh"

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cmath>

using namespace std;

class LYSO_BIRKS_DetectorConstruction;
class LYSO_BIRKS_EventAction;
class LYSO_BIRKS_RunAction;

class LYSO_BIRKS_SteppingAction : public G4UserSteppingAction
{
public:
    LYSO_BIRKS_SteppingAction(LYSO_BIRKS_DetectorConstruction* ,LYSO_BIRKS_EventAction *);
    ~LYSO_BIRKS_SteppingAction();

    virtual void UserSteppingAction(const G4Step*);


    G4double GetkBirks() {return kBirks;}
    G4double GetnH() {return nH;}
    G4double GetnEH() {return nEH;}
    G4double GetdEdxO() {return dEdxO;}

    G4double GetkBirks_min() {return kBirks_min;}
    G4double GetnH_min() {return nH_min;}
    G4double GetnEH_min() {return nEH_min;}
    G4double GetdEdxO_min() {return dEdxO_min;}

    G4double GetkBirks_max() {return kBirks_max;}
    G4double GetnH_max() {return nH_max;}
    G4double GetnEH_max() {return nEH_max;}
    G4double GetdEdxO_max() {return dEdxO_max;}

    G4double GetkBirks_step() {return kBirks_step;}
    G4double GetnH_step() {return nH_step;}
    G4double GetnEH_step() {return nEH_step;}
    G4double GetdEdxO_step() {return dEdxO_step;}

    G4double GetkBirks_nominal() {return kBirks_nominal;}
    G4double GetnH_nominal() {return nH_nominal;}
    G4double GetnEH_nominal() {return nEH_nominal;}
    G4double GetdEdxO_nominal() {return dEdxO_nominal;}

    void SetRunAction(LYSO_BIRKS_RunAction* run) {fRunAction = (LYSO_BIRKS_RunAction*) run;}
    


private:
    LYSO_BIRKS_EventAction *fEventAction;
    LYSO_BIRKS_DetectorConstruction *fDetector;
    G4LogicalVolume* fScoringVolume = nullptr;
    LYSO_BIRKS_RunAction *fRunAction = nullptr;

    G4GenericMessenger* fMessenger;
    G4double nH, nEH, kBirks, dEdxO;

    G4double nH_min, nEH_min, kBirks_min, dEdxO_min;
    G4double nH_max, nEH_max, kBirks_max, dEdxO_max;
    G4double nH_step, nEH_step, kBirks_step, dEdxO_step;

    G4double nH_nominal, nEH_nominal, kBirks_nominal, dEdxO_nominal;

};

#endif
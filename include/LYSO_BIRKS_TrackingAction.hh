#ifndef TRACKING_HH
#define TRACKING_HH

#include "G4UserTrackingAction.hh"
#include "globals.hh"
#include "G4RunManager.hh"
#include "G4Track.hh"
#include "G4HadronicProcessType.hh"
#include "G4SystemOfUnits.hh"
#include "G4AnalysisManager.hh"


#include "LYSO_BIRKS_DetectorConstruction.hh"


class LYSO_BIRKS_DetectorConstruction;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class LYSO_BIRKS_TrackingAction : public G4UserTrackingAction {

  public:  
    LYSO_BIRKS_TrackingAction(LYSO_BIRKS_DetectorConstruction*);
   ~LYSO_BIRKS_TrackingAction();
   
    virtual void  PreUserTrackingAction(const G4Track*);   
    virtual void PostUserTrackingAction(const G4Track*);
    
  private:
    LYSO_BIRKS_DetectorConstruction* fDetector;
};




#endif
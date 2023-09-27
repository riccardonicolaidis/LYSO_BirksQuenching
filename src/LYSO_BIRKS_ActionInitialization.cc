#include "LYSO_BIRKS_ActionInitialization.hh"

LYSO_BIRKS_ActionInitialization::LYSO_BIRKS_ActionInitialization(LYSO_BIRKS_DetectorConstruction *detector) 
  : G4VUserActionInitialization(),
  fDetector(detector)
{}

LYSO_BIRKS_ActionInitialization::~LYSO_BIRKS_ActionInitialization()
{}

void LYSO_BIRKS_ActionInitialization::BuildForMaster() const
{
  LYSO_BIRKS_RunAction *runAction = new LYSO_BIRKS_RunAction(fDetector, 0, 0, 0);
  SetUserAction(runAction);
}


void LYSO_BIRKS_ActionInitialization::Build() const
{
  LYSO_BIRKS_PrimaryGenerator *primary        = new LYSO_BIRKS_PrimaryGenerator();
  LYSO_BIRKS_EventAction      *eventAction    = new LYSO_BIRKS_EventAction();
  LYSO_BIRKS_SteppingAction   *steppingAction = new LYSO_BIRKS_SteppingAction(fDetector,eventAction);
  LYSO_BIRKS_RunAction        *runAction      = new LYSO_BIRKS_RunAction(fDetector, primary, eventAction, steppingAction);
  LYSO_BIRKS_TrackingAction   *trackingAction = new LYSO_BIRKS_TrackingAction(fDetector);
  


  SetUserAction(primary);
  SetUserAction(eventAction);
  SetUserAction(runAction);
  SetUserAction(trackingAction);
  SetUserAction(steppingAction);

}

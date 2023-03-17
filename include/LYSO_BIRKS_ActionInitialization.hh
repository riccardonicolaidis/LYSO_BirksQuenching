#ifndef ACTION_HH
#define ACTION_HH

#include "G4VUserActionInitialization.hh"

#include "LYSO_BIRKS_PrimaryGenerator.hh"
#include "LYSO_BIRKS_RunAction.hh"
#include "LYSO_BIRKS_SteppingAction.hh"
#include "LYSO_BIRKS_EventAction.hh"
//#include "detector.hh"
#include "LYSO_BIRKS_TrackingAction.hh"


class LYSO_BIRKS_DetectorConstruction;


class LYSO_BIRKS_ActionInitialization : public  G4VUserActionInitialization
{
public:
  LYSO_BIRKS_ActionInitialization(LYSO_BIRKS_DetectorConstruction *detector);
  virtual ~LYSO_BIRKS_ActionInitialization();

  virtual void BuildForMaster() const;
  virtual void Build() const;


private:
  LYSO_BIRKS_DetectorConstruction *fDetector;
};

#endif

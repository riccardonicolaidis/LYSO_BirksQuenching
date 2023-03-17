#include "LYSO_BIRKS_TrackingAction.hh"

LYSO_BIRKS_TrackingAction::LYSO_BIRKS_TrackingAction(LYSO_BIRKS_DetectorConstruction* det)
:G4UserTrackingAction(), fDetector(det)
{}

LYSO_BIRKS_TrackingAction::~LYSO_BIRKS_TrackingAction()
{}


void LYSO_BIRKS_TrackingAction::PreUserTrackingAction(const G4Track* track)
{}


void LYSO_BIRKS_TrackingAction::PostUserTrackingAction(const G4Track* )
{}
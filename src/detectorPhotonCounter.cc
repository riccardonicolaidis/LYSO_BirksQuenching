#include "detectorPhotonCounter.hh"
#include "G4OpticalPhoton.hh"
#include "action.hh"
#include "myglobals.hh"


MySensitiveDetector::MySensitiveDetector(const G4String& Name,
                         G4int Number,
                         const G4String& hitsCollectionName) 
 : G4VSensitiveDetector(Name),
   fHitsCollection(NULL)
{
  collectionName.insert(hitsCollectionName);
  DetectorType = Number;

}


MySensitiveDetector::~MySensitiveDetector()
{}




void MySensitiveDetector::Initialize(G4HCofThisEvent* hce)
{
  // Create hits collection

  fHitsCollection = new HitsCollection(SensitiveDetectorName, collectionName[0]); 

  // Add this collection in hce

  G4int hcID 
    = G4SDManager::GetSDMpointer()->GetCollectionID(collectionName[0]);
  hce->AddHitsCollection( hcID, fHitsCollection ); 
}




// Introduction of the sensitive detector informations

G4bool MySensitiveDetector::ProcessHits(G4Step *aStep, G4TouchableHistory *ROHist)
{
    if(DetectorType == SENSITIVE_DETECTOR_PHOTON_COUNTER)
    {
        G4Track *track = aStep -> GetTrack();

        if(track->GetParticleDefinition() == G4OpticalPhoton::OpticalPhotonDefinition())
        {
          track -> SetTrackStatus(fStopAndKill);

          MyEventAction *eventAction = (MyEventAction*)G4RunManager::GetRunManager() -> GetUserEventAction();
          eventAction -> AddCounter(1);

          G4StepPoint *preStepPoint = aStep -> GetPreStepPoint();    // when photon enters the detector
          G4StepPoint *postStepPoint = aStep -> GetPostStepPoint();  // when the photon exits the detector

          G4double time = preStepPoint -> GetGlobalTime();

          G4ThreeVector posPhoton = preStepPoint -> GetPosition();
    
          const G4VTouchable *touchable = aStep -> GetPreStepPoint() -> GetTouchable();
          G4int copyNo = touchable -> GetCopyNumber();

          G4VPhysicalVolume *physVol = touchable -> GetVolume();
          G4ThreeVector posDetector = physVol -> GetTranslation();


          G4int evt = G4RunManager::GetRunManager() -> GetCurrentEvent() -> GetEventID();
          //G4cout << "Event No.: " << evt << "Detector pos:  " << posDetector << "  Photon pos:  " << posPhoton << "  Copy number:  " <<  copyNo << G4endl;
        }
    }else if(DetectorType == SENSITIVE_DETECTOR_CALORIMETER)
    {
        // energy deposit
    G4double edep = aStep->GetTotalEnergyDeposit();

    if (edep==0.) return false;

    HitClass* newHit = new HitClass();

    newHit->SetTrackID  (aStep->GetTrack()->GetTrackID());
    newHit->SetChamberNb(aStep->GetPreStepPoint()->GetTouchableHandle()
                                                ->GetCopyNumber());
    newHit->SetEdep(edep);
    newHit->SetTime(aStep->GetPreStepPoint()->GetGlobalTime());
    newHit->SetPos (aStep->GetPostStepPoint()->GetPosition());

    fHitsCollection->insert( newHit );

    }

  return true;
}





void MySensitiveDetector::EndOfEvent(G4HCofThisEvent*)
{
  // 
  if (verboseLevel> 1 ) { 
     G4int nofHits = fHitsCollection->entries();
     G4cout
       << G4endl 
       << "-------->Hits Collection: in this event they are " << nofHits 
       << " hits in the tracker chambers: " << G4endl;
     for ( G4int i=0; i<nofHits; i++ ) (*fHitsCollection)[i]->Print();
  }
}
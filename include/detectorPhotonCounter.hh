#ifndef DETECTORPHOTONCOUNTER_HH
#define DETECTORPHOTONCOUNTER_HH

#include "G4VSensitiveDetector.hh"

#include "G4HCofThisEvent.hh"
#include "G4RunManager.hh"
#include "G4Step.hh"
#include "G4ThreeVector.hh"
#include "G4SDManager.hh"
#include "G4ios.hh"
#include "hit.hh"

#include "Analysis.hh"

#include <vector>



class G4Step;
class G4HCofThisEvent;


class MySensitiveDetector : public G4VSensitiveDetector
{
public:
    MySensitiveDetector(const G4String& Name, G4int Number,
                const G4String& hitsCollectionName);
    virtual ~MySensitiveDetector();

       // methods from base class
    virtual void   Initialize(G4HCofThisEvent* hitCollection);
    virtual G4bool ProcessHits(G4Step* astep, G4TouchableHistory* ROhist);
    virtual void   EndOfEvent(G4HCofThisEvent* hitCollection);

private:

    HitsCollection* fHitsCollection;
    G4int DetectorType = 0;

};


#endif
#ifndef CONSTRUCTION_HH
#define CONSTRUCTION_HH


#include "G4GeometryManager.hh"
#include "G4PhysicalVolumeStore.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4SolidStore.hh"
#include "G4VUserDetectorConstruction.hh"
#include "G4PVPlacement.hh"
#include "G4LogicalVolume.hh"
#include "G4SystemOfUnits.hh"
#include "G4VPhysicalVolume.hh"
#include "G4NistManager.hh"
#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4GenericMessenger.hh"
#include "G4MaterialPropertiesTable.hh"
#include "G4GenericMessenger.hh"
#include "G4UnionSolid.hh"
#include "G4SubtractionSolid.hh"
#include "G4RotationMatrix.hh"
#include "G4ThreeVector.hh"
#include "G4Transform3D.hh"
#include "G4SDManager.hh"
#include "G4GDMLParser.hh"
#include "G4GDMLWriteStructure.hh"
#include "G4GDMLEvaluator.hh"
#include "G4MaterialTable.hh"
#include <cmath>
#include "G4VisAttributes.hh"
#include "G4Ellipsoid.hh"


#include "G4LogicalBorderSurface.hh"
#include "G4LogicalSkinSurface.hh"
#include "G4OpticalSurface.hh"


#include "Analysis.hh"
//#include "detector.hh"
#include "detectorPhotonCounter.hh"
#include "myglobals.hh"

class MyDetectorConstruction : public G4VUserDetectorConstruction
{
public:
  MyDetectorConstruction();
  ~MyDetectorConstruction();

  virtual G4VPhysicalVolume *Construct();


  G4LogicalVolume* GetScoringVolume() const { return fScoringVolume; }


private:
  G4LogicalVolume* fScoringVolume = nullptr;

  virtual void ConstructSDandField();

  // World
  G4Box             *solidWorld;
  G4LogicalVolume   *logicWorld;
  G4VPhysicalVolume *physWorld;
  G4VisAttributes   *visWorld;

  G4LogicalBorderSurface *logicSurfScintillator;
  G4OpticalSurface   *opsurfScintillator;

 
  void DefineMaterials();

  // Materials
  G4Material *worldMat, *EJ200, *SiMat, *CdZnTe, *Al, *bachelite, *LYSO;
  G4Element *Si;
  
  // World
  G4double xWorld, yWorld, zWorld;
  
  // Scintillator
  G4double xScintillator, yScintillator, zScintillator;
  G4double LxScintillator, LyScintillator, LzScintillator;

  // PMT
  G4double xPMT, yPMT, zPMT;
  G4double diameterPMT, heightPMT;

  G4double TotalMass;

  G4Ellipsoid *solidScintillator;
  G4LogicalVolume *logicScintillator;
  G4VisAttributes *visScintillator;
  G4VPhysicalVolume *physScintillator;


  G4Tubs *solidPMT;
  G4LogicalVolume *logicPMT;
  G4VisAttributes *visPMT;
  G4VPhysicalVolume *physPMT;


  G4double kBirks;
  G4GenericMessenger *fMessenger;

};

#endif

#include "generator.hh"
#include "myglobals.hh"
#include "G4RunManager.hh"

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>

using namespace std;


MyPrimaryGenerator::MyPrimaryGenerator(): G4VUserPrimaryGeneratorAction(),
   fParticleSource(0)
{ 
  // PARTICLE GUN DEFINITION
  fParticleSource = new G4GeneralParticleSource();
  fParticleGun = new G4ParticleGun();
  
  // COUNTER
  ParticleNumber = 0;
  
  // GPS
  UseGPS = false;

  G4ParticleTable *particleTable = G4ParticleTable::GetParticleTable();
  G4String          particleName = "geantino";
  G4ParticleDefinition *particle = particleTable -> FindParticle(particleName);


  fParticleGun -> SetParticlePosition(G4ThreeVector(0.,0.,0.));
  fParticleGun -> SetParticleDefinition(particle);
  fParticleGun -> SetParticleEnergy(1.*MeV);


  // Messenger
  fMessenger = new G4GenericMessenger(this, "/MyPrimaryGenerator/", "Primary generator control");
  fMessenger -> DeclareProperty("UseGPS", UseGPS, "Use GPS or not");
}

MyPrimaryGenerator::~MyPrimaryGenerator()
{
  delete fParticleSource;
  delete fParticleGun;
}

void MyPrimaryGenerator::GeneratePrimaries(G4Event *anEvent)
{

  G4AnalysisManager *man = G4AnalysisManager::Instance();
  G4ThreeVector positionStart;
  G4ThreeVector momentumStart;

  if(UseGPS)
  {
    fParticleSource -> GeneratePrimaryVertex(anEvent);
    positionStart = fParticleSource -> GetParticlePosition();
    momentumStart = fParticleSource -> GetParticleMomentumDirection();

    man -> FillNtupleDColumn(0, fParticleSource->GetParticleEnergy());
  }
  else
  {
    fParticleGun -> GeneratePrimaryVertex(anEvent);
    positionStart = fParticleGun -> GetParticlePosition();
    momentumStart = fParticleGun -> GetParticleMomentumDirection();
  
    man -> FillNtupleDColumn(0, fParticleGun->GetParticleEnergy());
  }


  // FILL ALL THE DETAILS REQUIRED FOR THE ANALYSIS
  man -> FillNtupleDColumn(1, positionStart.getX());
  man -> FillNtupleDColumn(2, positionStart.getY());
  man -> FillNtupleDColumn(3, positionStart.getZ());
  man -> FillNtupleDColumn(4, momentumStart.getX());
  man -> FillNtupleDColumn(5, momentumStart.getY());
  man -> FillNtupleDColumn(6, momentumStart.getZ());
  
    
  G4int evt = ParticleNumber;
  // Create an output txt file but do not overwrite it if it exists
  ofstream myfile;
  myfile.open ("../output_debug.txt", ios::app);
  myfile << evt << " " << positionStart.getX() << " " << positionStart.getY() << " " << positionStart.getZ() << " " << momentumStart.getX() << " " << momentumStart.getY() << " " << momentumStart.getZ() << " " << fParticleGun->GetParticleEnergy() << endl;
  myfile.close();




  man -> FillNtupleIColumn(7,evt);

  
  
  if(ParticleNumber%100 == 0)
  {
    G4cout << ParticleNumber <<  G4endl;
  }
  

  ++ParticleNumber;
  
}

#include "LYSO_BIRKS_PrimaryGenerator.hh"


using namespace std;


LYSO_BIRKS_PrimaryGenerator::LYSO_BIRKS_PrimaryGenerator(): G4VUserPrimaryGeneratorAction(),
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

LYSO_BIRKS_PrimaryGenerator::~LYSO_BIRKS_PrimaryGenerator()
{
  delete fParticleSource;
  delete fParticleGun;
}

void LYSO_BIRKS_PrimaryGenerator::GeneratePrimaries(G4Event *anEvent)
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
  man -> FillNtupleDColumn(0, 1, positionStart.getX());
  man -> FillNtupleDColumn(0, 2, positionStart.getY());
  man -> FillNtupleDColumn(0, 3, positionStart.getZ());
  man -> FillNtupleDColumn(0, 4, momentumStart.getX());
  man -> FillNtupleDColumn(0, 5, momentumStart.getY());
  man -> FillNtupleDColumn(0, 6, momentumStart.getZ());
  
    
  G4int evt = ParticleNumber;
  // Create an output txt file but do not overwrite it if it exists
  //ofstream LYSO_BIRKS_file;
  //LYSO_BIRKS_file.open ("../output_debug.txt", ios::app);
  //LYSO_BIRKS_file << evt << " " << positionStart.getX() << " " << positionStart.getY() << " " << positionStart.getZ() << " " << momentumStart.getX() << " " << momentumStart.getY() << " " << momentumStart.getZ() << " " << fParticleGun->GetParticleEnergy() << endl;
  //LYSO_BIRKS_file.close();



  G4cout << ParticleNumber <<  G4endl;  
    

  ++ParticleNumber;
  
}

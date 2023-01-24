#include "physics.hh"

MyPhysicsList::MyPhysicsList()
{
  defaultCutValue = 1. * CLHEP::nm;

  
  RegisterPhysics (new G4EmStandardPhysics());
  RegisterPhysics (new G4OpticalPhysics());  
  RegisterPhysics (new G4DecayPhysics());
  RegisterPhysics (new G4RadioactiveDecayPhysics());
}

MyPhysicsList::~MyPhysicsList()
{}

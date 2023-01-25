#include "construction.hh"
#include "myglobals.hh"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

#include "G4UserLimits.hh"
#include "G4Region.hh"
#include "G4ProductionCuts.hh"


using namespace std;

MyDetectorConstruction::MyDetectorConstruction()
{

  kBirks = 0.;
  fMessenger = new G4GenericMessenger(this, "/MyLysoBirksConstruction/", "Control Birks quenching parameters etc. In the STEPPING action");
  fMessenger -> DeclareProperty("kBirks", kBirks, "kBirks in mm/MeV");

  // Define the materials only once
  DefineMaterials();  
}

MyDetectorConstruction::~MyDetectorConstruction()
{}

void MyDetectorConstruction::DefineMaterials()
{
  G4NistManager *nist = G4NistManager::Instance();

  

  Si    = nist -> FindOrBuildElement("Si");
  SiMat = new G4Material("Si", 2.328*g/cm3, 1);
  SiMat -> AddElement(Si,1);


  G4Element* Lutetium = nist->FindOrBuildElement("Lu");
  G4Element* Yttrium = nist->FindOrBuildElement("Y");
  G4Element* Oxygen = nist->FindOrBuildElement("O");
  G4Element* Cerium = nist->FindOrBuildElement("Ce");
  G4Element* Silicon = nist->FindOrBuildElement("Si");

  LYSO = new G4Material("LYSO",7.1*g/cm3,(G4int) 5);
  LYSO->AddElement(Lutetium,71.43*perCent);
  LYSO->AddElement(Yttrium,4.03*perCent);
  LYSO->AddElement(Silicon,6.37*perCent);
  LYSO->AddElement(Oxygen,18.14*perCent);
  LYSO->AddElement(Cerium,0.02*perCent); 


  EJ200     = nist -> FindOrBuildMaterial("G4_PLASTIC_SC_VINYLTOLUENE");
  CdZnTe    = nist -> FindOrBuildMaterial("G4_CADMIUM_TELLURIDE");
  worldMat  = nist -> FindOrBuildMaterial("G4_Galactic");
  Al        = nist -> FindOrBuildMaterial("G4_Al");
  bachelite = nist -> FindOrBuildMaterial("G4_BAKELITE");

  
  std::vector<G4double> RIndexWorld   = {1.                , 1.                 , 1.};
  std::vector<G4double> RIndexSi      = {3.88163           , 3.88163            , 3.88163};
  std::vector<G4double> RIndexAl      = {3.88163           , 3.88163            , 3.88163};
  std::vector<G4double> RIndexEJ200   = {1.58              , 1.58               , 1.58};
  std::vector<G4double> RIndexCdZnTe  = {3.09              , 3.09               , 3.09};
  std::vector<G4double> RIndexLYSO    = {1.82              , 1.82               , 1.82};


  std::vector<G4double> Wavelength0 = { 400 * nm, 425*nm, 500*nm};
  std::vector<G4double> Energy ;

  for (int i = 0; i < Wavelength0.size(); i++)
  {
    Energy.push_back(1240*nm/Wavelength0[i]);
  }

  std::vector<G4double> ABSL        = { 380. * cm, 380. * cm , 380. * cm  };
  std::vector<G4double> ABSL_Al     = { 0.1 * um, 0.1 * um , 0.1 * um  };
  std::vector<G4double> ABSL_Si     = { 0.1 * um, 0.1 * um , 0.1 * um  };
  std::vector<G4double> ABSL_CdZnTe = { 10 * um , 10 * um  , 10 * um   };
  std::vector<G4double> ABSL_LYSO   = { 40 * cm , 40 * cm  , 40 * cm   };
  //std::vector<G4double> ABSL_LYSO   = { 3 * cm , 3 * cm  , 3 * cm   };

  // Open a file called Efficiency.txt and read the values from a two column table
  // The first column is the wavelength in nm and the second column is the efficiency

  std::ifstream infile("/home/riccardo/Documenti/GeantProjects/LYSO_BirksQuenching/src/Efficiency.txt");
  std::string line;
  std::vector<G4double> Wavelength;
  std::vector<G4double> Efficiency;
  while (std::getline(infile, line))
  {
    std::istringstream iss(line);
    G4double a, b;
    if (!(iss >> a >> b)) { break; } // error
    Wavelength.push_back(a);
    Efficiency.push_back(b);
  }

  // Convert the wavelength to energy
  std::vector<G4double> Energy2;
  for (int i = 0; i < Wavelength.size(); i++)
  {
    Energy2.push_back(1239.84193 / Wavelength[i] * eV);
  }


  // Print the table
  for (int i = 0; i < Wavelength.size(); i++)
  {
    cout << Energy2[i] << " " << Efficiency[i] << endl;
  }

  // As before, open a file named EfficiencyLYSO.txt and read the values from a two column table
  // The first column is the wavelength in nm and the second column is the efficiency

  std::ifstream infile2("/home/riccardo/Documenti/GeantProjects/LYSO_BirksQuenching/src/EfficiencyLYSO.txt");
  std::string line2;
  std::vector<G4double> Wavelength2;
  std::vector<G4double> Efficiency2;

  while (std::getline(infile2, line2))
  {
    std::istringstream iss(line2);
    G4double a, b;
    if (!(iss >> a >> b)) { break; } // error
    Wavelength2.push_back(a);
    Efficiency2.push_back(b);
  }

  // Convert the wavelength to energy
  std::vector<G4double> Energy3;
  for (int i = 0; i < Wavelength2.size(); i++)
  {
    Energy3.push_back(1239.84193 / Wavelength2[i] * eV);
  }

  // Print the table
  for (int i = 0; i < Wavelength2.size(); i++)
  {
    cout << Energy3[i] << " " << Efficiency2[i] << endl;
  }



  G4MaterialPropertiesTable *mptWorld   = new G4MaterialPropertiesTable();
  G4MaterialPropertiesTable *mptSi      = new G4MaterialPropertiesTable();
  G4MaterialPropertiesTable *mptEJ200   = new G4MaterialPropertiesTable();
  G4MaterialPropertiesTable *mptCdZnTe  = new G4MaterialPropertiesTable();
  G4MaterialPropertiesTable *mptAl      = new G4MaterialPropertiesTable();
  G4MaterialPropertiesTable *mptLYSO    = new G4MaterialPropertiesTable();
  

  mptWorld -> AddProperty("RINDEX", Energy, RIndexWorld, 3);
  worldMat -> SetMaterialPropertiesTable(mptWorld);

  mptSi    -> AddProperty("RINDEX", Energy, RIndexWorld, 3);
  mptSi    -> AddProperty("ABSLENGTH", Energy, ABSL_Si, 3);
  SiMat    -> SetMaterialPropertiesTable(mptSi);

  mptEJ200    -> AddProperty("RINDEX", Energy, RIndexLYSO, 3);
  //mptEJ200    -> AddProperty("ABSLENGTH", Energy, ABSL, 3);
  
  mptLYSO     -> AddProperty("RINDEX", Energy, RIndexLYSO, 3);
  mptLYSO     -> AddProperty("ABSLENGTH", Energy, ABSL_LYSO, 3);

  if(OPTICAL_PROCESSES == 1)
  {
  
  mptEJ200 -> AddProperty("RINDEX", Energy, RIndexLYSO);
  //mptEJ200 -> AddProperty("ABSLENGTH", Energy, ABSL);
  EJ200    -> SetMaterialPropertiesTable(mptEJ200);
  

  mptLYSO -> AddProperty("SCINTILLATIONCOMPONENT1", Energy3, Efficiency2);
  mptLYSO -> AddProperty("SCINTILLATIONCOMPONENT2", Energy3, Efficiency2);
  mptLYSO -> AddProperty("RINDEX", Energy, RIndexLYSO);
  mptLYSO -> AddProperty("ABSLENGTH", Energy, ABSL_LYSO);
  mptLYSO -> AddConstProperty("SCINTILLATIONYIELD", 33200. / MeV);
  mptLYSO -> AddConstProperty("RESOLUTIONSCALE", 1.0);
  mptLYSO -> AddConstProperty("SCINTILLATIONTIMECONSTANT1", 36 * ns);
  mptLYSO -> AddConstProperty("SCINTILLATIONTIMECONSTANT2", 36 * ns);
  mptLYSO -> AddConstProperty("SCINTILLATIONYIELD1", 1.0);
  mptLYSO -> AddConstProperty("SCINTILLATIONYIELD2", 0.0);
  LYSO    -> SetMaterialPropertiesTable(mptLYSO);
  LYSO    -> GetIonisation()->SetBirksConstant(0.024* mm / MeV);
  }
  else
  {
    EJ200    -> SetMaterialPropertiesTable(mptEJ200);
    LYSO     -> SetMaterialPropertiesTable(mptLYSO);
  }
  
  

  mptCdZnTe   -> AddProperty("RINDEX", Energy, RIndexCdZnTe, 3);
  mptCdZnTe   -> AddProperty("ABSLENGTH", Energy, ABSL_CdZnTe, 3);
  CdZnTe      -> SetMaterialPropertiesTable(mptCdZnTe);

  mptAl       -> AddProperty("RINDEX", Energy, RIndexAl, 3);
  mptAl       -> AddProperty("ABSLENGTH", Energy, ABSL_Al, 3);
  Al          -> SetMaterialPropertiesTable(mptAl);


}

G4VPhysicalVolume *MyDetectorConstruction::Construct()
{


  // Cleanup old geometry
  G4GeometryManager::GetInstance()->OpenGeometry();
  G4PhysicalVolumeStore::GetInstance()->Clean();
  G4LogicalVolumeStore::GetInstance()->Clean();
  G4SolidStore::GetInstance()->Clean();


  // .oOo.oOo.oOo.oOo.oOo.oOo.oOo.oOo.oOo.oOo.oOo.oOo
  // .oOo.oOo.oOo.oOo.oOo.oOo.oOo.oOo.oOo.oOo.oOo.oOo
  //
  //        NUMERICAL DATA - PARAMETRIZATION
  //
  // .oOo.oOo.oOo.oOo.oOo.oOo.oOo.oOo.oOo.oOo.oOo.oOo
  // .oOo.oOo.oOo.oOo.oOo.oOo.oOo.oOo.oOo.oOo.oOo.oOo
  

  G4double MaxStepLength = 0.01 * um;



  // World dimensions
  xWorld = 5.*m;
  yWorld = 5.*m;
  zWorld = 5.*m;

  TotalMass = 0.0;
 
  solidWorld = new G4Box("solidWorld",xWorld*0.5, yWorld*0.5, zWorld*0.5);  
  logicWorld = new G4LogicalVolume(solidWorld, worldMat, "logicWorld");
  visWorld   = new G4VisAttributes(G4Colour(1.0,1.0,1.0));
  visWorld->SetVisibility(false);
  logicWorld->SetVisAttributes(visWorld);
  physWorld  = new G4PVPlacement(0, G4ThreeVector(0.,0.,0.), logicWorld, "physWorld", 0, false, 0, true);
  
  
  // Optical surface for the Scintillator
  G4MaterialPropertiesTable *mptSurface = new G4MaterialPropertiesTable();
  std::vector<G4double> energy        = {0.01*eV     ,      8.0 * eV    ,    1000* eV};
  std::vector<G4double> REFLECTIVITY  = { 1.              , 1.0              , 1.0 };
  std::vector<G4double> REFLECTIVITY2  = { 0.1              , 0.1              , 0.1 };

  mptSurface->AddProperty("REFLECTIVITY", energy , REFLECTIVITY, 3);
  opsurfScintillator  = new G4OpticalSurface("Surface");
  opsurfScintillator -> SetType(dielectric_metal);
  opsurfScintillator -> SetFinish(polished);
  opsurfScintillator -> SetModel(glisur);
  opsurfScintillator -> SetMaterialPropertiesTable(mptSurface);

  




  // SOLID SCINTILLATOR
  //solidScintillator = new G4Box("solidScintillator",LxScintillator*0.5, LyScintillator*0.5, LzScintillator*0.5);

  LxScintillator = 3.8 * cm;
  LyScintillator = 2. * cm;
  LzScintillator = 0.28 * 2 * cm;

  xScintillator = 0. * cm;
  yScintillator = 0. * cm;
  zScintillator = 0. * cm;


  // Dimensions of the PMT
  diameterPMT = 38. * mm;
  heightPMT   = 1 * mm;

  // Position of the PMT
  xPMT = 0. * cm;
  yPMT = 0. * cm;
  zPMT = - 0.5 * heightPMT;


  solidScintillator = new G4Ellipsoid("solidScintillator",LxScintillator*0.5, LyScintillator*0.5, LzScintillator*0.5, 0.,LzScintillator*0.5);

  // LOGIC SCINTILLATOR
  logicScintillator = new G4LogicalVolume(solidScintillator, LYSO, "logicScintillator");
  TotalMass += (logicScintillator->GetMass())/kg;
  visScintillator = new G4VisAttributes(G4Colour(0.3,0.3,1.0));
  visScintillator->SetForceSolid(true);
  logicScintillator->SetVisAttributes(visScintillator);

  G4UserLimits *MaxStep = new G4UserLimits();
  MaxStep -> SetMaxAllowedStep(MaxStepLength);
  logicScintillator -> SetUserLimits(MaxStep);

  G4Region *ScintillatorRegion = new G4Region("ScintillatorRegion");
  G4ProductionCuts *ScintillatorCuts = new G4ProductionCuts();
  ScintillatorCuts -> SetProductionCut(1*CLHEP::nm , "gamma");
  ScintillatorCuts -> SetProductionCut(1*CLHEP::nm , "e-");
  ScintillatorCuts -> SetProductionCut(1*CLHEP::nm , "e+");


  ScintillatorRegion -> AddRootLogicalVolume(logicScintillator);
  ScintillatorRegion -> SetProductionCuts(ScintillatorCuts);




  TotalMass+= (logicScintillator->GetMass())/g;

  for(int i = 0; i < 5; ++i)
  {
    G4cout << "######################################" << G4endl;
  }

  G4cout << "Total mass of the detector: " << TotalMass << " g" << G4endl;

  for(int i = 0; i < 5; ++i)
  {
    G4cout << "######################################" << G4endl;
  }

  // PHYSICAL SCINTILLATOR
  physScintillator = new G4PVPlacement(0, G4ThreeVector(xScintillator, yScintillator, zScintillator), logicScintillator, "physScintillator", logicWorld, false, 0, true);


  // Definition of a coating for the scintillator
  G4LogicalSkinSurface *ScintillatorSurface = new G4LogicalSkinSurface("ScintillatorSurface", logicWorld, opsurfScintillator);


  // SOLID PMT
  solidPMT = new G4Tubs("solidPMT", 0., diameterPMT*0.5, heightPMT*0.5, 0., 360.*deg);

  // LOGIC PMT
  logicPMT = new G4LogicalVolume(solidPMT, EJ200, "logicPMT");
  visPMT = new G4VisAttributes(G4Colour(0.9,0.9,0.9));
  visPMT->SetForceSolid(true);
  logicPMT->SetVisAttributes(visPMT);

  // PHYSICAL PMT
  physPMT = new G4PVPlacement(0, G4ThreeVector(xPMT, yPMT, zPMT), logicPMT, "physPMT", logicWorld, false, 0, true);


  // Assegno il volume dove voglio misurare il rilascio di energia
  fScoringVolume = logicScintillator;


  return physWorld;  
}


void MyDetectorConstruction::ConstructSDandField()
{
  MySensitiveDetector *sensDet = new MySensitiveDetector("SensitiveDetector", SENSITIVE_DETECTOR_PHOTON_COUNTER, "SensitiveDetector");
  logicPMT -> SetSensitiveDetector(sensDet);


}


# LOW ENERGY MODULE: Geant4 simulation


![Geant4 logo](https://geant4.web.cern.ch/sites/default/files/g4logo-web.png)


- For the official page of **Geant4 toolkit** visit [https://geant4.web.cern.ch/](https://geant4.web.cern.ch/)

------------------------------------

## The content of the software

In this simulation I am going to build the geometry of the Low Energy Module which is an instrument for the measurement of the fluxes of particles in the sub-MeV region up to some tenths of MeVs for electrons. 

![LEM](./docs/assets/images/LEM0._0000.png)


The detector is designed in the following way. From the external to the internal part there are:
- An aluminium mask formin a collimator. The Aluminium layer is 0.8 cm thick
- An active Veto made of plastic scintillator EJ-200 1 cm thick
- A first Silicon layer $100 \ \mu m$ thick. This is the $\Delta E$ layer in which only a small fraction of the energy is deposited. This amount of energy is sometimes called Linear Energy Transfer (LET). This because  
$$
\Delta E \approx \biggl| - \frac{1}{\rho} \frac{dE}{dx} \biggr| \rho_{Si} \times \Delta x
$$
- Then, a second layer made of Silicon with larger thickness is the layer in which the particle has to stop. The simulation has the setting to change this thickness and also to select another option of the material which is the CZT, a denser material.
- An active veto, made of plastic scintillator Ej-200 is then positioned on the back. If a particle is too energetic, it reaches the veto which wiull trigger ignoring that event.
- Additional plastic scintillator layers are added to extend te observed energy window to higher energies

The detector is based on the $\Delta E - E$ technique. This simulation is aimed at characterising the energy deposition inside the materials using the correct geometry of the instrument. 

Below are reported the front and the lateral view of the instrument.


![LEM](./docs/assets/images/LEM1._0000.png)

![LEM](./docs/assets/images/LEM2._0000.png)

This geometry has been implemented within the DetectorConstruction class.

**The number of holes is parametrised in the myglobals.cc**
--------------------

- All the parameters are described in the first part of the **Construct()** method

- **All the variables used have been declared in the preamble of the class declaration (file .hh) in the private section**

- **All the energy measurements are performed invoking the Sensitive Detector class**


- In the ConstructSDandFields() methods are defined all the sensitive detectors.

**PROBLEM**:
------------------------

Since the sensitive detector class has been used, it is not possible to reinitialize the geometry in order to change some parameters and materials because there is a problem with the sensitive detector construction. **To be solved!**

![LEM](./docs/assets/images/LEM3._0000.png)



## Particle gun and GPS

There are two possible way to generate particles:
- The **General Particle Source** generates an isotropic flux of particles from a generation plane. To set the required features one needs to write a macros as in the following case. As you can see, the **Debugging Mode** has to be switched off. By default it is switched off but it is always a good practise to initialise in the macro the value of the boolean variable.

```
/particleEnergyRandom/DebbuggingModeIsOn false

/gps/verbose 0
/gps/pos/type Plane
/gps/pos/shape Square
/gps/pos/centre 0 0 -2.4 cm
/gps/pos/halfx 6.1 cm
/gps/pos/halfy 6.1 cm
/gps/ang/type iso
/gps/ene/type Lin
/gps/ene/gradient 0
/gps/ene/intercept 1
/gps/ang/maxtheta 90 deg
/gps/ang/mintheta 0 deg

 # -------------------------------------------------------- #
 #                         ELECTRONS                        #
 # -------------------------------------------------------- #

/gps/particle e-
/gps/ene/min 0.001 MeV
/gps/ene/max 1 MeV

/NameOfFile/NameOfFile 01_08_2022_ELECTRON
/run/beamOn 10000000

```

- The **Debugging Mode** is extremely useful when studying the energy deposition inside the materials involved in the detector. With this mode, particles are shooted exactly along the axes of the collimators as it is possible to see from the pictures


![LEM](./docs/assets/images/LEM4._0000.png)






-----------------------
## Brief description of the directory structure

This is a **CMake** project. Therefore, a CMake file is required. In this case, the file is named **CMakeLists.txt**. The subdirectory **build/** is needed to compile the project.

Files are divided into two cathegories:
- **src** : Sources file with the implementation of all the classes
- **include** : Classes declaration and all the libraries to be included in the project
- **macros** : Here there are all the macros used to generate the data for the simulation. To run these macros you need to start the simulation in batch mode
```
./Name_of_the_executable_file Name_of_the_macro.mac
```
- **ROOT_macros** : Here there are all the .root macros to process data and also used to generate all the plots. To propely run these macros you need to follow these steps:
  - Run the sumulation in batch mode using one of the macros .mac in the ./macros folder. For this example take  the following (you need to be in the build directory)
```
./Simulation run_01_08_2022.mac
```
  - Create a new directory in the LEM_SIMULATION/ folder named DST_01_08_2022
```
mkdir ${LOCATION_REPOSITORY}/LEM_SIMULATION/DST_01_08_2022
```
  - Move all the .root files in this directory 
```
cd build
mv *.root ../DST_01_08_2022/.
```
  - Create a .txt file named FileNames.txt and write the names of the files in this .txt 
```
01_08_2022_ELECTRON.root
01_08_2022_PROTON.root
01_08_2022_ALPHA.root
```
  - Now, from this directory you can run a root macro
```
cd ${LOCATION_REPOSITORY}/LEM_SIMULATION/DST_01_08_2022
root -l ../ROOT_macros/AngleResolution.C
```

### Tree structure of the directory

```
.
├── APPUNTI.txt
├── AnglesHoles.txt
├── BRANCHNAME.txt
├── CMakeLists.txt
├── CadGeometry
│   ├── LEM_gdml.gdml
│   ├── LEM_geom_1_gdml-logicContainer0x2461c80.step
│   ├── LEM_geom_1_gdml.gdml
│   ├── LEM_geom_2_gdml.gdml
│   ├── LEM_geom_2_gdml1-logicContainer0x1a0ba80.step
│   ├── LEM_geometry_15_07_2022-logicContainer0x19c88a0.step
│   └── LEM_geometry_15_07_2022.gdml
├── DST_01_08_2022
│   ├── 01_08_2022_ALPHA.root
│   ├── 01_08_2022_ELECTRON.root
│   ├── 01_08_2022_PROTON.root
│   └── FileNames.txt
├── DST_27_07_2022
│   ├── 27_07_2022_ALPHA.root
│   ├── 27_07_2022_ELECTRON.root
│   ├── 27_07_2022_PROTON.root
│   └── FileNames.txt
├── DST_E_05_08_2022
│   ├── Edep_05_08_2022_ALPHA.root
│   ├── Edep_05_08_2022_ELECTRON.root
│   ├── Edep_05_08_2022_PROTON.root
│   └── FileNames.txt
├── DST_E_19_07_2022
│   ├── DEBUG_19_07_2022_ALPHA.root
│   ├── DEBUG_19_07_2022_ELECTRON.root
│   ├── DEBUG_19_07_2022_MUON.root
│   ├── DEBUG_19_07_2022_PROTON.root
│   └── ProvaFile.root.root
├── GNUmakefile
├── README.md
├── ROOTNotebook
│   ├── GeometricalFactorNotebook.ipynb
│   └── YieldNotebook.ipynb
├── ROOT_macros
│   ├── AnalysisSimulation.C
│   ├── AnalysisSimulation2.C
│   ├── AngleResolution.C
│   ├── DeltaE_E_plot.C
│   ├── DetectionYield.C
│   ├── Energy.C
│   ├── GeometricalFactor.C
│   ├── PID.C
│   └── debug.C
├── Simulation.cc
├── _config.yml
├── docs
│   └── assets
├── include
│   ├── Analysis.hh
│   ├── PhysicsList.hh
│   ├── action.hh
│   ├── construction.hh
│   ├── detector.hh
│   ├── event.hh
│   ├── generator.hh
│   ├── hit.hh
│   ├── myglobals.hh
│   ├── physics.hh
│   ├── run.hh
│   ├── stepping.hh
│   └── tracking.hh
├── macros
│   ├── default_generator.mac
│   ├── run.mac
│   ├── run_01_08_2022.mac
│   ├── run_14_07_2022.mac
│   ├── run_16_07_2022.mac
│   ├── run_17_07_2022.mac
│   ├── run_20_07_2022.mac
│   ├── run_21_07_2022.mac
│   ├── run_27_07_2022.mac
│   ├── run_E_05_08_2022.mac
│   ├── run_E_19_07_2022.mac
│   └── vis.mac
└── src
    ├── action.cc
    ├── construction.cc
    ├── detector.cc
    ├── event.cc
    ├── generator.cc
    ├── hit.cc
    ├── physics.cc
    ├── run.cc
    ├── stepping.cc
    └── tracking.cc

12 directories, 78 files
```


---------------------------------------


## Build the project
To build the project go to the /build directory and run the command

```
cmake ..
```

Then, you can run the command

```
make
```

Then, an executable file will be created in the /build directory. To run the executable file, simply run the command
```
./Name_of_the_executable_file
```
The name of the executable file, in this case, is simply "simulation", which is exactly the name of the project.

--------------------------------------------

## Useful resources 
- Physics Matters Geant4 tutorial : [Link](https://www.youtube.com/playlist?list=PLLybgCU6QCGWgzNYOV0SKen9vqg4KXeVL)
- Geant4 page : [Link](https://geant4.web.cern.ch/)
- Geant4 User guide : [Link](https://geant4.web.cern.ch/support/user_documentation)
- ROOT Cern C++ : [Link](https://root.cern/)

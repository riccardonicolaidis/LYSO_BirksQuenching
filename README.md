# LYSO Birks Quenching


![Geant4 logo](https://geant4.web.cern.ch/sites/default/files/g4logo-web.png)


- For the official page of **Geant4 toolkit** visit [https://geant4.web.cern.ch/](https://geant4.web.cern.ch/)

------------------------------------

## The content of the software

Simulation of the Birks quenching in LYSO crystal.
Geometry described below
![LYSO Geometry](./docs/assets/images/LYSO_0000.png)

Geometry:
- White: PMT detecting photons
- Violet: LYSO crystal

Properties of the LYSO
- 7.81 $g \ cm^{-3}$
- Refractive index 1.82
- Light efficiency taken from the website (Saint Gobain)


## Physics List
There is the possibility to use different Physics Lists inside the simulation. in particular we used:
- FTFP_BERT: Stangard Geant 4 library. Good at high energies but not at low energies
- LBE: Good at low energies but no photon quenching 

Optical photons (Scintillation and Cherenkov) are implemented. To enable or disable optical photons it is necessary to change the defined variable described in the file `myglobals.hh`

```
// GLOBAL VARIABLES
#define OPTICAL_PROCESSES 0
```
If the variable is set to 1 optical photons are simulated

## Tree structure

Data from the Geant4 simulation are stored in root files (located in the DST folder).
The TTree structure is reported below.

![Tree structure](./docs/assets/images/TreeStructure2.png)
- Name of the Tree: **Event**
  - IncidentEnergy: Energy of the incident particle set in the **PrimaryGenerator**
  - Xgen, Ygen, Zgen: Position of the generated particle
  - pDirX, pDirY, pDirZ: Momentum direction at generation
  - event: Event Number (empty, used only when optical photons are simulated)
  - EdepScint: Energy deposited inside the LYSO (WITHOUT Quenching)
  - DetectedPhotons : Used only when optical photons are simulated. This is the number of photons collected by the PMT.
  - EdepQuenchedIon (not shown in the picture): Energy Ionization quenched


Quenching is computed in the `SteppingAaction`. Birks quenching formula is the following

$$
Y = \left[1 - \eta_{e/h} \exp \left(- \frac{dE/dx}{(dE/dx)_0}\right)\right] \left[\frac{1 - \eta_H}{1 + B(1 - \eta_H)\times\frac{dE}{dx}} + \eta_H \right]
$$

For each step in the simulation, the $dE$ energy deposited, the $dX$ step length, are collected. Then, the stopping power $dE/dx$ is computed. The luminosity is computed by multiplying the Light Yield $Y$ by the partial energy deposition in that step.
In the end, the energy converted in light will be given by
$$
dE_{quenched} = Y \times dE
$$
The energy is then stored and at the end of the event is collected inside a `TTree`.

## Physics List
The\mathcal{L} Physics List used is the `FTFP_BERT` with some additional adjustments:
- Registered `G4EmLowEPPhysics`
```
physicsList -> ReplacePhysics(new G4EmLowEPPhysics());
```
- Registered `G4StepLimiterPhysics`
```
  G4StepLimiterPhysics* stepLimiter = new G4StepLimiterPhysics();
  stepLimiter -> SetApplyToAll(true);
  physicsList -> RegisterPhysics(stepLimiter);
```

- Using `G4Region` it is possible to instruct Geant to lower the threshold for the propagation of secondaries
- using `G4UserLimit` it is possible to instruct Geant to fix the maximum steplength in a specific logical volume







### Options
There are different settings from different runs
- A: FTFP_BERT 



- D: LBE , no photons , /process/eLoss/StepFunction 0.001 10 nm
- E: LBE , no photons , /process/eLoss/StepFunction 0.0/process/eLoss/StepFunction 0.00005 0.5 nm0005 0.5 nm
- F: LBE , no photons ,/process/eLoss/StepFunction 0.0001 1 nm
- G: LBE , no photons , /process/eLoss/StepFunction 0.0001 0.5 nm
- J: Limito il passo per produzione di secondari a 1 nm














--------------------------------------------

## Useful resources 
- Physics Matters Geant4 tutorial : [Link](https://www.youtube.com/playlist?list=PLLybgCU6QCGWgzNYOV0SKen9vqg4KXeVL)
- Geant4 page : [Link](https://geant4.web.cern.ch/)
- Geant4 User guide : [Link](https://geant4.web.cern.ch/support/user_documentation)
- ROOT Cern C++ : [Link](https://root.cern/)

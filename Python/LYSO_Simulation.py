import numpy as np
import matplotlib.pyplot as plt

import sys
import os
import argparse
import shutil



def Generate_Output_Folder(output_folder):


    current_dir = os.getcwd()
    os.chdir(output_folder)
    output_folder = os.getcwd()
    os.chdir(current_dir)


    if not os.path.exists(output_folder):
        os.makedirs(output_folder)
        print('Output folder created')
    else:
        print('Output folder already exists')
        answer = ''
        while answer != 'y' and answer != 'n':
            answer = input('Do you want to delete the folder? (y/n)')
            answer = answer.lower()
        if answer == 'y':
            shutil.rmtree(output_folder)
            os.makedirs(output_folder)
            print('Output folder created')
        else:
            print('Output folder not deleted')
            print('Overwriting files in the folder')


    Dictionary_Folder = {}
    Dictionary_Folder['out_mother'] = output_folder
    Dictionary_Folder['macro'] = os.path.join(output_folder, 'macro') 
    Dictionary_Folder['root'] = os.path.join(output_folder, 'root')
    Dictionary_Folder['log'] = os.path.join(output_folder, 'log')
    
    for key in Dictionary_Folder:
        if not os.path.exists(Dictionary_Folder[key]):
            os.makedirs(Dictionary_Folder[key])
            print(Dictionary_Folder[key], 'created')
        else:
            print(Dictionary_Folder[key], 'already exists')

    return Dictionary_Folder


# ---------------------------------------------------------------------------- #
#                         DATI DELLA SIMULAZIONE GEANT4                        #
# ---------------------------------------------------------------------------- #
# Modificare i dati della simulazione nei parametri di default della funzione  #

def Launch_Simulation(
    output_folder,
    kBirks    = [0.1, 0.2],   # Default Birks constant
    nH        = [0.01, 0.02], # Default nH
    nEH       = [0.01, 0.02], # Default nEH
    dEdx0     = [0.01, 0.02], # Default dEdx0
    Energy    = '100 keV',    # Default energy
    particle  = 'e-',         # Default particle
    nElemGrid = 2,            # Default number of elements in the grid
    nEvents   = 1000,         # Default number of events
    compile   = True,         # Default compile flag
):

    #TODO: Migliorare questa parte per quando si dovra fare la parallelizzazione dei jobs
    Directories = Generate_Output_Folder(output_folder)


    Project_Path = os.path.dirname(os.path.realpath(__file__))
    # Go back untill the project folder is LYSO_BIRKSQUENCHING 
    while os.path.basename(Project_Path) != 'LYSO_BirksQuenching':
        Project_Path = os.path.dirname(Project_Path)
        print(Project_Path) 

    #TODO: This is not a good way to do it, but for now it works
    #TODO: Change the check for the path of the project

    # Check if there is "build"
    if not os.path.exists(os.path.join(Project_Path, 'build')):
        os.makedirs(os.path.join(Project_Path, 'build'))
        compile = True
    

    if compile:
        os.chdir(os.path.join(Project_Path, 'build'))
        os.system('cmake ..')
        os.system('make -j4')

    Executable = os.path.join(Project_Path, 'build', 'Simulation')
    for files in os.listdir(os.path.join(Project_Path, 'build')):
        if files == 'Simulation':
            print('Simulation compiled')

    
    # Generate the macro file
    macro_file_name = 'macro_' + particle + '_' + Energy.replace(' ','_') + '.mac'
    macro_file_name_path = os.path.join(Directories['macro'],macro_file_name)
    macro_file = open(macro_file_name_path, 'w')

    macro_file.write("/MyPrimaryGenerator/UseGPS false \n")
    macro_file.write("/gun/particle " + particle + "\n")    
    macro_file.write("/MyLysoBirksStepping/kBirks_min " + str(kBirks[0]) + "\n")    
    macro_file.write("/MyLysoBirksStepping/kBirks_max " + str(kBirks[1]) + "\n")    

    macro_file.write("/MyLysoBirksStepping/nH_min " + str(nH[0]) + "\n")    
    macro_file.write("/MyLysoBirksStepping/nH_max " + str(nH[1]) + "\n")    
    
    macro_file.write("/MyLysoBirksStepping/nEH_min " + str(nEH[0]) + "\n")    
    macro_file.write("/MyLysoBirksStepping/nEH_max " + str(nEH[1]) + "\n")        
    
    macro_file.write("/MyLysoBirksStepping/dEdx0_min " + str(dEdx0[0]) + "\n")    
    macro_file.write("/MyLysoBirksStepping/dEdx0_max " + str(dEdx0[1]) + "\n")    
    
    macro_file.write("/MyLysoBirksStepping/kBirks_nominal " + str(kBirks[0]) + "\n")
    macro_file.write("/MyLysoBirksStepping/nH_nominal " + str(nH[0]) + "\n")    
    macro_file.write("/MyLysoBirksStepping/nEH_nominal " + str(nEH[0]) + "\n")    
    macro_file.write("/MyLysoBirksStepping/dEdx0_nominal " + str(dEdx0[0]) + "\n")    
        
    macro_file.write("/gun/position 0 0 5 cm" + "\n")    
    macro_file.write("/gun/direction 0 0 -1" + "\n")        
    
    macro_file.write("/NumberElementsGrid/NumberElementsGrid " + str(nElemGrid) + "\n")    
    macro_file.write("/NameOfFile/NameOfFile " + os.path.join(Directories['root'], macro_file_name.replace('.mac', '')) + "\n")    
    macro_file.write("/NameOfFile/NameOfConfigFile " + os.path.join(Directories['log'], macro_file_name.replace('.mac', '')) + "\n")    
    macro_file.write("/gun/energy  " + Energy + "\n")

    macro_file.write("/run/beamOn " + str(nEvents) + "\n")

    macro_file.close()

    # Launch the simulation
    print('Launching simulation')
    print(Executable + ' ' + macro_file_name_path)
    os.system(Executable + ' ' + macro_file_name_path)


if __name__ == "__main__":

    parser = argparse.ArgumentParser(description='Launch Simulation')

    parser.add_argument('-o','--output_folder', type=str, default='./output_folder', help='Output folder')
    parser.add_argument('-c','--compile' , action='store_true', help='Compile the code')
    args = parser.parse_args()
    Launch_Simulation(args.output_folder)











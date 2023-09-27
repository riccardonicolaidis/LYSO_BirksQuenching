import os
import shutil
import sys
import time
import subprocess
import argparse
import requests

import numpy as np
import matplotlib.pyplot as plt
import matplotlib.colors as colors

from LatexDocumentClass import LatexDocumentClass
from utils import *
from PlotViolation import PlotViolation

import re



def GetToken(TokenFile):
    FilePath = os.path.dirname(os.path.abspath(__file__))
    if os.path.exists(os.path.join(FilePath,'token',TokenFile)):
        f_token = open(os.path.join(FilePath,'token',TokenFile), 'r')
        TOKEN = f_token.read()
        TOKEN = TOKEN.replace('\n', '')
        f_token.close()
        return TOKEN
    else:
        print('Token file does not exist')
        return None
    
    
TOKEN_global = GetToken('token.token')    

def SendMessage(message):
    chatID = '-1001921335158'
    apiURL = 'https://api.telegram.org/bot{}/sendMessage'.format(TOKEN_global)
    try:
        response = requests.post(apiURL, json={'chat_id': chatID, 'text': message, 'parse_mode': 'HTML'})
        print(response.text)
    except Exception as e:
        print(e)



def SendPhoto(photo_path):
    chatID = '-1001921335158'        
    apiURL = 'https://api.telegram.org/bot{}/sendPhoto'.format(TOKEN_global)
    print(apiURL)
    
    try:
        params = {'chat_id': chatID}
        files = {'photo': open(photo_path, 'rb')}
        response = requests.post(apiURL, params=params, files=files)
        print(response.text)
    except Exception as e:
        print(e)





def Analysis(input_dir, OnlyLatex, OnlyRoot, BypassRemoval, SendTelegramMessage, CleanROOTFiles):
    
    E_thr_Thin = 0.01
    E_thr_Thick = 0.01
    E_thr_Plastic = 0.00001
    
    actual_dir = os.getcwd()
    print('Actual directory: {}'.format(actual_dir))
    os.chdir(input_dir)
    actual_dir = os.getcwd()
    print('Input directory: {}'.format(input_dir))
    global_input_dir = input_dir
    print('Global input directory: {}'.format(global_input_dir))
    dirs = os.listdir(global_input_dir)
    print('List of directories: {}'.format(dirs))
    
    
    # Check if the directory structure is correct
    DirectoryCheck = 0
    
    for dir in dirs:
        if os.path.isdir(os.path.join(global_input_dir, dir)):
            print('Directory: {}'.format(dir))
            if dir == 'GDML_src':
                DirectoryCheck += 1
            elif dir == 'Geant_macros':
                DirectoryCheck += 1
            elif dir == 'Text_output':
                DirectoryCheck += 1
            elif dir == 'Analysis_output':
                DirectoryCheck += 1
            elif dir == 'DST':
                DirectoryCheck += 1    
    
    print('DirectoryCheck: {}'.format(DirectoryCheck))    
    
    if DirectoryCheck < 4:
        print('The input directory is not correct. Please check the input directory.')
        sys.exit()
    else:
        print('The input directory is correct.')
        print('Continue...')
        
    # Retrieve the project directory
    # Retrieve the root macros directory
    # Retrieve the python macros directory
    # 
    # The script assumes that the project directory is the parent directory of the input directory
    
    if 'LEM_GDML_upgrade' in actual_dir:
        while not os.getcwd().endswith('LEM_GDML_upgrade'):
            print('Not in the project directory. Go up one level.')
            os.chdir('..')    
            print('Actual directory: {}'.format(os.getcwd()))
        project_dir = os.getcwd()
    else:
        project_dir = ''
        for root, dirs, files in os.walk(actual_dir):
            for dir in dirs:
                if dir == 'LEM_GDML_upgrade':
                    os.chdir(os.path.join(root, dir))
                    project_dir = os.getcwd()
                    break
    
    print('Project directory: {}'.format(project_dir))
    
    python_dir = ''
    root_dir = ''
    for dirs in os.listdir(project_dir):
        if dirs == 'PythonMacros':
            python_dir = os.path.join(project_dir, dirs)
            print('Python directory: {}'.format(python_dir))
        elif dirs == 'ROOT':
            root_dir = os.path.join(project_dir, dirs)
            print('ROOT macros directory: {}'.format(root_dir))

    # Retreive the subfolders of the input directory
    dst_dir = ''
    for dirs in os.listdir(global_input_dir):
        if dirs == 'DST':
            dst_dir = os.path.join(global_input_dir, dirs)
            print('DST directory: {}'.format(dst_dir))
        elif dirs == 'GDML_src':
            gdml_src_dir = os.path.join(global_input_dir, dirs)
            print('GDML_src directory: {}'.format(gdml_src_dir))
    
    Number_of_files = len(os.listdir(dst_dir))
    # The script, inside DST, automatically generates folders for each GDML file:
    # GDML_file_0
    # GDML_file_1
    # ...
    # GDML_file_N where N is the number of files in the GDML_ForSimulation directory, copied in the GDML_src
    
    if not os.path.exists(os.path.join(global_input_dir, 'Analysis_output')):
        if OnlyLatex:
            print('The Analysis_output directory does not exist. Please run the script without the -L option')
            sys.exit()
        else:
            os.makedirs(os.path.join(global_input_dir, 'Analysis_output'))
    else:
        if not OnlyLatex:
            print("OnlyLatex is False: I need to re run the ROOT processing. I need to clean the Analysis_output directory")
            input_answer = ''
            if BypassRemoval:
                input_answer = 'y'
            else:
                while (input_answer != 'y') and (input_answer != 'n'):
                    input_answer = input('The Analysis_output directory already exists' + '\n' + 'Do you want to overwrite it? [y/n]')
                    if input_answer != 'y' and input_answer != 'n':
                        print('Please insert only y or n')
        
        
            if input_answer == 'y':
                shutil.rmtree(os.path.join(global_input_dir, 'Analysis_output'))
                os.makedirs(os.path.join(global_input_dir, 'Analysis_output'))
            else: 
                sys.exit()
        else:
            print("OnlyLatex is True: I don't need to re run the ROOT processing. I don't need to clean the Analysis_output directory")        

    
    # Create a txt file in which I write the bash commands of the analysis
    if not OnlyLatex:
        f_CMD = open(os.path.join(global_input_dir, 'Analysis_output', 'CMD.txt'), 'w')
    
    
    # Create subdirectories for each GDML file
    if not OnlyLatex:
        for i in range(Number_of_files):
            os.makedirs(os.path.join(global_input_dir, 'Analysis_output', 'GDML_file_{}'.format(i)))


    # From the config.txt file retrieve how many particles do I expect to analyze
    f_config = open(os.path.join(global_input_dir, 'config.txt'), 'r')
    ParticleName = []
    Emin = []
    Emax = []
    for line in f_config:
        line_array = line.split(' ')
        ParticleName.append(line_array[0])
        Emin.append(float(line_array[1]))
        Emax.append(float(line_array[2]))

    f_config.close()    
    
    
    NFiles = len(ParticleName)
    Emin_plots = np.array(Emin).min()
    Emax_plots = np.array(Emax).max()
    
    
    #  Change permissions of the files
    # os.system('chmod 777 -R ' + global_input_dir)
    # os.system('chmod 777 -R ' + dst_dir)
    # os.system('chmod 777 -R ' + gdml_src_dir)
    # os.system('chmod 777 -R ' + os.path.join(global_input_dir, 'Analysis_output'))
    
    # ######################################################## #
    #                  HADD OF THE ROOT FILES                  #
    # ######################################################## #

    # Prepare the FileNames.txt file for each GDML_file_N directory
    # Prepare the EnergyGen.txt file for each GDML_file_N directory
    # This part is common to the ROOT processing and to the latex report generation
    for root, dirs, files in os.walk(dst_dir):
        if 'GDML_file_' in root:
            f_FileNames = open(os.path.join(root, 'FileNames.txt'), 'w')
            f_EnergyGen = open(os.path.join(root, 'EnergyGen.txt'), 'w')
            for ii in range(len(Emin)):
                f_EnergyGen.write(str(Emin[ii]) + ' ' + str(Emax[ii]) + '\n')
            f_EnergyGen.close()
            
            for particle in ParticleName:
                TargetName = os.path.join(root, particle + '_t0.root')
                f_FileNames.write(TargetName + '\n')
            f_FileNames.close()



    HADD_to_apply = False
    
    # os.system('chmod 777 -R ' + global_input_dir)


    if not OnlyLatex:
        for root, dirs, files in os.walk(dst_dir):
            if 'GDML_file_' in root:
                pattern = r"/GDML_file_(\d+)"
                match = re.search(pattern, root)
                if match is not None:
                    number = int(match.group(1))
                else:
                    number = -1
                    
                HADD_to_apply = False
                # Look if there are files such as: particle_j0_t0.root particle_j1_t0.root
                for file in files:
                    if '_j' in file:
                        HADD_to_apply = True
                        print('Filse processed by Cluter\nMerging files...')
                        break
                
                # Ok, but if there is already a file called particle_t0.root, I don't need to merge anything
                if HADD_to_apply and not CleanROOTFiles:
                    CounterFilesMerged = 0
                    for particle in ParticleName:
                        if os.path.exists(os.path.join(root, particle + '_t0.root')):
                            CounterFilesMerged += 1
                    if CounterFilesMerged == len(ParticleName):
                        HADD_to_apply = False
                        print('All files already merged')
                
                                        
                if HADD_to_apply:
                    for particle in ParticleName:            
                        FilesToMerge = []
                        for file in files:
                            if particle in file and '_j' in file and file.endswith(".root"):
                                FilesToMerge.append(os.path.join(root, file))
                        if len(FilesToMerge) > 0:
                            TargetName = os.path.join(root, particle + '_t0.root')
                            CMD_TO_EXECUTE = 'hadd -f ' + os.path.join(root, TargetName)
                            for file in FilesToMerge:
                                CMD_TO_EXECUTE += ' ' + os.path.join(root, file)
                            print(CMD_TO_EXECUTE)
                            f_CMD.write(CMD_TO_EXECUTE + '\n\n')
                            os.system(CMD_TO_EXECUTE)
                            #input('Press enter to continue')
                
                
                
                Command = ''
                ROOT_MacroName = os.path.join(root_dir, 'SetAliases.C')
                ArgVect = []

                PrefixRootFiles = os.path.join(dst_dir,'GDML_file_' + str(number))
                PrefixRootFilesAlias = os.path.join(global_input_dir, 'Analysis_output', 'GDML_file_' + str(number))
                

                ListOfRootFiles = []
                # open file FileNames.txt
                with open(os.path.join(PrefixRootFiles, 'FileNames.txt'), 'r') as f:
                    for line in f:
                        ListOfRootFiles.append(line.rstrip('\n'))
                        print(line.rstrip('\n'))
                
                fNamesAlias = open(os.path.join(PrefixRootFilesAlias, 'FileNames.txt'), 'w')

                for rootFileName in ListOfRootFiles:
                    if rootFileName.endswith(".root"):
                        baseName = rootFileName.split('/')[-1]
                        aliasName = baseName.replace(".root", "_alias.root")
                        fNamesAlias.write(os.path.join(PrefixRootFilesAlias, aliasName) + '\n')
                        print(os.path.join(PrefixRootFilesAlias, aliasName))
                fNamesAlias.close()
            
                for index, file in enumerate(ListOfRootFiles):
                    print("Processing file: {}".format(file))
                    ArgVect = []
                    ArgVect.append(os.path.join(root,file.split('/')[-1]))
                    ArgVect.append(file.split('/')[-1].replace(".root", ""))
                    ArgVect.append(os.path.join(global_input_dir, 'Analysis_output', 'GDML_file_{}'.format(number)))
                    ArgVect.append(E_thr_Thin)
                    ArgVect.append(E_thr_Thick)
                    ArgVect.append(E_thr_Plastic)
                    Command = ROOT_CMD(ROOT_MacroName, ArgVect)
                    print(Command)
                    f_CMD.write(Command + '\n\n')
                    return_code = os.system(Command)
                    if return_code != 0:
                        print("Error in SetAliases.C")
                        exit(1)
                    
                    # Remove the extension from the file name
                    fcsvName = file.split('/')[-1].replace(".root", "") + '_Hits.csv'
                    PlotViolation(os.path.join(global_input_dir, 'Analysis_output', 'GDML_file_{}'.format(number),'EnergyViolation3D',fcsvName))
                    
                    
                # Retrieve the information about the positions of the silicon detectors and dump them in a file in the AnalysisOutput/GDML_file_n directory
                
                for fileName in os.listdir(os.path.join(global_input_dir,"Text_output", "GDML_file_"+str(number))):
                    if "SiliconPosition" in fileName:
                        f_SiliconPosition = open(os.path.join(global_input_dir,"Text_output", "GDML_file_"+str(number), fileName), 'r')
                        f_SiliconPosition_out = open(os.path.join(PrefixRootFilesAlias, "SiliconPosition.txt"), 'w')
                        for line in f_SiliconPosition:
                            line_split = line.replace('\n','').split(' ')
                            if "SiliconDetector" in line_split[0]:
                                if "Thick" in line_split[0]:
                                    ThinThick = 1
                                elif "Thin" in line_split[0]:
                                    ThinThick = 0
                                    
                                if '_0' in line_split[0]:
                                    Area = 55.0
                                    Radius = np.sqrt(Area/3.14159265359)
                                else:
                                    Area = 150.0
                                    Radius = np.sqrt(Area/3.14159265359) 
                                
                                # The last character of the first word is the number of the detector. Find this number integer
                                # Use re
                                pattern = r"(\d+)"
                                match = re.search(pattern, line_split[0])
                                if match is not None:
                                    IdDetector = int(match.group(1))    
                                
                                f_SiliconPosition_out.write(str(ThinThick) + ' '+ str(IdDetector) + ' ' + line_split[1] + ' ' + line_split[2] + ' ' + line_split[3]+ ' ' + str(Radius) + '\n')   
                        f_SiliconPosition.close()
                        f_SiliconPosition_out.close()
                        break
                
                Command = ''
                ROOT_MacroName = os.path.join(root_dir, 'PID.C')
                ArgVect = []
                ArgVect.append(NFiles)
                ArgVect.append(os.path.join(PrefixRootFilesAlias, 'FileNames.txt'))
                ArgVect.append(os.path.join(PrefixRootFiles, 'EnergyGen.txt'))
                ArgVect.append(PrefixRootFilesAlias)
                ArgVect.append(Emin_plots)
                ArgVect.append(Emax_plots)
                ArgVect.append(E_thr_Thin)
                ArgVect.append(E_thr_Thick)
                ArgVect.append(E_thr_Plastic)
                ArgVect.append(os.path.join(PrefixRootFilesAlias, "SiliconPosition.txt"))
                Command = ROOT_CMD(ROOT_MacroName, ArgVect)            
                print(Command)
                f_CMD.write(Command + '\n\n')
                return_code = os.system(Command)
                if return_code != 0:
                    print("Error in PID.C")
                    exit(1)
                    
                if SendTelegramMessage:
                    DirectoryWithPictures_startWalk = os.path.join(global_input_dir, 'Analysis_output', 'GDML_file_' + str(number))
                    ###### GET THE NAME OF THE GDML FILE ####
                    f_Report = open(os.path.join(global_input_dir,'report.txt'))
                    
                    lineOld = ''
                    for line in f_Report:
                        if ("File "+str(number)) in lineOld:
                            FileNameGDML = line.rstrip('\n')
                            break                
                        lineOld = line
                    MSG_txt = "Analysis of the GDML file: " + FileNameGDML + " completed.\nSending plots to Telegram."
                    SendMessage(MSG_txt)
                    
                    for root2, dir2, files2 in os.walk(DirectoryWithPictures_startWalk):
                        # If in the actual folder there are pictures send a message with the directory  and then the plots
                        if len(files2) > 0:
                            MSG_txt = "Directory: " + root2
                            SendMessage(MSG_txt)
                            for file in files2:
                                if file.endswith(".png"):
                                    SendPhoto(os.path.join(root2, file))
                                    time.sleep(2)                
                    
                
            
        f_CMD.close()
            
            
            
            
    if not OnlyRoot:   
        for root, dirs, files in os.walk(os.path.join(global_input_dir, 'Analysis_output')):
            for dir in dirs:
                pattern = r"/GDML_file_(\d+)"
                match = re.search(pattern, os.path.join(root, dir))
                if match is not None:
                    number = int(match.group(1))
                else:
                    number = -1
                    
                PDF_images_dir = os.path.join(global_input_dir, 'Analysis_output', 'GDML_file_{}'.format(number))
                if not os.path.exists(PDF_images_dir):
                    # Skip the directory
                    continue
                
                LatexReportDir = os.path.join(global_input_dir, 'LatexReport', 'GDML_file_{}'.format(number))
                if not os.path.exists(LatexReportDir):
                    os.makedirs(LatexReportDir)
                else:
                    shutil.rmtree(LatexReportDir)
                    os.makedirs(LatexReportDir)
                
                
                # Retrieve the list of root files
                PrefixRootFiles = os.path.join(dst_dir,'GDML_file_' + str(number))
                PrefixRootFilesAlias = os.path.join(global_input_dir, 'Analysis_output', 'GDML_file_' + str(number))
                
                ListOfRootFiles = []
                # open file FileNames.txt
                with open(os.path.join(PrefixRootFiles, 'FileNames.txt'), 'r') as f:
                    for line in f:
                        ListOfRootFiles.append(line.rstrip('\n'))
                        print(line.rstrip('\n'))
                
                fNamesAlias = open(os.path.join(PrefixRootFilesAlias, 'FileNames.txt'), 'w')

                for rootFileName in ListOfRootFiles:
                    if rootFileName.endswith(".root"):
                        baseName = rootFileName.split('/')[-1]
                        aliasName = baseName.replace(".root", "_alias.root")
                        fNamesAlias.write(os.path.join(PrefixRootFilesAlias, aliasName) + '\n')
                        print(os.path.join(PrefixRootFilesAlias, aliasName))
                fNamesAlias.close()

                        
                # File Names without extension
                fNames_noExt = []
                for items in ListOfRootFiles:
                    fNames_noExt.append(items.split('/')[-1].replace(".root", ""))

                ###### GET THE NAME OF THE GDML FILE ####
                f_Report = open(os.path.join(global_input_dir,'report.txt'))
                
                lineOld = ''
                for line in f_Report:
                    if ("File "+str(number)) in lineOld:
                        FileNameGDML = line.rstrip('\n')
                        break                
                    lineOld = line

                    
                # Creazione del report
                Report = LatexDocumentClass()
                #Settings
                Report.SetNameOfTheDocument('Report_File_'+str(number) + "_" + FileNameGDML)
                Report.SetTitle('GEANT4 Simulation Report')
                Report.SetAuthor("Riccardo Nicolaidis")
                Report.SetEmail("riccardo.nicolaidis@unitn.it")
                Report.SetOutputDirectory(LatexReportDir)
                Report.SetDocumentClass("beamer")

                
                ScalePicture = 1.
                
                Report.BeginSlide("Info")
                
                

                # Adding to the slide the name of the GDML file
                Report.Body += r'''
                \centering
                GDML File Name : \textbf{ '''+ FileNameGDML + r'''}
                
                '''
                
                
                # Dumping the NTuples of the file
                f_NTuple_Info = None
                f_MaterialInfo = None
                # Retrieving the file with the .txt report                
                for fileName in os.listdir(os.path.join(global_input_dir,"Text_output", "GDML_file_"+str(number))):
                    if "NTupleInfo" in fileName:
                        f_NTuple_Info = open(os.path.join(global_input_dir,"Text_output", "GDML_file_"+str(number), fileName))
                        break
            
                for fileName in os.listdir(os.path.join(global_input_dir,"Text_output", "GDML_file_"+str(number))):
                    if "MaterialInfo" in fileName:
                        f_MaterialInfo = open(os.path.join(global_input_dir,"Text_output", "GDML_file_"+str(number), fileName))
                        break
            

                # Dumping in an itemize
                
                Report.Body += r'''
                \vspace{2 cm}
                \textbf{NTuple Info}:
                \vspace{1 cm}
                '''
                Report.BeginItemize()
                for line in f_NTuple_Info:
                    if "Ntuple ID" in line:
                        Report.Item(line.rstrip('\n'))
                Report.EndItemize()               
                Report.EndSlide()
                

                Report.BeginSlide("Material Info")
              
                Report.BeginTable([" Volume ", " Material ", " Mass (g) "])
                for line in f_MaterialInfo:
                    if "Volume " in line:
                        line2 = line.rstrip("\n").replace("Volume ", "").replace("Mass", "").replace("Material", "")
                        Report.AddRow(line2.split(":"))
                Report.EndTable()

                
                Report.EndSlide()
                
                
                print("Inserting Slide With PID performances")
                
                Report.BeginSlide("PID")
                for pictures in os.listdir(os.path.join(PDF_images_dir,"PID_plots")):
                    if "PID.pdf" in pictures:
                        Caption = "PID, No Gaussian Smearing, Total Energy is the Energy reconstructed."
                        Report.InsertFigure(os.path.join(PDF_images_dir,"PID_plots",pictures), Caption, ScalePicture)
                        print("Inserted figure: {}".format(pictures))
                        break
                Report.EndSlide()
                
                Report.BeginSlide("PID No Calorimeter")
                for pictures in os.listdir(os.path.join(PDF_images_dir,"PID_plots")):
                    if "PID_NoCalo.pdf" in pictures:
                        Caption = "PID, No Gaussian Smearing, Total Energy is the Energy reconstructed, No Calorimeter."
                        Report.InsertFigure(os.path.join(PDF_images_dir,"PID_plots",pictures), Caption, ScalePicture)
                        print("Inserted figure: {}".format(pictures))
                        break
                Report.EndSlide()
                
                
                Report.BeginSlide("PID - Non Confinement Violation Factor - NOK")
                for pictures in os.listdir(os.path.join(PDF_images_dir,"PID_plots")):
                    if "PID_NCVF_NOK.pdf" in pictures:
                        Caption = "PID. Convined events but Measured energy is not equal to the MC energy."
                        Report.InsertFigure(os.path.join(PDF_images_dir,"PID_plots",pictures), Caption, 0.5)
                        print("Inserted figure: {}".format(pictures))
                        break
                Report.EndSlide()

                Report.BeginSlide("PID - Non Confinement Violation Factor - OK")
                for pictures in os.listdir(os.path.join(PDF_images_dir,"PID_plots")):
                    if "PID_NCVF_OK.pdf" in pictures:
                        Caption = "PID. Convined events and Measured energy is equal to the MC energy."
                        Report.InsertFigure(os.path.join(PDF_images_dir,"PID_plots",pictures), Caption, 0.5)
                        print("Inserted figure: {}".format(pictures))
                        break
                Report.EndSlide()
                
                Report.BeginSlide("PID - Non Confinement Violation Factor - NOK - No Calorimeter")
                for pictures in os.listdir(os.path.join(PDF_images_dir,"PID_plots")):
                    if "PID_NCVF_NOK_NoCalo.pdf" in pictures:
                        Caption = "PID. Convined events but Measured energy is not equal to the MC energy. No Calorimeter."
                        Report.InsertFigure(os.path.join(PDF_images_dir,"PID_plots",pictures), Caption, 0.5)
                        print("Inserted figure: {}".format(pictures))
                        break
                Report.EndSlide()
                
                Report.BeginSlide("PID - Non Confinement Violation Factor - OK - No Calorimeter")
                for pictures in os.listdir(os.path.join(PDF_images_dir,"PID_plots")):
                    if "PID_NCVF_OK_NoCalo.pdf" in pictures:
                        Caption = "PID. Convined events and Measured energy is equal to the MC energy. No Calorimeter."
                        Report.InsertFigure(os.path.join(PDF_images_dir,"PID_plots",pictures), Caption, 0.5)
                        print("Inserted figure: {}".format(pictures))
                        break
                Report.EndSlide()
                                                
                
                Report.BeginSlide("PID MC Energy")
                for pictures in os.listdir(os.path.join(PDF_images_dir,"PID_plots")):
                    if "PID2.pdf" in pictures:
                        Caption = "PID, No Gaussian Smearing, Total Energy is the MC Energy."
                        Report.InsertFigure(os.path.join(PDF_images_dir,"PID_plots",pictures), Caption, ScalePicture)
                        print("Inserted figure: {}".format(pictures))
                        break
                Report.EndSlide()
                
                Report.BeginSlide("PID MC Energy No Calorimeter")
                for pictures in os.listdir(os.path.join(PDF_images_dir,"PID_plots")):
                    if "PID2_NoCalo.pdf" in pictures:
                        Caption = "PID, No Gaussian Smearing, Total Energy is the MC Energy, No Calorimeter."
                        Report.InsertFigure(os.path.join(PDF_images_dir,"PID_plots",pictures), Caption, ScalePicture)
                        print("Inserted figure: {}".format(pictures))
                        break
                Report.EndSlide()
                
                # Now all the plots with gaussian smearing.
                # gPID.pdf, gPID_NoCalo.pdf and so on.
                
                Report.BeginSlide("PID Gaussian Smearing")
                for pictures in os.listdir(os.path.join(PDF_images_dir,"PID_plots")):
                    if "gPID.pdf" in pictures:
                        Caption = "PID, Gaussian Smearing, Total Energy is the Energy reconstructed."
                        Report.InsertFigure(os.path.join(PDF_images_dir,"PID_plots",pictures), Caption, ScalePicture)
                        print("Inserted figure: {}".format(pictures))
                        break
                Report.EndSlide()
                
                Report.BeginSlide("PID Gaussian Smearing No Calorimeter")
                for pictures in os.listdir(os.path.join(PDF_images_dir,"PID_plots")):
                    if "gPID_NoCalo.pdf" in pictures:
                        Caption = "PID, Gaussian Smearing, Total Energy is the Energy reconstructed, No Calorimeter."
                        Report.InsertFigure(os.path.join(PDF_images_dir,"PID_plots",pictures), Caption, ScalePicture)
                        print("Inserted figure: {}".format(pictures))
                        break
                Report.EndSlide()
                
                Report.BeginSlide("PID MC Energy Gaussian Smearing")
                for pictures in os.listdir(os.path.join(PDF_images_dir,"PID_plots")):
                    if "gPID2.pdf" in pictures:
                        Caption = "PID, Gaussian Smearing, Total Energy is the MC Energy."
                        Report.InsertFigure(os.path.join(PDF_images_dir,"PID_plots",pictures), Caption, ScalePicture)
                        print("Inserted figure: {}".format(pictures))
                        break
                Report.EndSlide()
                
                Report.BeginSlide("PID MC Energy Gaussian Smearing No Calorimeter")
                for pictures in os.listdir(os.path.join(PDF_images_dir,"PID_plots")):
                    if "gPID2_NoCalo.pdf" in pictures:
                        Caption = "PID, Gaussian Smearing, Total Energy is the MC Energy, No Calorimeter."
                        Report.InsertFigure(os.path.join(PDF_images_dir,"PID_plots",pictures), Caption, ScalePicture)
                        print("Inserted figure: {}".format(pictures))
                        break
                Report.EndSlide()
                
                
                Report.BeginSlide("PID graphs")
                for pictures in os.listdir(os.path.join(PDF_images_dir,"PID_plots")):
                    if "graph_PID_center.pdf" in pictures:
                        Caption = "PID, Gaussian Smearing, Total Energy is the Energy reconstructed, No Calorimeter."
                        Report.InsertFigure(os.path.join(PDF_images_dir,"PID_plots",pictures), Caption, ScalePicture)
                        print("Inserted figure: {}".format(pictures))
                        break
                Report.EndSlide()
                
                
                for index, file in enumerate(fNames_noExt):
                    Report.BeginSlide("MC quantities for " + file)
                    for mcfile in os.listdir(PDF_images_dir):
                        if ("Montecarlo" in mcfile) and (file.replace(".root", "") in mcfile):
                            Report.InsertFigure(os.path.join(PDF_images_dir, mcfile), "MC quantities", ScalePicture)
                            print("Inserted figure: {}".format(mcfile))
                    Report.EndSlide()
                    
                    Report.BeginSlide("Energies distribution for {}".format(ParticleName[index]))
                    for mcfile in os.listdir(PDF_images_dir):
                        if ("Energies" in mcfile) and (file.replace(".root", "") in mcfile):
                            Report.InsertFigure(os.path.join(PDF_images_dir, mcfile), "Detected energies", ScalePicture)
                            print("Inserted figure: {}".format(mcfile))
                    Report.EndSlide()
                    
                    Report.BeginSlide("Angles distribution accepted for {}".format(ParticleName[index]))                
                    for mcfile in os.listdir(PDF_images_dir):
                        if ("Angles" in mcfile) and (file.replace(".root", "") in mcfile):
                            Report.InsertFigure(os.path.join(PDF_images_dir, mcfile), "Angles distribution", ScalePicture)
                            print("Inserted figure: {}".format(mcfile))
                    Report.EndSlide()
                    
                    print("Histograms for {}".format(ParticleName[index]))
                    name_histo2D = []
                    name_PositionGen = []
                    
                    
                    for imgHisto2D in os.listdir(PDF_images_dir):
                        if  ("2DAngHistoFigure_" in imgHisto2D) and (file.replace(".root", "") in imgHisto2D):
                            name_histo2D.append(imgHisto2D)
                            print(imgHisto2D)
                        if  ("GenPosition_" in imgHisto2D) and (file.replace(".root", "") in imgHisto2D):
                            name_PositionGen.append(imgHisto2D)
                            print(imgHisto2D)
                    
                    print("Inserting the slides for the 2D histograms")
                    for index_img, name_img  in enumerate(name_histo2D):
                        for j in range(len(name_histo2D)):
                            if ("2DAngHistoFigure_{}".format(j) ) in name_img:
                                print("Inserting the slide for the 2D histogram {}".format(name_img))
                                Report.BeginSlide("Angles distribution accepted for {}".format(ParticleName[index]))
                                Report.InsertFigure(os.path.join(PDF_images_dir, name_img), "Angles distribution", ScalePicture)
                                Report.EndSlide()
                        
                    print("Inserting the slides for the generation position")            
                    for index_img, name_img  in enumerate(name_PositionGen):
                        for j in range(len(name_PositionGen)):
                            if ("GenPosition_{}".format(j) ) in name_img:
                                print("Inserting the slide for the generation position {}".format(name_img))
                                Report.BeginSlide("Generation Position distribution accepted for {}".format(ParticleName[index]))
                                Report.InsertFigure(os.path.join(PDF_images_dir, name_img), "Generation Position", ScalePicture)
                                Report.EndSlide()
                    
                    
                    Report.BeginSlide("Geometric factors for {}".format(ParticleName[index]))
                    for mcfile in os.listdir(PDF_images_dir):
                        print(mcfile)
                        if ("Acceptances" in mcfile) and (file.replace(".root", "") in mcfile):
                            Report.InsertFigure(os.path.join(PDF_images_dir, mcfile), "Geometric factors", ScalePicture)
                            print("Inserted figure: {}".format(mcfile))                
                    Report.EndSlide()
                    
                    
                    Report.BeginSlide("Geometric factors for {}".format(ParticleName[index]))
                    for mcfile in os.listdir(PDF_images_dir):
                        print(mcfile)
                        if "ChannelGeomFactor" in mcfile and file.replace(".root", "") in mcfile:
                            Report.InsertFigure(os.path.join(PDF_images_dir, mcfile), "Geometric factors", ScalePicture)
                            print("Inserted figure: {}".format(mcfile))
                    Report.EndSlide()
                    
                    
                Report.BeginSlide("Geometric factors for all particles")
                for mcfile in os.listdir(PDF_images_dir):
                    if "GeomAll" in mcfile:
                        Report.InsertFigure(os.path.join(PDF_images_dir, mcfile), "Geometric factors", ScalePicture)                         
                Report.EndSlide()
                
                
                
                
                
                
                print("Now compiling the report")
                Report.Compile()
                
                for files in os.listdir(LatexReportDir):
                    if files.endswith('.pdf'):
                        shutil.copy(os.path.join(LatexReportDir, files), os.path.join(global_input_dir,'LatexReport'))
    
    
    
    # In the end reorder the pictures creating subforlders with the name of the extension
    for root, dirs, files in os.walk(os.path.join(global_input_dir, 'Analysis_output')):
        FoundImages = False
        for file in files:
            if file.endswith(".png") or file.endswith(".pdf") or file.endswith(".root"):
                FoundImages = True
                break                    
        
        if FoundImages:
            # Make dirs Png, Pdf, Root
            if not os.path.exists(os.path.join(root, 'Png')):
                os.makedirs(os.path.join(root, 'Png'))
            if not os.path.exists(os.path.join(root, 'Pdf')):
                os.makedirs(os.path.join(root, 'Pdf'))
            if not os.path.exists(os.path.join(root, 'Root')):
                os.makedirs(os.path.join(root, 'Root'))

            for file in files:
                if file.endswith(".png"):
                    shutil.move(os.path.join(root, file), os.path.join(root, 'Png'))
                if file.endswith(".pdf"):
                    shutil.move(os.path.join(root, file), os.path.join(root, 'Pdf'))
                if file.endswith(".root"):
                    shutil.move(os.path.join(root, file), os.path.join(root, 'Root'))
                    
                
            
                
        
        
if __name__ == "__main__":
    
    
    
    
    parser = argparse.ArgumentParser(description='Script for the analysis of the simulation data of the Geant4 simulation for the Low Energy Module LEM')
    parser.add_argument('-i', '--input-dir', type=str, help='Input directory of the simulation data')
    parser.add_argument('-L', '--latex-only', action='store_true', help='Only the latex report is generated')
    parser.add_argument('-R', '--root-only', action='store_true', help='Only the root files are analyzed')
    parser.add_argument('-b', '--bypass-removal', action='store_true', help='Bypass the removal of the Analysis_output directory')
    parser.add_argument('-T', '--telegram', action='store_true', help='Send a telegram message when the analysis is finished')
    parser.add_argument('-C', '--clean', action='store_true', help='Clean the Analysis_output directory')


    args = parser.parse_args()
    actual_dir = os.getcwd()

    OnlyLatex = args.latex_only
    OnlyRoot = args.root_only
    BypassRemoval = args.bypass_removal

    
    if OnlyLatex and OnlyRoot:
        print('You cannot select both -L and -R options')
        sys.exit()
        
    os.chdir(args.input_dir)
    input_dir = os.getcwd()
    os.chdir(actual_dir)
    
    #input_dir = "/data1/home/rnicolai/LEM_GDML_upgrade/Output_Geant4Simulation_PID_20230702"
    
    print('Input directory: ' + input_dir)
        
    
    Analysis(input_dir, OnlyLatex, OnlyRoot, BypassRemoval, args.telegram, args.clean)

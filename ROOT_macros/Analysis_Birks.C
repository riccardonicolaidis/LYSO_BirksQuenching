#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include "TCanvas.h"
#include "TFile.h"
#include "TTree.h"
#include "TChain.h"

using namespace std;

int Analysis_Birks()
{
    TString FileName = "/data1/home/rnicolai/LYSO_BirksQuenching/OutputFiles/ProvaFile_2.root";
    TFile *f = new TFile(FileName);
    TTree *Event = (TTree*)f->Get("Event");

    return 0;
}
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>

#include "TFile.h"
#include "TTree.h"
#include "TChain.h"
#include "TCanvas.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TROOT.h"

using namespace std;


int Reconstruction()
{
    // Open a .txt file named FileNames.txt and read the file names
    ifstream FileNames("FileNames.txt");
    string FileName;
    vector<string> FileNamesVector;
    while (FileNames >> FileName)
    {
        FileNamesVector.push_back(FileName);
        cout << FileName << endl;
    }
    FileNames.close();
    // Compute the number of files
    int NFiles = FileNamesVector.size();
    cout << "Number of files: " << NFiles << endl;





    TFile *f = new TFile("../DST/RunNoOptical.root");
    TTree *Event = (TTree*)f->Get("Event");
    TTree *Hits = (TTree*)f->Get("Hits");




    Hits -> Print();

    int EventID;
    TFile *f1 = new TFile("../DST/RunNoOpticalReco.root", "RECREATE");
    TTree *EventReco = new TTree("EventReco", "EventReco");
    EventReco -> Branch("Xreco", &Xreco, "Xreco/D");
    EventReco -> Branch("Yreco", &Yreco, "Yreco/D");
    EventReco -> Branch("Zreco", &Zreco, "Zreco/D");

    EventID = 0;

    for(int i = 0; i < Event -> GetEntries(); i++)
    {
        Event -> GetEntry(i);

        Xreco = 2*Xgen;
        Yreco = 2*Ygen;
        Zreco = 2*Zgen;

        EventReco -> Fill();
    }

    


    EventReco -> Write();

    f1 -> Close();
    f -> Close();


    return 0;
}

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>

#include "TCanvas.h"
#include "TFile.h"
#include "TTree.h"
#include "TChain.h"
#include "TGraph.h"
#include "TGraphErrors.h"
#include "TMultiGraph.h"
#include "TAxis.h"
#include "TStyle.h"
#include "TMath.h"
#include "TROOT.h"
#include "TSystem.h"
#include "TApplication.h"
#include "TString.h"
#include "TObjArray.h"
#include "TObjString.h"
#include "TH1D.h"

#include "TGraph2D.h"


using namespace std;


int AnalysisCalorimeter()
{
    TFile *f2 = new TFile("Run_2_MeV_Reconstructed.root", "RECREATE");

    TFile *f = new TFile("Run_2_MeV.root");


    TTree *Event = (TTree*)f->Get("Event");
    TTree *Hits = (TTree*)f->Get("Hits");

    Event -> Print();
    Hits -> Print();

    int NEvents = 1000-1;
    int NSiPM = 6;


    TString EventBranches[9];
    TString HitsBranches[5];

    EventBranches[0] = "IncidentEnergy";
    EventBranches[1] = "Xgen";
    EventBranches[2] = "Ygen";
    EventBranches[3] = "Zgen";
    EventBranches[4] = "pDirX";
    EventBranches[5] = "pDirY";
    EventBranches[6] = "pDirZ";
    EventBranches[7] = "event";
    EventBranches[8] = "EdepScint";

    HitsBranches[0] = "event";
    HitsBranches[1] = "CopyNumber";
    HitsBranches[2] = "X";
    HitsBranches[3] = "Y";
    HitsBranches[4] = "Z";


    double IncidentEnergy;
    double Xgen, Ygen, Zgen;
    double pDirX, pDirY, pDirZ;
    int eventFromEventTree;
    int eventFromHitsTree;
    int CopyNumber;
    double EdepScint;
    int Counts[NSiPM];
    int totalCounts = 0;

    Event->SetBranchAddress("IncidentEnergy", &IncidentEnergy);
    Event->SetBranchAddress("Xgen", &Xgen);
    Event->SetBranchAddress("Ygen", &Ygen);
    Event->SetBranchAddress("Zgen", &Zgen);
    Event->SetBranchAddress("pDirX", &pDirX);
    Event->SetBranchAddress("pDirY", &pDirY);
    Event->SetBranchAddress("pDirZ", &pDirZ);
    Event->SetBranchAddress("event", &eventFromEventTree);
    Event->SetBranchAddress("EdepScint", &EdepScint);

    Hits->SetBranchAddress("event", &eventFromHitsTree);
    Hits->SetBranchAddress("CopyNumber", &CopyNumber);



    for(int i = 0; i < NSiPM; i++)
    {
        Counts[i] = 0;
    }


    TTree *eventReconstruction = new TTree("eventReconstruction", "eventReconstruction");
    eventReconstruction->Branch("IncidentEnergy", &IncidentEnergy, "IncidentEnergy/D");
    eventReconstruction->Branch("Xgen", &Xgen, "Xgen/D");
    eventReconstruction->Branch("Ygen", &Ygen, "Ygen/D");
    eventReconstruction->Branch("Zgen", &Zgen, "Zgen/D");
    eventReconstruction->Branch("pDirX", &pDirX, "pDirX/D");
    eventReconstruction->Branch("pDirY", &pDirY, "pDirY/D");
    eventReconstruction->Branch("pDirZ", &pDirZ, "pDirZ/D");
    eventReconstruction->Branch("event", &eventFromEventTree, "event/I");
    eventReconstruction->Branch("EdepScint", &EdepScint, "EdepScint/D");
    for(int i = 0; i < NSiPM; i++)
    {
        eventReconstruction->Branch(Form("Counts%d", i), &Counts[i], Form("Counts%d/I", i));
    }
    eventReconstruction -> Branch("totalCounts", &totalCounts, "totalCounts/I");


    // Loop over all the entries of the Hits tree
    int EventOld = 0;

    for(long long int i = 0; i < Hits -> GetEntries(); ++i)
    {
        //cout << "i: " << i << endl;
        Hits -> GetEntry(i);
        Event -> GetEntry(EventOld);


        ++Counts[CopyNumber];

        if(EventOld != eventFromHitsTree)
        {
            //cout << "Event: " << eventFromEventTree << endl;
            if(EventOld != -1)
            {
                for(int j = 0; j < NSiPM; j++)
                {
                    totalCounts += Counts[j];
                }
                eventReconstruction->Fill();
                //cout << totalCounts << endl;
                for(int j = 0; j < NSiPM; j++)
                {
                    //cout << "Counts[" << j << "]: " << Counts[j] << endl;
                    Counts[j] = 0;
                    totalCounts = 0;
                }
            }
            
            EventOld = eventFromHitsTree;
        }
    }


    TCanvas *c1 = new TCanvas("c1", "c1", 800, 600);
    TH1D *h1 = new TH1D("h1", "h1", 500, 0, 50000);
    eventReconstruction->Draw("(totalCounts+0*rndm)>>h1", "", "");
    gPad->SetLogy();
    gPad-> SetGrid();

    TCanvas *c2 = new TCanvas("c2", "c2", 800, 600);
    TH1D *h2 = new TH1D("h2", "h2", 500, 0, 3.);
    eventReconstruction->Draw("EdepScint>>h2", "", "");
    gPad->SetLogy();
    gPad-> SetGrid();


    // Create a new TFile and save the TTree eventReconstruction
    eventReconstruction->Write();
    f2->Close();



    return 0;
}
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

#include "TGraph2D.h"

//#include "../include/myglobals.hh"

#define NX_SIPM 3
#define NY_SIPM 3
#define REPETITIONS 10
#define FILENAME "RunAlpha3MeV_8SiPM_Holes_30_30_10.root"
#define FILENAME2 "RunAlpha3MeV_8SiPM_Plane_30_30_10.root"
#define LX_SIPM 100
#define LY_SIPM 100
#define SEGMENTS_X 30
#define SEGMENTS_Y 30


using namespace std;

void AnalysisScintillator()
{
    // Open an existing ROOT file
    TFile *f = new TFile(FILENAME);
    TFile *f2 = new TFile(FILENAME2);
    int NEvents = 9000-1;
    // Get the tree from the file
    TTree *Event = (TTree*)f->Get("Event");
    TTree *Event2 = (TTree*)f2->Get("Event");
    TTree *Hits = (TTree*)f->Get("Hits");
    TTree *Hits2 = (TTree*)f2->Get("Hits");


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

    double X[NEvents], Y[NEvents];
    double X2[NEvents], Y2[NEvents];
    double Counts[2*(NX_SIPM + NY_SIPM)][NEvents];
    double Counts2[2*(NX_SIPM + NY_SIPM)][NEvents];

    int TotalNumberSensors = 2 * (NX_SIPM + NY_SIPM);


    for(int i = 0; i < NEvents; i++)
    {
        for(int j = 0; j < TotalNumberSensors; ++j)
        {
            Counts[j][i] = 0.;
            Counts2[j][i] = 0.;
        }
    }

    double Xgen, Ygen, Zgen;
    double Xgen2, Ygen2, Zgen2;

    Event -> SetBranchAddress(EventBranches[1], &Xgen);
    Event -> SetBranchAddress(EventBranches[2], &Ygen);
    Event -> SetBranchAddress(EventBranches[3], &Zgen);

    Event2 -> SetBranchAddress(EventBranches[1], &Xgen2);
    Event2 -> SetBranchAddress(EventBranches[2], &Ygen2);
    Event2 -> SetBranchAddress(EventBranches[3], &Zgen2);


    int event, CopyNumber;
    int event2, CopyNumber2;

    Hits -> SetBranchAddress(HitsBranches[0], &event);
    Hits -> SetBranchAddress(HitsBranches[1], &CopyNumber);

    Hits2 -> SetBranchAddress(HitsBranches[0], &event2);
    Hits2 -> SetBranchAddress(HitsBranches[1], &CopyNumber2);


    int j = 0;
    int j2 = 0;
    for(int i = 0; i < NEvents -1; ++i)
    {
        Event -> GetEntry(i);
        Event2 -> GetEntry(i);
        X[i] = Xgen;
        Y[i] = Ygen;
        X2[i] = Xgen2;
        Y2[i] = Ygen2;
        do
        {
            Hits -> GetEntry(j);
            Counts[CopyNumber][i] = Counts[CopyNumber][i] + 1.;
            ++j;
        } while ( event == i);
        do
        {
            Hits2 -> GetEntry(j2);
            Counts2[CopyNumber2][i] = Counts2[CopyNumber2][i] + 1.;
            ++j2;
        } while ( event2 == i);
        
       //cout << event <<" X = " << X[i] << " Y = " << Y[i] << " Counts = " << Counts[0][i] << " " << Counts[1][i] << " " << Counts[2][i] << " " << Counts[3][i] << endl;
    }

    // Sum all the sensors counts

    double CountsTotal[NEvents];
    double CountsTotal2[NEvents];

    for(int i = 0; i < NEvents; ++i)
    {
        CountsTotal[i] = 0.;
        CountsTotal2[i] = 0.;
        for(int j = 0; j < TotalNumberSensors; ++j)
        {
            CountsTotal[i] += Counts[j][i];
            CountsTotal2[i] += Counts2[j][i];
        }
    }


    int NumberPointsGrid = (SEGMENTS_X+1) * (SEGMENTS_Y+1);
    double XGrid[NumberPointsGrid], YGrid[NumberPointsGrid];

    double CountsSiPM_Mean[TotalNumberSensors][NumberPointsGrid];
    double CountsSiPM_SquareMean[TotalNumberSensors][NumberPointsGrid];
    double CountsSiPM_StdDev[TotalNumberSensors][NumberPointsGrid];
    double CountsTotal_Mean[NumberPointsGrid];
    double CountsTotal_SquareMean[NumberPointsGrid];
    double CountsTotal_StdDev[NumberPointsGrid];
    int NumberEntriesInSum[NumberPointsGrid];

    double CountsSiPM_Mean2[TotalNumberSensors][NumberPointsGrid];
    double CountsSiPM_SquareMean2[TotalNumberSensors][NumberPointsGrid];
    double CountsSiPM_StdDev2[TotalNumberSensors][NumberPointsGrid];
    double CountsTotal_Mean2[NumberPointsGrid];
    double CountsTotal_SquareMean2[NumberPointsGrid];
    double CountsTotal_StdDev2[NumberPointsGrid];
    int NumberEntriesInSum2[NumberPointsGrid];



    // Fill the grid XGrid and YGrid in a for loop

    double SegmentX = LX_SIPM / (SEGMENTS_X+1.);
    double SegmentY = LY_SIPM / (SEGMENTS_Y+1.);

    int k = 0;
    for(int i = 0; i < SEGMENTS_X; ++i)
    {
        for(int j = 0; j < SEGMENTS_Y; ++j)
        {
            XGrid[k] = -LX_SIPM/2. + SegmentX/2. +  j * SegmentX;
            YGrid[k] = -LY_SIPM/2. + SegmentY/2. +  i * SegmentY;
            ++k;
        }
    }
   

    // Fill the grid CountsSiPM_Mean, CountsSiPM_SquareMean, CountsSiPM_StdDev, CountsTotal_Mean, CountsTotal_SquareMean, CountsTotal_StdDev in a for loop

    for(int i = 0; i < NumberPointsGrid; ++i)
    {
        for(int j = 0; j < TotalNumberSensors; ++j)
        {
            CountsSiPM_Mean[j][i] = 0.;
            CountsSiPM_SquareMean[j][i] = 0.;
            CountsSiPM_StdDev[j][i] = 0.;

            CountsSiPM_Mean2[j][i] = 0.;
            CountsSiPM_SquareMean2[j][i] = 0.;
            CountsSiPM_StdDev2[j][i] = 0.;

        }
        CountsTotal_Mean[i] = 0.;
        CountsTotal_SquareMean[i] = 0.;
        CountsTotal_StdDev[i] = 0.;
        NumberEntriesInSum[i] = 0;

        CountsTotal_Mean2[i] = 0.;
        CountsTotal_SquareMean2[i] = 0.;
        CountsTotal_StdDev2[i] = 0.;
        NumberEntriesInSum2[i] = 0;

    }

    for(int i = 0; i < NEvents; ++i)
    {
        for(int j = 0; j < NumberPointsGrid; ++j)
        {
            if( (abs(X[i]-XGrid[j])<0.5) &&  (abs(Y[i] - YGrid[j])<0.5) )
            {
                cout << "Entered in the if" << endl;
                cout << "X[i] = " << X[i] << " Y[i] = " << Y[i] << " XGrid[j] = " << XGrid[j] << " YGrid[j] = " << YGrid[j] << endl;
                for(int k = 0; k < TotalNumberSensors; ++k)
                {
                    CountsSiPM_Mean[k][j] = CountsSiPM_Mean[k][j] + Counts[k][i];
                    CountsSiPM_SquareMean[k][j] = CountsSiPM_SquareMean[k][j] + Counts[k][i] * Counts[k][i];
                }
                CountsTotal_Mean[j] = CountsTotal_Mean[j] + CountsTotal[i];
                CountsTotal_SquareMean[j] = CountsTotal_SquareMean[j] + CountsTotal[i] * CountsTotal[i];
                ++NumberEntriesInSum[j];
            }

            if( (abs(X2[i]-XGrid[j])<0.5) &&  (abs(Y2[i] - YGrid[j])<0.5) )
            {
                cout << "Entered in the if" << endl;
                cout << "X2[i] = " << X2[i] << " Y2[i] = " << Y2[i] << " XGrid[j] = " << XGrid[j] << " YGrid[j] = " << YGrid[j] << endl;
                for(int k = 0; k < TotalNumberSensors; ++k)
                {
                    CountsSiPM_Mean2[k][j] = CountsSiPM_Mean2[k][j] + Counts2[k][i];
                    CountsSiPM_SquareMean2[k][j] = CountsSiPM_SquareMean2[k][j] + Counts2[k][i] * Counts2[k][i];
                }
                CountsTotal_Mean2[j] = CountsTotal_Mean2[j] + CountsTotal2[i];
                CountsTotal_SquareMean2[j] = CountsTotal_SquareMean2[j] + CountsTotal2[i] * CountsTotal2[i];
                ++NumberEntriesInSum2[j];
            }
        }
    }

    // Normalize the sums and compute the STD
    
    for(int i = 0; i < NumberPointsGrid; ++i)
    {
        for(int j = 0; j < TotalNumberSensors; ++j)
        {
            CountsSiPM_Mean[j][i] = CountsSiPM_Mean[j][i] / NumberEntriesInSum[i];
            CountsSiPM_SquareMean[j][i] = CountsSiPM_SquareMean[j][i] / NumberEntriesInSum[i];
            CountsSiPM_StdDev[j][i] = sqrt(CountsSiPM_SquareMean[j][i] - CountsSiPM_Mean[j][i] * CountsSiPM_Mean[j][i]);

            CountsSiPM_Mean2[j][i] = CountsSiPM_Mean2[j][i] / NumberEntriesInSum2[i];
            CountsSiPM_SquareMean2[j][i] = CountsSiPM_SquareMean2[j][i] / NumberEntriesInSum2[i];
            CountsSiPM_StdDev2[j][i] = sqrt(CountsSiPM_SquareMean2[j][i] - CountsSiPM_Mean2[j][i] * CountsSiPM_Mean2[j][i]);
        }
        CountsTotal_Mean[i] = CountsTotal_Mean[i] / NumberEntriesInSum[i];
        CountsTotal_SquareMean[i] = CountsTotal_SquareMean[i] / NumberEntriesInSum[i];
        CountsTotal_StdDev[i] = sqrt(CountsTotal_SquareMean[i] - CountsTotal_Mean[i] * CountsTotal_Mean[i]);

        CountsTotal_Mean2[i] = CountsTotal_Mean2[i] / NumberEntriesInSum2[i];
        CountsTotal_SquareMean2[i] = CountsTotal_SquareMean2[i] / NumberEntriesInSum2[i];
        CountsTotal_StdDev2[i] = sqrt(CountsTotal_SquareMean2[i] - CountsTotal_Mean2[i] * CountsTotal_Mean2[i]);

    }
    



    TCanvas *c1[TotalNumberSensors];
    TGraph2D *g1[TotalNumberSensors];

    TCanvas *c2[TotalNumberSensors];
    TGraph2D *g2[TotalNumberSensors];
    
    

    for(int i = 0; i < TotalNumberSensors; ++i)
    {
        c1[i] = new TCanvas(Form("c1_%d", i), Form("c1_%d", i), 800, 600);
        cout << "Printing canvas " << i << endl;
        g1[i] = new TGraph2D(NEvents-1, X, Y, Counts[i]);
        g1[i] -> SetTitle(Form("Counts in scintillator %d", i));
        g1[i] -> GetXaxis() -> SetTitle("X [mm]");
        g1[i] -> GetYaxis() -> SetTitle("Y [mm]");
        g1[i] -> GetZaxis() -> SetTitle("Counts");
        g1[i] -> Draw("PCOL");
        g1[i] -> SetMarkerStyle(20);
        g1[i] -> SetMarkerSize(0.9);

        c1[i] -> SaveAs(Form("../docs/assets/images/Counts_Scintillator_Holes_%d.pdf", i));
        c1[i] -> SaveAs(Form("../docs/assets/images/Counts_Scintillator_Holes_%d.png", i));

        c2[i] = new TCanvas(Form("c2_%d", i), Form("c2_%d", i), 800, 600);
        cout << "Printing canvas " << i << endl;
        g2[i] = new TGraph2D(NEvents-1, X2, Y2, Counts2[i]);
        g2[i] -> SetTitle(Form("Counts in scintillator No Holes%d", i));
        g2[i] -> GetXaxis() -> SetTitle("X [mm]");
        g2[i] -> GetYaxis() -> SetTitle("Y [mm]");
        g2[i] -> GetZaxis() -> SetTitle("Counts");
        g2[i] -> Draw("PCOL");
        g2[i] -> SetMarkerStyle(20);
        g2[i] -> SetMarkerSize(0.9);

        c2[i] -> SaveAs(Form("../docs/assets/images/Counts_Scintillator_NoHoles_%d.pdf", i));
        c2[i] -> SaveAs(Form("../docs/assets/images/Counts_Scintillator_NoHoles_%d.png", i));
        
    }



    TCanvas *ct = new TCanvas();
    TGraph2D *gt = new TGraph2D(NEvents-1, X, Y, CountsTotal);
    gt -> SetTitle("Counts in scintillator Total");
    gt -> GetXaxis() -> SetTitle("X [mm]");
    gt -> GetYaxis() -> SetTitle("Y [mm]");
    gt -> GetZaxis() -> SetTitle("Counts");
    gt -> Draw("PCOL");
    gt -> SetMarkerStyle(20);
    gt -> SetMarkerSize(0.9);

    ct -> SaveAs("../docs/assets/images/Counts_Scintillator_Total.pdf");
    ct -> SaveAs("../docs/assets/images/Counts_Scintillator_Total.png");


    TCanvas *ct2 = new TCanvas();
    TGraph2D *gt2 = new TGraph2D(NEvents-1, X2, Y2, CountsTotal2);
    gt2 -> SetTitle("Counts in scintillator No Holes Total");
    gt2 -> GetXaxis() -> SetTitle("X [mm]");
    gt2 -> GetYaxis() -> SetTitle("Y [mm]");
    gt2 -> GetZaxis() -> SetTitle("Counts");
    gt2 -> Draw("PCOL");
    gt2 -> SetMarkerStyle(20);
    gt2 -> SetMarkerSize(0.9);

    ct2 -> SaveAs("../docs/assets/images/Counts_Scintillator_NoHoles_Total.pdf");
    ct2 -> SaveAs("../docs/assets/images/Counts_Scintillator_NoHoles_Total.png");


    TCanvas *c3[TotalNumberSensors];
    TGraph2D *g3[TotalNumberSensors];

    for(int i = 0; i < TotalNumberSensors; ++i)
    {
        c3[i] = new TCanvas(Form("c3_%d", i), Form("c3_%d", i), 800, 600);
        cout << "Printing canvas " << i << endl;
        g3[i] = new TGraph2D(NumberPointsGrid, XGrid, YGrid, CountsSiPM_Mean[i]);
        g3[i] -> SetTitle(Form("Mean counts in SiPM %d", i));
        g3[i] -> GetXaxis() -> SetTitle("X [mm]");
        g3[i] -> GetYaxis() -> SetTitle("Y [mm]");
        g3[i] -> GetZaxis() -> SetTitle("Counts");
        g3[i] -> Draw("colz");
        g3[i] -> SetMarkerStyle(20);
        g3[i] -> SetMarkerSize(0.9);
        // Add colorbar
        //gStyle->SetPalette(1);
        gStyle->SetOptStat(0);

        c3[i] -> SaveAs(Form("../docs/assets/images/MeanCounts_Holes_SiPM_%d.pdf", i));
        c3[i] -> SaveAs(Form("../docs/assets/images/MeanCounts_Holes_SiPM_%d.png", i));
    }

    TCanvas *c32[TotalNumberSensors];
    TGraph2D *g32[TotalNumberSensors];

    for(int i = 0; i < TotalNumberSensors; ++i)
    {
        c32[i] = new TCanvas(Form("c32_%d", i), Form("c32_%d", i), 800, 600);
        cout << "Printing canvas " << i << endl;
        g32[i] = new TGraph2D(NumberPointsGrid, XGrid, YGrid, CountsSiPM_Mean2[i]);
        g32[i] -> SetTitle(Form("Mean counts in SiPM No Holes %d", i));
        g32[i] -> GetXaxis() -> SetTitle("X [mm]");
        g32[i] -> GetYaxis() -> SetTitle("Y [mm]");
        g32[i] -> GetZaxis() -> SetTitle("Counts");
        g32[i] -> Draw("colz");
        g32[i] -> SetMarkerStyle(20);
        g32[i] -> SetMarkerSize(0.9);
        // Add colorbar
        //gStyle->SetPalette(1);
        gStyle->SetOptStat(0);

        c32[i] -> SaveAs(Form("../docs/assets/images/MeanCounts_NoHoles_SiPM_%d.pdf", i));
        c32[i] -> SaveAs(Form("../docs/assets/images/MeanCounts_NoHoles_SiPM_%d.png", i));
    }


    TCanvas *c4 = new TCanvas();

    TGraph2D *g4 = new TGraph2D(NumberPointsGrid, XGrid, YGrid, CountsTotal_Mean);
    g4 -> SetTitle("Mean counts in scintillator Total");
    g4 -> GetXaxis() -> SetTitle("X [mm]");
    g4 -> GetYaxis() -> SetTitle("Y [mm]");
    g4 -> GetZaxis() -> SetTitle("Counts");
    g4 -> Draw("colz");
    g4 -> SetMarkerStyle(20);
    g4 -> SetMarkerSize(0.9);

    c4 -> SaveAs("../docs/assets/images/MeanCounts_Scintillator_Total.pdf");
    c4 -> SaveAs("../docs/assets/images/MeanCounts_Scintillator_Total.png");

    
    TCanvas *c42 = new TCanvas();

    TGraph2D *g42 = new TGraph2D(NumberPointsGrid, XGrid, YGrid, CountsTotal_Mean2);
    g42 -> SetTitle("Mean counts in scintillator No Holes Total");
    g42 -> GetXaxis() -> SetTitle("X [mm]");
    g42 -> GetYaxis() -> SetTitle("Y [mm]");
    g42 -> GetZaxis() -> SetTitle("Counts");
    g42 -> Draw("colz");
    g42 -> SetMarkerStyle(20);
    g42 -> SetMarkerSize(0.9);

    c42 -> SaveAs("../docs/assets/images/MeanCounts_Scintillator_NoHoles_Total.pdf");
    c42 -> SaveAs("../docs/assets/images/MeanCounts_Scintillator_NoHoles_Total.png");





    double RatioCounts[NumberPointsGrid];


    for(int i = 0; i < NumberPointsGrid; ++i)
    {
        RatioCounts[i] = CountsTotal_Mean[i]/CountsTotal_Mean2[i];
    }

    TCanvas *c5 = new TCanvas();
    TGraph2D *g5 = new TGraph2D(NumberPointsGrid, XGrid, YGrid, RatioCounts);
    g5 -> SetTitle("Ratio of counts in scintillator Total");
    g5 -> GetXaxis() -> SetTitle("X [mm]");
    g5 -> GetYaxis() -> SetTitle("Y [mm]");
    g5 -> GetZaxis() -> SetTitle("Ratio");
    g5 -> Draw("colz");
    g5 -> SetMarkerStyle(20);
    g5 -> SetMarkerSize(0.9);

    c5 -> SaveAs("../docs/assets/images/RatioCounts_Scintillator_Total.pdf");
    c5 -> SaveAs("../docs/assets/images/RatioCounts_Scintillator_Total.png");


    TCanvas *c6[TotalNumberSensors];
    TGraph2D *g6[TotalNumberSensors];

    double RatioCountsSiPM[TotalNumberSensors][NumberPointsGrid];

    for(int i = 0; i < TotalNumberSensors; ++i)
    {
        for(int j = 0; j < NumberPointsGrid; ++j)
        {
            RatioCountsSiPM[i][j] = CountsSiPM_Mean[i][j]/CountsSiPM_Mean2[i][j];
        }
    }

    for(int i = 0; i < TotalNumberSensors; ++i)
    {
        c6[i] = new TCanvas(Form("c6_%d", i), Form("c6_%d", i), 800, 600);
        cout << "Printing canvas " << i << endl;
        g6[i] = new TGraph2D(NumberPointsGrid, XGrid, YGrid, RatioCountsSiPM[i]);
        g6[i] -> SetTitle(Form("Ratio of counts in SiPM %d", i));
        g6[i] -> GetXaxis() -> SetTitle("X [mm]");
        g6[i] -> GetYaxis() -> SetTitle("Y [mm]");
        g6[i] -> GetZaxis() -> SetTitle("Ratio");
        g6[i] -> Draw("colz");
        g6[i] -> SetMarkerStyle(20);
        g6[i] -> SetMarkerSize(0.9);
        // Add colorbar
        //gStyle->SetPalette(1);
        gStyle->SetOptStat(0);

        c6[i] -> SaveAs(Form("../docs/assets/images/RatioCounts_SiPM_%d.pdf", i));
        c6[i] -> SaveAs(Form("../docs/assets/images/RatioCounts_SiPM_%d.png", i));
    }



}
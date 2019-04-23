#include <vector>

void makePlots() {

    std::vector<unsigned short> * t0 = 0;
    UShort_t Samples[1024], SamIndex[1024];
    Double_t Smooth[1024];
    TH1F * hMinsJ = new TH1F("hMinsJ", "hMinsJ", 1024, 0, 1024);
    TH1F * hMins = new TH1F("hMins", "hMins", 1024, 0, 4095);
    TH1F * hTemplate = new TH1F("hTemplate", "hTemplate", 1024, 0, 1023);
    bool isSignal = 0; UInt_t isBTF = 0;

    //TFile * fTraces = TFile::Open("/Users/afranken/PADME/ECAL_TemplateFitting/EventScreen2.5ns.root");
    TFile * fTraces = TFile::Open("/Users/afranken/PADME/ECAL_TemplateFitting/slimmed.root");
    TFile * fPlots = TFile::Open("/Users/afranken/PADME/ECAL_TemplateFitting/plots.root", "RECREATE");

    TTree * tTraces = (TTree*)fTraces->Get("slimmed");

    tTraces->SetBranchAddress("t0", &t0);
    tTraces->SetBranchAddress("Samples", Samples);
    tTraces->SetBranchAddress("Smooth", Smooth);
    tTraces->SetBranchAddress("SamIndex", SamIndex);
    tTraces->SetBranchAddress("IsSignal", &isSignal);
    tTraces->SetBranchAddress("IsBTF", &isBTF);

    Int_t numEntries = tTraces->GetEntries();


    for (int i = 0; i < numEntries; i++) {

        tTraces->GetEntry(i);

        TH1F * temp = new TH1F("temp", "temp", 1024, 0, 1023);

        for (int j = 0; j < 1024; j++) {
            temp->SetBinContent(j+1, Samples[j]);
        }

        fPlots->Write("temp");
    }

    fPlots->Close();
    fTraces->Close();

    return;
}

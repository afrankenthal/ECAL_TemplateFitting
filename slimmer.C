#include <vector>

void slimmer() {

    std::vector<unsigned short> * t0 = 0;
    UShort_t Samples[1024], SamIndex[1024];
    Double_t Smooth[1024];
    bool isSignal = 0; UInt_t isBTF = 0;
    UChar_t Channel, Board;
    Int_t Number;

    int numTraces = 0;

    TFile * fTraces = TFile::Open("/Volumes/DATA/padme_data/experiment/Screen.root");
    TFile * fSlimmed = TFile::Open("slimmed.root", "RECREATE");
   
    TTree * tSlimmed = new TTree("slimmed", "slimmed tree");
    tSlimmed->Branch("t0", &t0);
    tSlimmed->Branch("Samples", Samples, "Samples[1024]/s");
    tSlimmed->Branch("Smooth", Smooth, "Smooth[1024]/D");
    tSlimmed->Branch("SamIndex", SamIndex, "SamIndex[1024]/s");
    tSlimmed->Branch("IsSignal", &isSignal, "IsSignal/O");
    tSlimmed->Branch("IsBTF", &isBTF, "IsBTF/i");
    tSlimmed->Branch("Channel", &Channel, "Channel/b");
    tSlimmed->Branch("Board", &Board, "Board/b");
    tSlimmed->Branch("Number", &Number, "Number/I");

    TTree * tTraces = (TTree*)fTraces->Get("NTU");

    tTraces->SetBranchAddress("t0", &t0);
    tTraces->SetBranchAddress("Samples", Samples);
    tTraces->SetBranchAddress("Smooth", Smooth);
    tTraces->SetBranchAddress("SamIndex", SamIndex);
    tTraces->SetBranchAddress("IsSignal", &isSignal);
    tTraces->SetBranchAddress("IsBTF", &isBTF);
    tTraces->SetBranchAddress("Channel", &Channel);
    tTraces->SetBranchAddress("Board", &Board);
    tTraces->SetBranchAddress("Number", &Number);

    Int_t numEntries = tTraces->GetEntries();

    std::cout << "Slimming data..." << std::endl;
    for (int i = 0; i < numEntries; i++) {

        tTraces->GetEntry(i);

        if (!isSignal || !isBTF || Channel != 14 || Board != 20)
            continue;

        tSlimmed->Fill();
    }

    fSlimmed->Write("slimmed");

    fSlimmed->Close();
    fTraces->Close();

    return;
}

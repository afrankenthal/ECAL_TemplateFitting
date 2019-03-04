#include <vector>

void testTemplate();
void test2Fits();

double templateVec[1000];
TSpline3 * templateSpline;

Double_t fitf(Double_t * x, Double_t * par) {
    double arg = (x[0] - par[1])/par[3];
    return par[0] * templateSpline->Eval(arg) + par[2];
}

Double_t fitf2(Double_t * x, Double_t * par) {
    double arg1 = (x[0] - par[1])/par[2];
    double arg2 = (x[0] - par[4])/par[5];
    return par[0] * templateSpline->Eval(arg1) + par[3] * templateSpline->Eval(arg2) + par[6];
}

TH1F * hMinsJ;
TH1F * hMins;
TH1F * hTemplate;

TTree * tTraces;
TFile * fTraces;
std::vector<unsigned short> * t0;
std::vector<unsigned short> * PeakPos;
UShort_t Samples[1024], SamIndex[1024];
Double_t Smooth[1024];
bool isSignal; UInt_t isBTF;
Int_t numEntries;
Int_t Number, Npeaks;

void init() {
    fTraces = TFile::Open("/Users/afranken/PADME/ECAL_TemplateFitting/slimmed.root");

    tTraces = (TTree*)fTraces->Get("slimmed");

    tTraces->SetBranchAddress("t0", &t0);
    tTraces->SetBranchAddress("Samples", Samples);
    tTraces->SetBranchAddress("Smooth", Smooth);
    tTraces->SetBranchAddress("SamIndex", SamIndex);
    tTraces->SetBranchAddress("IsSignal", &isSignal);
    tTraces->SetBranchAddress("IsBTF", &isBTF);
		tTraces->SetBranchAddress("PeakPos", &PeakPos);
		tTraces->SetBranchAddress("Npeaks", &Npeaks);

    numEntries = tTraces->GetEntries();
}

void end() {
		fTraces->Close();
}

void makeTemplate() {

				init();
//    std::vector<unsigned short> * t0 = 0;
//    UShort_t Samples[1024], SamIndex[1024];
//    Double_t Smooth[1024];
    hMinsJ = new TH1F("hMinsJ", "hMinsJ", 1024, 0, 1024);
    hMins = new TH1F("hMins", "hMins", 1024, 0, 4095);
    hTemplate = new TH1F("hTemplate", "hTemplate", 1000, 0, 999);
		hMinsJ->SetDirectory(0);
		hMins->SetDirectory(0);
		hTemplate->SetDirectory(0);
//    bool isSignal = 0; UInt_t isBTF = 0;

    for (int j = 0; j < 1000; j++) templateVec[j] = 0;
    int numTraces[1000];
	 	for (int j = 0; j < 1000; j++) numTraces[j] = 0;

    //TFile * fTraces = TFile::Open("/Users/afranken/PADME/ECAL_TemplateFitting/EventScreen2.5ns.root");
    //TFile * fTraces = TFile::Open("/Users/afranken/PADME/ECAL_TemplateFitting/slimmed.root");

    //TTree * tTraces = (TTree*)fTraces->Get("slimmed");

    //tTraces->SetBranchAddress("t0", &t0);
    //tTraces->SetBranchAddress("Samples", Samples);
    //tTraces->SetBranchAddress("Smooth", Smooth);
    //tTraces->SetBranchAddress("SamIndex", SamIndex);
    //tTraces->SetBranchAddress("IsSignal", &isSignal);
    //tTraces->SetBranchAddress("IsBTF", &isBTF);

    //Int_t numEntries = tTraces->GetEntries();

    std::cout << "Building template..." << std::endl;
    for (int i = 0; i < numEntries; i++) {

        tTraces->GetEntry(i);

        if (isSignal&&isBTF) {
            int min = 10000;
            int minj = -1;

            double baseline = 0;
						for (int j = 0; j < 50; j++)
										baseline += Samples[j]/50;

            for (int j = 0; j < 1000; j++) {
                if (Samples[j] < min) {
                    min = Samples[j];
                    minj = j;
                }
            }
            if (min < 3000) {
                for (int j = 0; j < 1000; j++) {
                    if (j+(minj-400) > 0 && j+(minj-400) < 1000) {
                        // Double_t temp = Smooth[j+(minj-200)];
                        //temp = (baseline - temp)/(baseline - minj);
                        // temp = (4096 - temp)/4096;
												if (j < minj-30)
																templateVec[j] += baseline;
												else
																templateVec[j] += Samples[j+(minj-400)];
												numTraces[j]++;
												//templateVec[j] += Samples[j];
                        // templateVec[j] += temp;
                    }
                }
                //numTraces++;
            }

            hMinsJ->Fill(minj);
            hMins->Fill(min);
            // std::cout << "Min location: " << minj << std::endl;
        }
    }

    for (int j = 0; j < 1000; j++) templateVec[j] /= numTraces[j];

    hMinsJ->Draw();
    TCanvas * newC = new TCanvas();
    hMins->Draw();
    TCanvas * newC2 = new TCanvas();

    for (int j = 0; j < 1000; j++) {
        hTemplate->SetBinContent(j+1, templateVec[j]);
        // std::cout << "j: " << j << ", template: " << templateVec[j] << std::endl;
    }

    hTemplate->Draw();
    templateSpline = new TSpline3("templateSpline", -1, 1000, templateVec, 1000);
    templateSpline->Draw("LCPSAME");

    std::cout << "Done building template." << std::endl;
		
		testTemplate();
		test2Fits();
}

void testTemplate() {

    std::cout << "Testing template on training set..." << std::endl;

    TH1F * hAmpl = new TH1F("hAmpls", "hAmpls", 1000, -10, 10);
    TH1F * hStretch = new TH1F("hStretch", "hStretch", 1000, -10, 10);
    TH1F * hOffset = new TH1F("hOffset", "hOffset", 1000, -1000, 1000);
    TH1F * hBaseline = new TH1F("hBaseline", "hBaseline", 1000, -20000, 10000);

    hAmpl->SetDirectory(0);
    hStretch->SetDirectory(0);
    hOffset->SetDirectory(0);
    hBaseline->SetDirectory(0);

    std::vector<TGraph*> hFits;

    TF1 * fitfunction = new TF1("fitf", fitf, 0, 1000, 4);
    fitfunction->SetParameters(1, 0, 0, 1);

    for (int i = 0; i < numEntries; i++) {

        tTraces->GetEntry(i);
				fitfunction->SetParameters(1, 0, 0, 1);

        if (isSignal&&isBTF) {

            int min = 10000;
            int minj = -1;

            //Double_t baseline = Smooth[100];

            for (int j = 0; j < 1000; j++) {
                if (Samples[j] < min) {
                    min = Samples[j];
                    minj = j;
                }
            }

						if (min > 3500)
										continue;

            Double_t x[1000], y[1000];
            for (int j = 0; j < 1000; j++) {
                x[j] = j;

                // Double_t temp = Smooth[j+(minj-200)];
                //temp = (baseline - temp)/(baseline - minj);
                // temp = (4096 - temp)/4096;

                y[j] = Samples[j];
                // y[j] = temp;
            }
            TGraph * testGraph = new TGraph(1000, x, y);
            // testGraph->SetDirectory(0);

            testGraph->Fit("fitf", "R");

            hFits.push_back(testGraph);


            hAmpl->Fill(fitfunction->GetParameter(0));
            hOffset->Fill(fitfunction->GetParameter(1));
            hBaseline->Fill(fitfunction->GetParameter(2));
            hStretch->Fill(fitfunction->GetParameter(3));

        }
    }

    TCanvas * canvasPar = new TCanvas();
    canvasPar->Divide(2,2);
    canvasPar->cd(1);
    hAmpl->Draw();
    canvasPar->cd(2);
    hOffset->Draw();
    canvasPar->cd(3);
    hBaseline->Draw();
    canvasPar->cd(4);
    hStretch->Draw();

    //fTraces->Close();

//    TGraph * g = new TGraph(*hFits[541]);
//
//    TGraph * g2 = new TGraph(*hFits[541]);
//
//    TGraph * g3 = new TGraph(1000);
//
//    Double_t basex, basey;
//    g->GetPoint(100, basex, basey);
//    for (int i = 0; i < 1000; i++) {
//        Double_t tempx, tempy;
//        g->GetPoint(i, tempx, tempy);
//        g->SetPoint(i, tempx, basey-tempy);
//        if (i < 300)
//            g3->SetPoint(i, tempx, basey-basey);
//        if (i+300 < 1000)
//            g3->SetPoint(i+300, tempx+300, basey-tempy);
//    }
//    g2->GetPoint(100, basex, basey);
//    for (int i = 0; i < 1000; i++) {
//        Double_t tempx, tempy;
//        g2->GetPoint(i, tempx, tempy);
//        g2->SetPoint(i, tempx, basey-tempy);
//    }
//    for (int i = 0; i < 1000; i++) {
//        Double_t tempx, tempy, tempx2, tempy2;
//        if (i+300 < 999) {
//            g->GetPoint(i+400, tempx, tempy);
//        }
//        g2->GetPoint(i, tempx2, tempy2);
//        if (i+300 < 999) {
//            g->SetPoint(i+400, tempx, tempy+tempy2);
//        }
//    }
//    for (int i = 0; i < 1000; i++) {
//        Double_t tempx, tempy;
//        g->GetPoint(i, tempx, tempy);
//        g->SetPoint(i, tempx, 4000-tempy);
//        g3->GetPoint(i, tempx, tempy);
//        g3->SetPoint(i, tempx, 4000-tempy);
//    }
//    TCanvas * c2c = new TCanvas();
//    g->Draw();
//    // TF1 * testf2 = new TF1("fitf2", fitf2, 0, 1000, 5);
//    TF1 * testf2 = new TF1("fitf2", fitf2, 0, 1000, 5);
//    //testf2->SetParameters(1.4, 24, 1.0, 1.45162,307.07,1.037,-1866.81);
//    // testf2->SetParameters(1.45162,307.07,-1866.81, 1.03783);
//    // testf2->SetParLimits(0, 0, 3);
//    // testf2->SetParLimits(1, 0, 1000);
//    // testf2->SetParLimits(3, 0, 2);
//    g->Fit("fitf2","R");


    TFile * outFile = TFile::Open("fits.root", "RECREATE");
		gFile = outFile;
    for (auto h : hFits)
        h->Write();
    hAmpl->Write();
    hOffset->Write();
    hBaseline->Write();
    hStretch->Write();
    hTemplate->Write();
    outFile->Close();

    return;
}

void test2Fits() {

    TF1 * fitfunction = new TF1("fitf2", fitf2, 0, 1000, 7);
    fitfunction->SetParameters(1, 0, 1, 1, 0, 1, 0);

    std::vector<TGraph*> hFits;

    for (int i = 0; i < numEntries; i++) {

        tTraces->GetEntry(i);

				if (Npeaks > 1)
								fitfunction->SetParameters(1, 400-PeakPos->at(0), 1, 1, 400-PeakPos->at(1), 1, 0);
				else
								fitfunction->SetParameters(1, 0, 1, 1, 0, 1, 0);

        if (isSignal&&isBTF) {

            int min = 10000;
            int minj = -1;

            //Double_t baseline = Smooth[100];

            for (int j = 0; j < 1000; j++) {
                if (Samples[j] < min) {
                    min = Samples[j];
                    minj = j;
                }
            }

						if (min > 3500)
										continue;

            Double_t x[1000], y[1000];
            for (int j = 0; j < 1000; j++) {
                x[j] = j;

                // Double_t temp = Smooth[j+(minj-200)];
                //temp = (baseline - temp)/(baseline - minj);
                // temp = (4096 - temp)/4096;

                y[j] = Samples[j];
                // y[j] = temp;
            }
            TGraph * testGraph = new TGraph(1000, x, y);
            // testGraph->SetDirectory(0);

            testGraph->Fit("fitf2", "R");

            hFits.push_back(testGraph);


        }
    }

    TFile * outFile = TFile::Open("fits2.root", "RECREATE");
		gFile = outFile;
    for (auto h : hFits)
        h->Write();
    outFile->Close();

		end();
    return;
}

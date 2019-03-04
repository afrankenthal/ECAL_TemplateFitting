# ECAL_TemplateFitting

To use it with ROOT (first remember to have ROOT set up, it might already be in your .bashrc but if not):

Source ROOT env:

<code>source ROOTLOCATION/bin/thisroot.sh</code>

Run ROOT with macro file makeTemplate.C:

<code>root -l -x makeTemplate.C </code>

Fits will be plotted in file fits.root. To open with ROOT from the command line:

<code>root -l fits.root </code>

or, if already inside a ROOT session (this is C++ syntax btw):

<code>TFile _fitsfile = TFile::Open("fits.root") </code>

Then open a TBrowser which will let you look at the plots:

<code>TBrowser b</code>


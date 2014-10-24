// edited to include monitor - 29/09/09
// #########################################################################################3
// Compillation in /home/tibor/12C/DcptoROOT folder
//   (a) edit libDAQ.cxx if needed
//   (b) run root
//   (c) compilation in ROOT: .L libDAQ/libDAQ.cxx++
//   (d) .x dcp2root.cxx
//   (e) provide filename when prompted

#include <iostream.h>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

void dcp2root(){
  //  PrepareConfig();
  //  PrepareTree();

  // Set name for used input and output files
  
  cout <<"========================================"<<endl;
  cout <<"DCP to ROOT file conversion             "<<endl;
  cout <<"========================================"<<endl;
  cout <<"STAGE 1: Enter DCP file for conversion: ";
  string infileName="output.root";
  getline( cin, infileName);

  string outfileName="event.root";
  outfileName.assign(infileName);
  outfileName.append(".root");

  TFile *file = new TFile(outfileName.c_str(),"recreate");

  LoadDCPFile(infileName.c_str());
  file->Write();
} 

// Creates the TDAQConfig object which specifies how the raw DAQ
// channels should be loaded into the tree.
TDAQConfig *PrepareConfig(void){
  // Create the object.
  TDAQConfig *RetVal = new TDAQConfig();

  // Creates a new branch. In ROOT, the events are stored in trees, which have
  // branches and leaves. Leaves contain the actual event data, and branches
  // group the leaves together. This allows for faster processing of events, as
  // branches can be disabled (resulting in less data being loaded of the disk
  // and processed for each event).
  TDAQBranch *CurBranch = new TDAQBranch("Miel", "Miel");

  // This adds a new ADC to the branch. The parameters for the TDAQADC
  // constructor are (<interface>, <ADC number>, <name>, <description>, <number
  // of channels>). In detail:
  // [] Interface: The interface group from the experiment definition.  Note
  //    that it's zero based, so "Coincidence Interface 1" is put in as
  //    interface 0.
  // [] ADC number: Like the interface, it's from the experiment definition and
  //    zero based. So ADC 14.1 is interface 13 ADC 0.
  // [] Name: The name of the leaf to store the ADC data in. Does not have to
  //    match experiment definition or anything.
  // [] Description: A description of the leaf. Useful if at some point you
  //    forget what the leaf is.
  // [] Number of channels: Name says it all really.
  //CurBranch->GetADCs()->Add(new TDAQADC(0, 0, "DE", "DeltaE", 2048));
  //CurBranch->GetADCs()->Add(new TDAQADC(0, 1, "E", "Energy", 2048));
  
  // Pair spectrometer runs: Jul-2013
   CurBranch->GetADCs()->Add(new TDAQADC(0, 0, "Miel1E",    "Miel1E", 8192));
   CurBranch->GetADCs()->Add(new TDAQADC(0, 1, "Miel2E",    "Miel2E", 8192));
   CurBranch->GetADCs()->Add(new TDAQADC(0, 2, "Miel3E",    "Miel3E", 8192));
   CurBranch->GetADCs()->Add(new TDAQADC(0, 3, "Miel4E",    "Miel4E", 8192));
   CurBranch->GetADCs()->Add(new TDAQADC(0, 4, "Miel5E",    "Miel5E", 8192));
   CurBranch->GetADCs()->Add(new TDAQADC(0, 5, "Miel6E",    "Miel6E", 8192));
   CurBranch->GetADCs()->Add(new TDAQADC(1, 0, "Aptherix",  "Aptherix", 8192));
   CurBranch->GetADCs()->Add(new TDAQADC(1, 1, "HallProbe", "HallProbe", 2048));
   CurBranch->GetADCs()->Add(new TDAQADC(1, 2, "VcontE",    "VcontE", 2048));
   CurBranch->GetADCs()->Add(new TDAQADC(1, 3, "VcontG",    "VcontG", 2048));
   CurBranch->GetADCs()->Add(new TDAQADC(1, 4, "Chopper",    "Chopper", 8192));

  CurBranch->GetADCs()->Add(new TDAQADC(11, 0, "MielT1T2", "MielT1-T2", 8192));
  CurBranch->GetADCs()->Add(new TDAQADC(11, 1, "MielT1T3", "MielT1-T3", 8192));
  CurBranch->GetADCs()->Add(new TDAQADC(11, 2, "MielT1T4", "MielT1-T4", 8192));
  CurBranch->GetADCs()->Add(new TDAQADC(11, 3, "MielT1T5", "MielT1-T5", 8192));
  CurBranch->GetADCs()->Add(new TDAQADC(11, 4, "MielT1T6", "MielT1-T6", 8192));
  CurBranch->GetADCs()->Add(new TDAQADC(11, 5, "MielT2T3", "MielT2-T3", 8192));
  CurBranch->GetADCs()->Add(new TDAQADC(11, 6, "MielT2T4", "MielT2-T4", 8192));
  CurBranch->GetADCs()->Add(new TDAQADC(11, 7, "MielT2T5", "MielT2-T5", 8192));
  CurBranch->GetADCs()->Add(new TDAQADC(11, 8, "MielT2T6", "MielT2-T6", 8192));
  CurBranch->GetADCs()->Add(new TDAQADC(11, 9, "MielT3T4", "MielT3-T4", 8192));
  CurBranch->GetADCs()->Add(new TDAQADC(11, 10, "MielT3T5", "MielT3-T5", 8192));
  CurBranch->GetADCs()->Add(new TDAQADC(11, 11, "MielT3T6", "MielT3-T6", 8192));
  CurBranch->GetADCs()->Add(new TDAQADC(11, 12, "MielT4T5", "MielT4-T5", 8192));
  CurBranch->GetADCs()->Add(new TDAQADC(11, 13, "MielT4T6", "MielT4-T6", 8192));
  CurBranch->GetADCs()->Add(new TDAQADC(11, 14, "MielT5T6", "MielT5-T6", 8192));
  /*
   CurBranch->GetADCs()->Add(new TDAQADC(5, 0, "Miel1T", "Miel1T", 4096));
   CurBranch->GetADCs()->Add(new TDAQADC(5, 1, "Miel2T", "Miel2T", 4096));
   CurBranch->GetADCs()->Add(new TDAQADC(5, 2, "Miel3T", "Miel3T", 4096));
   CurBranch->GetADCs()->Add(new TDAQADC(5, 3, "Miel4T", "Miel4T", 4096));
   CurBranch->GetADCs()->Add(new TDAQADC(5, 4, "Miel5T", "Miel5T", 4096));
   CurBranch->GetADCs()->Add(new TDAQADC(5, 5, "Miel6T", "Miel6T", 4096));
  */
  // This now adds the branch to the DAQConfig object.
  RetVal->GetBranches()->Add(CurBranch);

  // Another branch. Note that here, we only have a single leaf. As a result,
  // the leaf is ignored and ROOT treats the branch "ToF" as a leaf.
  //CurBranch = new TDAQBranch("Expt", "Expt");
   //  CurBranch->GetADCs()->Add(new TDAQADC(0, 0, "MWPCToF", "Multiwire time-of-flight", 1024));
   //CurBranch->GetADCs()->Add(new TDAQADC(0, 8,  "FissionPulser", "FissionPulser", 1024));
   //CurBranch->GetADCs()->Add(new TDAQADC(0, 9,  "Monitor1", "Monitor1", 1024));
   //CurBranch->GetADCs()->Add(new TDAQADC(0, 10, "Monitor2", "Monitor2", 1024));
   //RetVal->GetBranches()->Add(CurBranch);


   
   //   CurBranch = new TDAQBranch("Alpha", "Alpha");
   //   CurBranch->GetADCs()->Add(new TDAQADC(0, 11, "EAnnular", "Energy Annular", 1024));
   //   CurBranch->GetADCs()->Add(new TDAQADC(0, 12, "TAnnular", "Time Annular", 1024));
   //   CurBranch->GetADCs()->Add(new TDAQADC(0, 13, "ESBFront",  "Energy SurfaceBarrier front", 1024));
   //   RetVal->GetBranches()->Add(CurBranch);

//   CurBranch = new TDAQBranch("Annular", "Annular");
//   CurBranch->GetADCs()->Add(new TDAQADC(0, 12, "XPos", "X positon", 1024));
//   CurBranch->GetADCs()->Add(new TDAQADC(0, 13, "YPos", "Y position", 1024));
//   RetVal->GetBranches()->Add(CurBranch);

//   CurBranch = new TDAQBranch("Front", "Front");
//   CurBranch->GetADCs()->Add(new TDAQADC(0, 14, "Mon1", "Monitor 1", 1024));
//   CurBranch->GetADCs()->Add(new TDAQADC(0, 15, "Mon2", "Monitor 2", 1024));
//   RetVal->GetBranches()->Add(CurBranch);

  // Everything is added to the config, so we're all done.
  return RetVal;
}

// Creates a new tree, based on the configuration above.
TTree *PrepareTree(void)
{
  // Create the configuration using the function above.
  TDAQConfig *DAQConfig = PrepareConfig();
  // Create the new tree. You can set the name and description to something more
  // appropriate for your experiment.
  TTree *MielTree = new TTree("MielTree", "Miel event tree");
  // This calls into libDAQ to have the TDAQConfig object set up the tree. This
  // creates all the branches, etc.
  DAQConfig->InitTree(MielTree);
  // We no longer need the DAQConfig object, so delete it so as to not leak
  // memory.
  delete DAQConfig;
  // And we're all done.
  return MielTree;
}

// Creates a new TTree object and loads a DCP file into the tree.
TTree *LoadDCPFile(char *Filename)
{
  // Like the function above, we create the configuration and set up the tree.
  TDAQConfig *DAQConfig = PrepareConfig();
  TTree *MielTree = new TTree("MielTree", "Miel event tree");
  DAQConfig->InitTree(MielTree);
  // Now, we need to create another object that does the actual loading. This
  // object is passed the DAQConfig object so it knows how to treat the input
  // DCP file and how to store it in the tree.
  TDCPReader Reader(DAQConfig);
  // This does the actual DCP->TTree load.
  Reader.FillTreeFromDCPFile(MielTree, Filename);
  // Like above, tidy up and return.
  delete DAQConfig;
  return MielTree;
}

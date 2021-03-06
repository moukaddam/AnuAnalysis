// edited to include monitor - 29/09/09
// edited the argument passing to the function LoadDCPFile(), to handle a run-time error. [Mhd: moukaddam@triumf.ca : 25/10/14]
// edited add a DEBUG variable. [Mhd: moukaddam@triumf.ca : 25/10/14]
// edited add an ADC configuration file. [Mhd: moukaddam@triumf.ca : 27/10/14]
// edited by adding the argument of configuration file name to the functions . [Mhd: moukaddam@triumf.ca : 02/11/14]
// #########################################################################################3
//   (a) edit libDAQ.cxx if needed
//   (b) run root
//   (c) compilation in ROOT: .L libDAQ/libDAQ.cxx++
//   (d) .x dcp2root.cxx
//   (e) provide filename when prompted

#include <iostream> // MHD : 25 October 2014, remove ".h" 
#include <fstream> // MHD : 27 October 2014, add file stream library
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

using namespace std ; // MHD : 27 October 2014, add name space

#define DEBUG 0

void dcp2root(){

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

 string DaqConfigName="";
  cout <<"STAGE 2: Enter DAQ configuration (return for default <./daq.config> ): \n";
  getline( cin, DaqConfigName);
  if (DaqConfigName=="") DaqConfigName="daq.config"; //default 
  cout << DaqConfigName << endl ; 
  
  //  PrepareConfig(DaqConfigName);
  //  PrepareTree(DaqConfigName);
  
  TFile *file = new TFile(outfileName.c_str(),"recreate");
  LoadDCPFile(infileName, DaqConfigName );
  file->Write();

if (DEBUG) cout << " End of dcp2root " << endl;
} 

// Creates the TDAQConfig object which specifies how the raw DAQ
// channels should be loaded into the tree.
TDAQConfig *PrepareConfig(string DaqConfig){

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
  // MHD : 27 October 2014, adding ADC configuration file
  cout << " Daq configuration is read from : " << DaqConfig << endl;
  ifstream daqconfig ; 
  daqconfig.open(DaqConfig.c_str());
  int Interface = 0 ; 
  int AdcNumber = 0 ;
  string LeafName = "LeafName";
  string LeafDescription = "LeafDescription";
  int NumberOfChannel = 8192;
  
  cout << "  |  "<<"Interface"  << "  |  "<< "AdcNumber"  << "  |  "<< "LeafName"  << "  |  "<< "LeafDescription"  << "  |  "<< "NumberOfChannel"<<"\n" ;    
  while (daqconfig >> Interface) {
  	daqconfig >> AdcNumber >> LeafName >> LeafDescription >> NumberOfChannel ;
  	cout <<" " << "  |  " << Interface  << "  |  " << AdcNumber   << "  |  "<< LeafName  << "  |  "<< LeafDescription  << "  |  "<< NumberOfChannel<<"\n" ;   
    CurBranch->GetADCs()->Add(new TDAQADC( Interface , AdcNumber , LeafName.c_str() , LeafDescription.c_str() , NumberOfChannel ) ); 
    }

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

if (DEBUG) cout << " End of PrepareConfig " << endl;
  // Everything is added to the config, so we're all done.
  return RetVal;
}

// Creates a new tree, based on the configuration above.
TTree *PrepareTree(string DaqConfigname)
{
  // Create the configuration using the function above.
  TDAQConfig *DAQConfig = PrepareConfig(DaqConfigname);
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
  if (DEBUG) cout << " End of Preparetree " << endl;
  return MielTree;
}

// Creates a new TTree object and loads a DCP file into the tree.
void LoadDCPFile(string Filename, string DaqConfigname)
{
  // Like the function above, we create the configuration and set up the tree.
  TDAQConfig *DAQConfig = PrepareConfig(DaqConfigname);

  TTree *MielTree = new TTree("MielTree", "Miel event tree");
  DAQConfig->InitTree(MielTree);
  // Now, we need to create another object that does the actual loading. This
  // object is passed the DAQConfig object so it knows how to treat the input
  // DCP file and how to store it in the tree.
  TDCPReader Reader(DAQConfig);
  // This does the actual DCP->TTree load.
  Reader.FillTreeFromDCPFile(MielTree, Filename.c_str());
  // Like above, tidy up and return.
  delete DAQConfig;
  if (DEBUG) cout << " End of LoadDCPFile " << endl;
}

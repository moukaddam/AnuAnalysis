// #########################################################################################
// compile :
//  g++ struct2class.cxx libAnalysis/libMielData.so libAnalysis/libMielHit.so libAnalysis/libMielEvent.so\
                -IlibAnalysis --std=c++0x -o newstruct2class\
                -O2 `root-config --cflags --libs`\
                -lTreePlayer -lgsl -lgslcblas 
// #########################################################################################

//c++
#include <stdio.h> //c
#include <stdlib.h> //c
#include <string.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cmath>
using namespace std;

//Root 
#include <TFile.h>
#include <TTree.h>
#include <TBranch.h>

//User 
#include "./libAnalysis/TMielData.h"
#include "./libAnalysis/TMiel.h"


// #########################################################################################
//                               global variables 
// #########################################################################################

//Declare global variables

//structure to read the basic tree
// *************************************************************************************
// I M P O R T A N T  : The type (uShort_t) and the order of the declaration is relevant
// Use : MielTree->Print() to print the structure
//
//*Br    0 :Miel      : Miel1E/s:Miel2E:Miel3E:Miel4E:Miel5E:Miel6E:Aptherix:  *
//*         | HallProbe:VcontE:VcontG:Miel1T:Miel2T:Miel3T:Miel4T:Miel5T:Miel6T*
//
// **************************************************************
struct Miel_st {
 UShort_t Miel1E;  UShort_t Miel2E; UShort_t Miel3E; UShort_t Miel4E; UShort_t Miel5E; UShort_t Miel6E;  
 UShort_t Aptherix; UShort_t HallProbe; UShort_t VcontE; UShort_t VcontG;
 UShort_t Miel1T;  UShort_t Miel2T; UShort_t Miel3T; UShort_t Miel4T; UShort_t Miel5T; UShort_t Miel6T; 
 } gMiel_st; // old tree , struct based

TMielData* gMielData; // new tree, data vector based branch, data organised, energy calibrated
TMielEvent* gMielEvent; 		  // new tree, analysis branch, hitpattern, add-back
TTree *gNewTree ;
TTree *gOldTree ;
TFile *gInputFile; 
TFile *gOutputFile;

enum FileOption {NONE, ROOTFILE, CALFILE} ; 
 vector <char*> gRootFilesList;
 vector <char*> gCalibrationFilesList; 
 
 
//Buffer variables
vector <UShort_t>	gBuffer_energy; 
vector <UShort_t>	gBuffer_time; 
UShort_t gBuffer_gamma ; 
UShort_t gBuffer_hall ; 
UShort_t gBuffer_VContE ;  
UShort_t gBuffer_VContG ; 	

vector < vector<double> > gCalibration;
bool gCalibrationRead;

//Declare functions
void GetEvent(int i); 
void ResetBuffers(); 
void ReadMielCalibration(string filename);
float CalibrateMielEnergy(int segment, int E_charge); 
float CalibrateTime(int segment, int T_charge); 
void PrintBuffers();
void PrintMiel();
void ParseInputLine(int argc, char **argv);

// #########################################################################################
//                                Main function
// #########################################################################################

int main(int argc, char **argv) {
   
  if(argc < 2)	{
    printf(" use : ./struct2class -c <calibration-file> -f <data-root-file> (XXX.root YYY.root .. etc...) \n");
    return 1;
  }

  // parse input line  
  ParseInputLine(argc,argv) ; 
  
   // Analyse root files  
  bool GoodEvent=false; 
  gCalibrationRead=false;
  gMielData 	= new TMielData(); 	// organise data
  gMielEvent 	= new TMielEvent(); // analyse data
  
	for( unsigned z = 0 ; z<gRootFilesList.size(); z++)	{
					  
		printf(" Reading file : %s\t--\t", gRootFilesList.at(z));
		string inputname = gRootFilesList.at(z);
		string outputname = inputname;	
		size_t pos = inputname.find(".root");
		outputname.insert(pos, "_data");	
		printf("Output file : %s\n", outputname.c_str());

		// point to the input and output files
		gInputFile = new TFile(inputname.c_str());
		gOutputFile = new TFile(outputname.c_str(),"RECREATE");

		//get the trees inside
		gInputFile->ls();

		gOldTree = (TTree*)gInputFile->Get("MielTree");
		//gOldTree->SetMakeClass(1); 
		gOldTree->SetBranchAddress("Miel",&gMiel_st);

		//point to the new tree and set the addresses
		gNewTree = new TTree("MielDataTree","MielDataTree");
		gNewTree->Branch("TMielData",&gMielData,16000,99);
		gNewTree->Branch("TMielEvent",&gMielEvent,16000,99);

		//Iterate through events
		Int_t nentries = (Int_t)gOldTree->GetEntries();
		cout << "Tree contains " << nentries <<endl;


		for (int j=0 ; j < 1000; j++) {
			//Get the entry and set the events in the new Tree
			GoodEvent=false; 
			GetEvent(j);	

			for (int k = 0 ; k < 6 ; k++ ) {
		  
				if (gBuffer_energy.at(k) > 0) { // keep this good event
					GoodEvent=true; 
					gMielData->SetMiel(k, gBuffer_energy.at(k), CalibrateMielEnergy(k,gBuffer_energy.at(k)), gBuffer_time.at(k)) ;
				}
			}

		//gammas
		gMielData->SetAptherix(gBuffer_gamma);
		//Control Measurements
		gMielData->SetHallProbe(gBuffer_hall);
		gMielData->SetVcontE(gBuffer_VContE);	
		gMielData->SetVContG(gBuffer_VContG);

		if (GoodEvent){
			cout << " G O O D   E V E N T " << endl;
			gMielData->Print();
						
			gMielEvent->SetMielData(gMielData); // Calculate positions, patterns, etc..
			gMielEvent->BuildAddBack(); //Calculate AddBack

			gMielEvent->Print();
			getchar() ;
		}
		
		gNewTree->Fill();	// fill the tree	
		gMielData->Clear();
		gMielEvent->Clear();
	}

// Write the new trees in seperate files 
gNewTree->AutoSave();  

gOutputFile->Write();
gOutputFile->Close();  

} //end of input files 

	
return 0 ; 	

}//end 

// #########################################################################################
//                                functions start here 
// #########################################################################################

void PrintMiel(){

	cout << "E1" << " \t" << "E2" << " \t" << "E3" << " \t" << "E4" << " \t" << "E5" << " \t" << "E6" << " \n";
	cout << gMiel_st.Miel1E << " \t";
	cout << gMiel_st.Miel2E << " \t";
	cout << gMiel_st.Miel3E << " \t";
	cout << gMiel_st.Miel4E << " \t";
	cout << gMiel_st.Miel5E << " \t";
	cout << gMiel_st.Miel6E << " \n";
	
	cout << "T1" << " \t" << "T2" << " \t" << "T3" << " \t" << "T4" << " \t" << "T5" << " \t" << "T6" << " \n" ;
	cout << gMiel_st.Miel1T << " \t";
	cout << gMiel_st.Miel2T << " \t";
	cout << gMiel_st.Miel3T << " \t";
	cout << gMiel_st.Miel4T << " \t";
	cout << gMiel_st.Miel5T << " \t";
	cout << gMiel_st.Miel6T << " \n";
	
	cout << "gamma" << " \t" << "hall" << " \t" << "VcontE" << " \t" << "VcontG" << " \n" ;
	cout << gMiel_st.Aptherix << " \t";
	cout << gMiel_st.HallProbe << " \t";
	cout << gMiel_st.VcontE << " \t";
	cout << gMiel_st.VcontG << " \n";

	}
	
	
void PrintBuffers(){

	cout << "======= Print buffers ====  \n" ;
	cout << "==========================\n" ; 

	cout << "Time size: " << gBuffer_time.size() <<"\n" ;
			
	for (unsigned i = 0 ; i < gBuffer_time.size() ; i++ ) {
	cout << " "<< gBuffer_time.at(i)<<"\t";
	}
	cout << "\n-------------------------\n" ; 
	
	cout << "Energy size: " << gBuffer_energy.size() <<"\n" ;
	for (unsigned i = 0 ; i < gBuffer_energy.size() ; i++ ) {
	cout << " "<< gBuffer_energy.at(i)<<"\t";
	}
	cout << "\n-------------------------\n" ; 

	cout << "gBuffer_gamma " << gBuffer_gamma <<" \n";
		cout << "gBuffer_hall " << gBuffer_hall <<" \n";
			cout << "gBuffer_VContE " << gBuffer_VContE <<" \n";
				cout << "gBuffer_VContG " << gBuffer_VContG <<" \n";
			
	cout << "==========================\n" ; 
}

// ##############################
void ResetBuffers(){

	gBuffer_energy.clear(); 
	gBuffer_time.clear();
	gBuffer_gamma =   -1;
	gBuffer_hall =   -1;
	gBuffer_VContE =  -1; 
	gBuffer_VContG =  -1;
	
	gBuffer_energy.resize(6,-1); 
	gBuffer_time.resize(6,-1);
	
} 

// ##############################
void GetEvent(int i){

	ResetBuffers();
	gOldTree->GetEntry(i);
	
    //Fill the buffers 
		//Miel Energies
	if(gBuffer_energy.size()==6) {
		gBuffer_energy.at(0) =   gMiel_st.Miel1E; 
		gBuffer_energy.at(1) =   gMiel_st.Miel2E; 
		gBuffer_energy.at(2) =   gMiel_st.Miel3E; 
		gBuffer_energy.at(3) =   gMiel_st.Miel4E; 
		gBuffer_energy.at(4) =   gMiel_st.Miel5E; 
		gBuffer_energy.at(5) =   gMiel_st.Miel6E; 
	}
	else{cout << "wrong size" ; exit(-1);}
	
		//Miel times
	if(gBuffer_energy.size()==6) { 
	gBuffer_time.at(0) =   gMiel_st.Miel1T; 
	gBuffer_time.at(1) =   gMiel_st.Miel2T; 
	gBuffer_time.at(2) =   gMiel_st.Miel3T; 
	gBuffer_time.at(3) =   gMiel_st.Miel4T; 
	gBuffer_time.at(4) =   gMiel_st.Miel5T; 
	gBuffer_time.at(5) =   gMiel_st.Miel6T;
	}
	else{cout << "wrong size" ; exit(-1);}
	
		// gamma energies
	gBuffer_gamma =   gMiel_st.Aptherix;
		//Hall probe 
	gBuffer_hall  =   gMiel_st.HallProbe;
		//Normalizing parameters
	gBuffer_VContE =  gMiel_st.VcontE; 
	gBuffer_VContG =   gMiel_st.VcontG;	

} 

void ReadMielCalibration(string fFilename) {

  int sSeg;
  double sLin, sGain, sQuad;  

  ifstream sCalFile(fFilename);
  if (sCalFile.is_open()) {
    while ( true )  {
      sCalFile >> sSeg >> sLin >> sGain >> sQuad;
      if ( sCalFile.eof()) break;
      vector<double> sVector;
      sVector.push_back(sLin);
      sVector.push_back(sGain);
      sVector.push_back(sQuad);
      gCalibration.push_back(sVector);
    }
    sCalFile.close(); 
  } else cout << "Unable to open file" << endl;

  int size = gCalibration.size();
  printf("\tRead in %d miel channels\n", size);

}

// ##############################
float CalibrateMielEnergy(int segment, int E_charge) {
  if (gCalibrationRead) {
    float temp = 0;
    for (int i=0; i<gCalibration.at(segment).size(); i++)
      temp += gCalibration.at(segment).at(i) * pow(static_cast<double>(E_charge), i);
    return temp ; 
  } else
    return E_charge;  
}

// ##############################
float CalibrateTime(int segment, int T_charge){
//need to implement 
return T_charge ; 
}


// ##############################
void ParseInputLine(int argc, char **argv) {

    FileOption option = NONE ; 
    
    for(int i=1;i<argc;i++)	{
    
    //printf(" Reading argv[%d] : %s \n", i, argv[i]);
    if (strcmp(argv[i], "-f") == 0) { //option = 1 
		if  (i+1 == argc) {
			printf(" You must provide a file after option  : %s \n", argv[i]);
			printf(" Program aborted\n");
			exit(-1);		
		}
		option = ROOTFILE ;
		continue ;
    }
    
    if (strcmp(argv[i], "-c") == 0) { //option = 2
		if  (i+1 == argc) {
			printf(" You must provide a file after option  : %s \n", argv[i]);
			printf(" Program aborted\n");
			exit(-1);		
		}
    option = CALFILE ;
    continue;
    }
    
   if (option==ROOTFILE) gRootFilesList.push_back(argv[i]);
   else   if (option==CALFILE) gCalibrationFilesList.push_back(argv[i]) ;
          else if (option==NONE) {
			printf(" Provide an option before file \n");
			printf(" to analyse <file>              : -f <file>  \n");
			printf(" to load <file> for calibration : -c <file> :  \n");
			exit(-1);		
			}
    	}
    
    if (gRootFilesList.size()==0) {
            printf(" No root files are provided.. \n");
			printf(" to analyse <file>              : -f <file>  \n");
    		}
    
    if (gCalibrationFilesList.size()==0) {
            printf(" No calibration files are provided.. \n");
			printf(" to load <file> for calibration : -c <file> :  \n");
    		}
       
// Read calibration 
    for(unsigned i = 0;i<gCalibrationFilesList.size();i++)	{
    		printf(" Reading in calibration file %s\n", gCalibrationFilesList.at(i) );
		    ReadMielCalibration(gCalibrationFilesList.at(i));
		    gCalibrationRead=true;
    		}

}



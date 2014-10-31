// #########################################################################################
// compile only MielData: \
 g++ struct2class.cxx libAnalysis/libMielData.so  -IlibAnalysis --std=c++0x -o AnalyseAnu.exe -O2 `root-config --cflags --libs`   -lTreePlayer -lgsl -lgslcblas
// compile all : \
 g++ struct2class.cxx libAnalysis/libMielData.so libAnalysis/libMielHit.so  libAnalysis/libMielEvent.so -IlibAnalysis --std=c++0x -o AnalyseAnu.exe -O2 `root-config --cflags --libs`   -lTreePlayer -lgsl -lgslcblas 
// #########################################################################################

//c++
#include <stdio.h> //c
#include <stdlib.h> //c
#include <string.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <cmath>
using namespace std;

//Root 
#include <TObjArray.h>
#include <TFile.h>
#include <TTree.h>
#include <TBranch.h>
#include <TLeaf.h>
#include <TH1F.h>
#include <TF1.h>
#include <TString.h>
#include <TStopwatch.h>

//User 
#include "./libAnalysis/TMielData.h"
#include "./libAnalysis/TMielEvent.h"


//Declare global variables

//structure to read the basic tree
// *****************************************************************************
// I M P O R T A N T  : 
// The type (uShort_t) and the order of the declaration is relevant
// Use : MielTree->Print() to print the structure
// *****************************************************************************
const int param_number  = 26 ;          // experiment parameters number 
struct Miel_st {
 UShort_t TableAt[param_number]; 
 } gMiel_st; // old tree , struct based

enum IDENTITY {
 Miel1E,   Miel2E,  Miel3E,  Miel4E,  Miel5E,  Miel6E,  // {0,1...5}
 Aptherix,  HallProbe,  VcontE,  VcontG,  Chopper,		// {0,1...5}
 MielT1T2,  MielT1T3,  MielT1T4,  MielT1T5,  MielT1T6, 
            MielT2T3,  MielT2T4,  MielT2T5,  MielT2T6,
                       MielT3T4,  MielT3T5,  MielT3T6,
                                  MielT4T5,  MielT4T6,
                                             MielT5T6};

enum FileOption {NONE, ROOTFILE, ECALFILE, TCALFILE, PHYSICS, HIST} ; 
 vector <char*> gRootFilesList;
 vector <char*> gECalibrationFilesList; 
 vector <char*> gTCalibrationFilesList; 
 vector <char*> gPhysicsOptionList;
 vector <char*> gHistFileName; // only the last filename will be considered

//Other variables
int    gCycle=0;
vector < vector<double> > gECalibrationCoeff;
std::map<int, vector <double> > gTCalibrationCoeff;

//boolean for mode selection
bool gkData = false ; 
bool gkPairMode = false ; 
bool gkClusterMode = false  ;
bool gkPairSumMode = false  ; // not implemented in TMielEvent yet 

vector < vector<int> > gTimeCombinations;
bool 	gECalibrationRead=false;
bool 	gTCalibrationRead=false;;

//Declare functions
int GetTimeDifference(int SEG, int seg);
void ReadEnergyCalibration(string filename);
void ReadTimeCalibration(string filename);
float CalibrateMielEnergy(int segment, int E_charge); 
float CalibrateMielTime(int SEGMENT, int segment, int time); 
double CalculateBRho(float energy);
void PrintBasicMiel();
void ParseInputLine(int argc, char **argv);
char NextCycle() ; 
void InputMessage(); 
void ParsePhysicsOption(); 

// #########################################################################################
//                                Main function
// #########################################################################################

int main(int argc, char **argv) {

	// parse input line  
	ParseInputLine(argc,argv) ; 
	
	//Data Files
	TFile *gInputFile; 
	TFile *gOutputFile;
	//Trees
	TTree *gNewTree ;
	TTree *gOldTree ;

	//Data structures
	TMielData* gMielData; // new tree, data vector based branch, data organised, energy calibrated
	TMielEvent* gMielEvent; 		  // new tree, analysis branch, hitpattern, add-back
	
	// Analyse root files  
	bool GoodEvent=false; 
	//gEnergyCalibrationRead=false;

	gMielData 	= new TMielData(); 	// organise data
	gMielEvent 	= new TMielEvent();     // analyse data
    
    // Stopwatch treating time
    TStopwatch stopwatch;
  		   
	for( unsigned z = 0 ; z<gRootFilesList.size(); z++)	{
		 
		printf(" Reading file : %s\t--\t", gRootFilesList.at(z));
		string inputname = gRootFilesList.at(z);
		string outputname = inputname;
		size_t pos = inputname.find(".root");
		outputname.insert(pos, "_data");	
		printf("Output file : %s\n", outputname.c_str());
		
		//Set Benchmark (measure the processing time)
		stopwatch.Start(); 
		
		// point to the input and output files
		gInputFile = new TFile(inputname.c_str());
		gOutputFile = new TFile(outputname.c_str(),"RECREATE");

		//get the trees inside
		gInputFile->ls();

		gOldTree = (TTree*)gInputFile->Get("MielTree");
		gOldTree->SetBranchAddress("Miel",&gMiel_st);

		//point to the new tree and set the addresses
		gNewTree = new TTree("MielDataTree","MielDataTree");
		if(gkData) gNewTree->Branch("TMielData",&gMielData);
		gNewTree->Branch("TMielEvent",&gMielEvent);

		//Iterate through events
		Int_t nentries = (Int_t)gOldTree->GetEntries();
		cout << "Tree contains " << nentries <<endl;
		//nentries = 50000000 ; // experimenting value


		// progress bar variables
		int GoodMiel = 0 ;
    	int j = 0 ; 
		char BarString[30] = "[                     ] <( )>";
		int  Loop = 0 ;
		printf("\rProcessing events from file %s ... %s (%d total events) (%d Miel events [%2.0f\%%]) ",inputname.c_str(), BarString, j , GoodMiel, GoodMiel*100.0/nentries); 
		fflush(stdout);
  
		for (j=0 ; j < nentries; j++) {
		
			// progress bar			
			if (j % 500 == 1 ) {
					printf("\rProcessing events from file %s ... %s (%d total events) (%d Miel events [%2.0f\%%]) ",inputname.c_str(), BarString, j , GoodMiel, GoodMiel*100.0/nentries);
		   			fflush(stdout);
					}
			if (j % 5000==1) BarString[26] = NextCycle();
			if (j % (nentries/20)==1) {
					BarString[Loop+1] = '=';
					printf("\rProcessing events from file %s ... %s (%d total events) (%d Miel events [%2.0f\%%])  ",inputname.c_str(), BarString, j , GoodMiel, GoodMiel*100.0/nentries);
					fflush(stdout);
					Loop++ ; 
					}
			
			//Get the entry and set the events in the new Tree
			GoodEvent=false; 
			bool TimeOriginSet = false ;
			gOldTree->GetEntry(j);

			int SEG = -1 ; // this is the origin of time
			for (int seg = 0 ; seg < 6 ; seg++ ) {
				if (gMiel_st.TableAt[seg] > 0) { // keep this good event
					GoodMiel++;					
					GoodEvent=true;
					if(!TimeOriginSet) {
						SEG = seg ; 
						TimeOriginSet = true ;
						//cout << " SEG = "<< SEG <<"\t";
						//getchar();
					 	}
					int time = GetTimeDifference(SEG,seg);
          float energy = CalibrateMielEnergy(seg,gMiel_st.TableAt[seg]);
          double brho = CalculateBRho(energy);
					gMielData->SetMiel(seg, gMiel_st.TableAt[seg], // segment and charge 
					energy, brho, // calibrated energy & brho
					CalibrateMielTime(SEG,seg,time) ) ; // calibrated time
				}
				
			}
			
			//gammas
			gMielData->SetAptherix(gMiel_st.TableAt[Aptherix]);
			//Control Measurements
      double magfield = 4.04013 * static_cast<double>(gMiel_st.TableAt[HallProbe]) + 28.571;
			gMielData->SetHallProbe(magfield);
      double contv = 4.1314 * static_cast<double>(gMiel_st.TableAt[VcontE]) - 3.8348;
			gMielData->SetVcontE(contv);	
			gMielData->SetVContG(gMiel_st.TableAt[VcontG]);
			gMielData->SetChopper(gMiel_st.TableAt[Chopper]);
		
			//FillHist(); 
			if (GoodEvent){
				//cout << " G O O D   E V E N T " << endl;	
				gMielEvent->SetMielData(gMielData); // Calculate positions, patterns, etc..
				gMielEvent->BuildAddBack(gkClusterMode,gkPairMode, gkPairSumMode); //Calculate AddBack
		
				//PrintBasicMiel();
				//gMielData->Print();				
				//gMielEvent->Print();
				//getchar() ;
				}
		
			gNewTree->Fill();	// fill the tree	
			gMielData->Clear();
			gMielEvent->Clear();
			if (j % 50000 == 0 ) gNewTree->AutoSave("FlushBaskets");  
			}
		printf("\rProcessing events from file %s ... %s (%d total events) (%d Miel events [%2.0f\%%])  Done! \n",inputname.c_str(), BarString, j , GoodMiel, GoodMiel*100.0/j);
		
		// Write the new trees in seperate files 
		gNewTree->Write();	// fill the tree	
		gOutputFile->Write();
		gOutputFile->Close(); 

		gNewTree=NULL;
		gOldTree=NULL;
		gOutputFile=NULL;
		
		stopwatch.Stop(); 
   		cout << " End of the current file : " << inputname << "\n";
   		cout << " Cpu  time " << " =\t" << (int) stopwatch.CpuTime()/60 << "' " <<  ( (int) stopwatch.CpuTime())%60<< "\"" <<endl;
		cout << " Real time " << " =\t" << (int) stopwatch.RealTime()/60 << "' "<<  ( (int) stopwatch.RealTime())%60<< "\"" <<endl;
		stopwatch.Reset();
		
		}//end of input files


	delete gMielData ; 	// free memory
	delete gMielEvent ;     // free memory
	
	}//end 



// #########################################################################################
//                                functions start here 
// #########################################################################################

void PrintBasicMiel(){

	cout << "======================================\n" ; 
	cout << "======= Print basic Miel Event  ====  \n" ;
	cout << "======================================\n" ; 
	
	cout << "Energy : \n" ;
	cout << "E1" << " \t" << "E2" << " \t" << "E3" << " \t" << "E4" << " \t" << "E5" << " \t" << "E6" << " \n";
	cout << gMiel_st.TableAt[Miel1E] << " \t";
	cout << gMiel_st.TableAt[Miel2E] << " \t";
	cout << gMiel_st.TableAt[Miel3E] << " \t";
	cout << gMiel_st.TableAt[Miel4E] << " \t";
	cout << gMiel_st.TableAt[Miel5E] << " \t";
	cout << gMiel_st.TableAt[Miel6E] << " \n";
	
	cout << "Time : \n" ;
	cout << "T1T2" << " \t" << "T1T3" << " \t" << "T1T4" << " \t" << "T1T5" << " \t" << "T1T6" << " \n" ;
	cout << gMiel_st.TableAt[MielT1T2] << " \t";
	cout << gMiel_st.TableAt[MielT1T3] << " \t";
	cout << gMiel_st.TableAt[MielT1T4] << " \t";
	cout << gMiel_st.TableAt[MielT1T5] << " \t";
	cout << gMiel_st.TableAt[MielT1T6] << " \n";

	cout << "T2T3" << " \t" << "T2T4" << " \t" << "T2T5" << " \t" << "T2T6" << " \n" ;
	cout << gMiel_st.TableAt[MielT2T3] << " \t";
	cout << gMiel_st.TableAt[MielT2T4] << " \t";
	cout << gMiel_st.TableAt[MielT2T5] << " \t";
	cout << gMiel_st.TableAt[MielT2T6] << " \n";
	
	cout << "T3T4" << " \t" << "T3T5" << " \t" << "T3T6" << " \n" ;
	cout << gMiel_st.TableAt[MielT3T4] << " \t";
	cout << gMiel_st.TableAt[MielT3T5] << " \t";
	cout << gMiel_st.TableAt[MielT3T6] << " \n";

	cout << "T4T5" << " \t" << "T4T6" << " \n" ;
	cout << gMiel_st.TableAt[MielT4T5] << " \t";
	cout << gMiel_st.TableAt[MielT4T6] << " \n";

	cout << "T5T6" << " \n" ;
	cout << gMiel_st.TableAt[MielT5T6] << " \n";
	
	cout << "Other : \n" ;	
	cout << "gamma" << " \t" << "hall" << " \t" << "VcontE" << " \t" << "VcontG" << " \n" ;
	cout << gMiel_st.TableAt[Aptherix] << " \t";
	cout << gMiel_st.TableAt[HallProbe] << " \t";
	cout << gMiel_st.TableAt[VcontE] << " \t";
	cout << gMiel_st.TableAt[VcontG]<< " \t";
	cout << gMiel_st.TableAt[Chopper] << " \n";
	}
	

// ##############################
int GetTimeDifference(int SEG, int seg){

	if (SEG == seg ) return 0 ;
	
	int sign = +1 ;  
	if (SEG > seg ) {  // flip values to correspond to the right ADC,  
		int temp = SEG ; 
		SEG = seg ; 
		seg = temp ;
		sign = -sign; //and return the negatif value
		}
	
	if (SEG == 0 ) {
		if (seg == 1)	return sign*gMiel_st.TableAt[MielT1T2];
		if (seg == 2)	return sign*gMiel_st.TableAt[MielT1T3];
		if (seg == 3)	return sign*gMiel_st.TableAt[MielT1T4];
		if (seg == 4)	return sign*gMiel_st.TableAt[MielT1T5];
		if (seg == 5)	return sign*gMiel_st.TableAt[MielT1T6];
		}
	if (SEG == 1 ) {
		if (seg == 2)	return sign*gMiel_st.TableAt[MielT2T3];
		if (seg == 3)	return sign*gMiel_st.TableAt[MielT2T4];
		if (seg == 4)	return sign*gMiel_st.TableAt[MielT2T5];
		if (seg == 5)	return sign*gMiel_st.TableAt[MielT2T6];
		}
	if (SEG == 2) {
		if (seg == 3)	return sign*gMiel_st.TableAt[MielT3T4];
		if (seg == 4)	return sign*gMiel_st.TableAt[MielT3T5];
		if (seg == 5)	return sign*gMiel_st.TableAt[MielT3T6];
		}
	if (SEG == 3) {
		if (seg == 4)	return sign*gMiel_st.TableAt[MielT4T5];
		if (seg == 5)	return sign*gMiel_st.TableAt[MielT4T6];
		}
	if (SEG == 4) {
		if (seg == 5)	return sign*gMiel_st.TableAt[MielT5T6];
		}		
}


void ReadEnergyCalibration(string fFilename) {

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
      gECalibrationCoeff.push_back(sVector);
    }
    sCalFile.close(); 
  } 
  else {
  	cout << "Unable to open file, no energy calibration will be made,   " << endl;
  	gECalibrationRead = false ; 
  }

  int size = gECalibrationCoeff.size();
  printf("\tRead in %d miel energy channels\n\n", size);

//inspect vector 
/*
  for ( int i = 0 ; i < gECalibrationCoeff.size(); i++) {
    	cout << i << " => " << gECalibrationCoeff.at(i).size() << '\n';
    	for (int j = 0 ; j < gECalibrationCoeff.at(i).size()  ; j++) {
			cout << " \t " << gECalibrationCoeff.at(i).at(j); 
			}
    cout << "\n" ; 
  }
getchar();
*/
 
}


void ReadTimeCalibration(string fFilename) {

  int sSEG, sSeg;
  double sLin, sGain, sQuad;  

  ifstream sCalFile(fFilename);
  if (sCalFile.is_open()) {
    while ( true )  {
      sCalFile >> sSEG >> sSeg >> sLin >> sGain >> sQuad;
      int key = sSEG*10 + sSeg ; 
      if ( sCalFile.eof()) break;
      vector<double> sVector;
      sVector.push_back(sLin);
      sVector.push_back(sGain);
      sVector.push_back(sQuad);
      gTCalibrationCoeff[key]= sVector ;
    }
    sCalFile.close(); 
  }   
  else {
  	cout << "Unable to open file, no time calibration will be made,   " << endl;
  	gTCalibrationRead = false ; 
  }
  
  int size = gTCalibrationCoeff.size();
  printf("\tRead in %d miel time channels\n\n", size);

//inspect map
/*  
  std::map<int, vector<double> >::iterator it;
  for ( it=gTCalibrationCoeff.begin(); it!=gTCalibrationCoeff.end(); ++it) {
    std::cout << it->first << " => " << it->second.size() << '\n';
    for (int i = 0 ; i < it->second.size()  ; i++) {
		cout << " \t " << it->second.at(i); 
		}
    cout << "\n" ; 
  }
 */
 

}


// ##############################
float CalibrateMielEnergy(int segment, int E_charge) {
    
  if (gECalibrationRead) {
		float temp = 0;
		for (int i=0; i<gECalibrationCoeff.at(segment).size(); i++)
		  temp += gECalibrationCoeff.at(segment).at(i) * pow(static_cast<double>(E_charge), i);
		return temp ; 
	  } 
  else
    return E_charge;  
}

// ##############################
float CalibrateMielTime(int SEG, int seg, int T_charge){

	int sign = +1 ; 
  	if (gTCalibrationRead) {
  		 // flip values to correspond to the right coeff of the corresponding ADC,  
		if ( SEG > seg ) { 
			int temp = SEG ; 
			SEG = seg ; 
			seg = temp ;
			sign = -sign; //and return the negatif value
			}
		// create the map key
		int key = SEG*10 + seg ; 
		// calibrate
		float temp = 0;
		for (int i = 0; i < gTCalibrationCoeff[key].size() ; i++)
			temp += gTCalibrationCoeff[key].at(i) * pow(static_cast<double>(T_charge), i);
		return sign*temp ; 
	  } 
  else
    return T_charge;
}

double CalculateBRho(float energy) {

  double constant = 17045.09;
  double electron = 510.998928;

  double brho = (constant / electron) * sqrt( pow(energy, 2.) + 2.*electron*energy );

  return brho;

}

void InputMessage(){
		printf(" \tto analyse <file>			: -f  <file>  \n");
		printf(" \tto load <file> for energy calibration	: -ce <file>   \n");
		printf(" \tto load <file> for time calibration	: -ct <file>  \n");
		printf(" \tto load analysis physics <option>	: -phys/-p <option-1> <option-2> etc...  \n");
		printf(" \tto write produce flat histogram <file>	: -hist/-h <file>   \n");
		exit(-1);		
		}
			

// ##############################
void ParseInputLine(int argc, char **argv) {

	if(argc < 2) {
	printf(" (minimal) use 		: ./AnalyseAnu.exe -f <data-root-file> (file1.root file2.root .. etc...) \n");
	printf(" Possible options 	:  \n");
	InputMessage(); 
	return ;
	}

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
    
    if (strcmp(argv[i], "-ce") == 0) { //option = 2
		if  (i+1 == argc) {
			printf(" You must provide a file after option  : %s \n", argv[i]);
			printf(" Program aborted\n");
			exit(-1);		
		}
    option = ECALFILE ;
    continue;
    }
 
     if (strcmp(argv[i], "-ct") == 0) { //option = 3
		if  (i+1 == argc) {
			printf(" You must provide a file after option  : %s \n", argv[i]);
			printf(" Program aborted\n");
			exit(-1);		
		}
    option = TCALFILE ;
    continue;
    }
    
	if (strcmp(argv[i], "-p") == 0 || strcmp(argv[i], "-phys") == 0) { //option = 4  { physics option what are the branches that is needed to figure in the tree}  
		if  (i+1 == argc) {
			printf(" You must provide an analysis mode after option  : %s \n", argv[i]);
			printf(" Program aborted\n");
			exit(-1);		
		}
    option = PHYSICS ;
    continue;
    }

	if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "-hist") == 0) { //option = 5  { generate flat histograms }  
		if  (i+1 == argc) {
			printf(" You must provide a flat histogram file name after option  : %s \n", argv[i]);
			printf(" Program aborted\n");
			exit(-1);		
		}
    option = HIST ;
    continue;
    }
       
   if (option==ROOTFILE) gRootFilesList.push_back(argv[i]);
	   else   if (option==ECALFILE) gECalibrationFilesList.push_back(argv[i]) ;
		   else   if (option==TCALFILE) gTCalibrationFilesList.push_back(argv[i]) ;
		 	   else   if (option==PHYSICS) gPhysicsOptionList.push_back(argv[i]) ;
		 	   		else   if (option==HIST) gHistFileName.push_back(argv[i])  ; // if several file names are given in a row, the last one will be the name of the flat histogram  
          
          else if (option==NONE)  {
      			printf(" Provide an option before file : \n");
      			InputMessage();
          }
          
    	}
    
   printf(" \n + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + \n");	 		   
    
    if (gRootFilesList.size()==0) {
            printf(" No root files are provided.. \n");
			printf(" \tto analyse <file>              : -f <file>  \n");
    		}
    if (gECalibrationFilesList.size()==0) {
            printf(" No energy calibration files are provided.. \n");
			printf(" \tto load <file> for calibration : -ce <file> \n");
    		}
    if (gTCalibrationFilesList.size()==0) {
            printf(" No time calibration files are provided.. \n");
			printf(" \tto load <file> for calibration : -ct <file> \n");
    		}
    if (gPhysicsOptionList.size()==0) {
            printf(" No Physics option are selected.. All options will be constructed \n");
			printf(" \tto select <option> for calibration : -p <data>/<sum>/<cluster>/<pair> \n");
    		} 
    if (gHistFileName.size()==0) {
            printf(" No Flat histogram .. \n");
			printf(" \tto produce flat histogram : -hist <output-file-name> \n");
    		}
    
    printf(" \n + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + \n");	 		   
// Read files / Options / calibrations
			for(unsigned i = 0 ; i<gRootFilesList.size();i++){ 
				if( i == 0 ) printf(" Reading in root files that will be analysed : ");
				printf(" %s\t", gRootFilesList.at(i) );  // this will be passed as booleans to the TMielEvent class
				if( i+1 == gRootFilesList.size()) printf("\n\n"); 
				}
    		if(gECalibrationFilesList.size()){
    			gECalibrationRead=true;
    			unsigned index = gECalibrationFilesList.size()-1 ; 
				printf(" Reading in energy calibration file %s\n", gECalibrationFilesList.at(index) );
				ReadEnergyCalibration(gECalibrationFilesList.at(index)); // read the last one given 
				}
    		if(gTCalibrationFilesList.size()){
				gTCalibrationRead=true;
    			unsigned index = gTCalibrationFilesList.size()-1 ; 
				printf(" Reading in time calibration file %s\n", gTCalibrationFilesList.at(index) );
				ReadTimeCalibration(gTCalibrationFilesList.at(index)); // read the last one given 
				}
			for(unsigned i = 0 ; i<gPhysicsOptionList.size() ; i++)	{
				//gPhysOptionRead=true;
				if( i == 0 ) printf(" Reading in physics options : ");
				printf(" %s\t", gPhysicsOptionList.at(i) );  // this will be passed as booleans to the TMielEvent class
				if( i+1 == gPhysicsOptionList.size()) {
					ParsePhysicsOption(); 
					printf("\n\n");} 
				}   		 
    		if(gHistFileName.size()){
				//gHistOptionRead=true;
    			unsigned index = gHistFileName.size()-1 ; 
				printf(" Reading in the flat histogram file name : "); 
				printf(" %s\n\n", gHistFileName.at(index) );  // this will be passed as booleans to the TMielEvent class
				}
}

char NextCycle(){

if (gCycle==0 )  { gCycle++ ; return '-' ; } 
if (gCycle==1 )  { gCycle++ ; return '\\' ; }
if (gCycle==2 )  { gCycle++ ; return '|' ; }
if (gCycle==3 )  { gCycle++ ; return '/' ; }
if (gCycle==4 )  { gCycle=1 ; return '-' ; }

return '#' ;
}

void ParsePhysicsOption(){

cout << "\n\tSelected analysis mode : " << endl ; 
cout << "\t------------------------" << endl ;
	for(unsigned i = 0 ; i<gPhysicsOptionList.size();i++){
	 
		if(strcmp(gPhysicsOptionList.at(i), "data") == 0) 	{ gkData = true ; cout << "\t\t Enabled "<< gPhysicsOptionList.at(i) << " mode "<< endl ; continue ;  }
		else 
	 	if(strcmp(gPhysicsOptionList.at(i), "pair") == 0) 	{ gkPairMode = true ; cout << "\t\t Enabled "<< gPhysicsOptionList.at(i) << " mode "<< endl ; continue ; } 
	 	else 
	 	if(strcmp(gPhysicsOptionList.at(i), "cluster") == 0)	{gkClusterMode = true  ; cout << "\t\t Enabled "<< gPhysicsOptionList.at(i) << " mode "<< endl ; continue ; }
	 	else
	 	if(strcmp(gPhysicsOptionList.at(i), "pairsum") == 0)  {gkPairSumMode = false ; cout << "\t\t Enabled "<< gPhysicsOptionList.at(i) << "\t Mode not implemented yet! " << endl ; continue ; } // not implemented in TMielEvent yet
		else 
		{cout << "\t\t   ???   "<< gPhysicsOptionList.at(i) <<"\t\t\t Mode does not exist "  << endl ; continue ; } 
	}
			

}



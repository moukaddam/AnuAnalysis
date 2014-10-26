// #########################################################################################
// compile only MielData: \
 g++ struct2class.cxx libAnalysis/libMielData.so  -IlibAnalysis --std=c++0x -o newstruct2class -O2 `root-config --cflags --libs`   -lTreePlayer -lgsl -lgslcblas
// compile all : \
 g++ struct2class.cxx libAnalysis/libMielData.so libAnalysis/libMielHit.so  libAnalysis/libMielEvent.so -IlibAnalysis --std=c++0x -o newstruct2class -O2 `root-config --cflags --libs`   -lTreePlayer -lgsl -lgslcblas 

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
#include <TObjArray.h>
#include <TFile.h>
#include <TTree.h>
#include <TBranch.h>
#include <TLeaf.h>
#include <TH1F.h>
#include <TF1.h>
#include <TString.h>

//User 
#include "./libAnalysis/TMielData.h"
#include "./libAnalysis/TMielEvent.h"

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
 UShort_t Aptherix; UShort_t HallProbe; UShort_t VcontE; UShort_t VcontG; UShort_t Chopper;
 UShort_t MielT1T2; UShort_t MielT1T3; UShort_t MielT1T4; UShort_t MielT1T5; UShort_t MielT1T6; 
                    UShort_t MielT2T3; UShort_t MielT2T4; UShort_t MielT2T5; UShort_t MielT2T6;
                                       UShort_t MielT3T4; UShort_t MielT3T5; UShort_t MielT3T6;
                                                          UShort_t MielT4T5; UShort_t MielT4T6;
                                                                             UShort_t MielT5T6;
 } gMiel_st; // old tree , struct based




enum FileOption {NONE, ROOTFILE, CALFILE} ; 
 vector <char*> gRootFilesList;
 vector <char*> gCalibrationFilesList; 
 
 
//Buffer variables
vector <UShort_t>	gBuffer_energy; 
UShort_t 		gBuffer_time[6][6]; 
UShort_t 		gBuffer_Gamma ; 
UShort_t 		gBuffer_Hall ; 
UShort_t 		gBuffer_VContE ;  
UShort_t 		gBuffer_VContG ; 	
UShort_t 		gBuffer_Chopper ;
int 			gCycle=0;

vector < vector<double> > gCalibration;
vector <double> gTimeCalibration;
vector < vector<int> > gTimeCombinations;
bool gEnergyCalibrationRead;

//Declare functions
void GetEvent(TTree* OldTree, int i); 
void ResetBuffers(); 
void ResetTimeBuffer();
void ReadEnergyCalibration(string filename);
void ReadTimeCalibration(string filename);
float CalibrateMielEnergy(int segment, int E_charge); 
float CalibrateMielTime(int SEGMENT, int segment, int time); 
void PrintBuffers();
void PrintBasicMiel();
void ParseInputLine(int argc, char **argv);
char NextCycle() ; 

// #########################################################################################
//                                Main function
// #########################################################################################

int main(int argc, char **argv) {


	// parse input line  
	ParseInputLine(argc,argv) ; 
	
	//Data structures
	TMielData* gMielData; // new tree, data vector based branch, data organised, energy calibrated
	TMielEvent* gMielEvent; 		  // new tree, analysis branch, hitpattern, add-back
	//Data Files
	TFile *gInputFile; 
	TFile *gOutputFile;
	//Trees
	TTree *gNewTree ;
	TTree *gOldTree ;

	// Analyse root files  
	bool GoodEvent=false; 
	gEnergyCalibrationRead=false;
	gMielData 	= new TMielData(); 	// organise data
	gMielEvent 	= new TMielEvent();     // analyse data
  
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
		gNewTree->Branch("TMielData",&gMielData);
		gNewTree->Branch("TMielEvent",&gMielEvent);

		//Iterate through events
		Int_t nentries = (Int_t)gOldTree->GetEntries();
		cout << "Tree contains " << nentries <<endl;
		//nentries = 1000000 ; // experimenting value

		// progress bar variables
		char BarString[30] = "[                     ] <( )>";
		int  Loop = 0 ; 
		printf("Processing events from file %s ... %s (%d total events) (%d Miel events) ",inputname.c_str(), BarString, 0,0);
		fflush(stdout);
  
    	int GoodMiel = 0 ;
    	int j = 0 ; 
		for (j=0 ; j < nentries; j++) {
		
			// progress bar			
			if (j % 500 == 1 ) {
				printf("\rProcessing events from file %s ... %s (%d total events) (%d Miel events) ",inputname.c_str(), BarString, j,GoodMiel);
	   			fflush(stdout);
			    }
			if (j % 5000==1) BarString[26] = NextCycle();
			if (j % (nentries/20)==1) {
				BarString[Loop+1] = '=';
				printf("\rProcessing events from file %s ... %s (%d total events) (%d Miel events) ",inputname.c_str(), BarString, j,GoodMiel);
				fflush(stdout);
				Loop++ ; 
				}
			
			//Get the entry and set the events in the new Tree
			GoodEvent=false; 
			bool TimeOriginSet = false ;
			GetEvent(gOldTree, j);	

			int SEG = -1 ; // this is the origin of time
			for (int seg = 0 ; seg < 6 ; seg++ ) {
				if (gBuffer_energy.at(seg) > 0) { // keep this good event
					GoodMiel++;					
					GoodEvent=true;
					if(!TimeOriginSet) {
						SEG = seg ; 
						TimeOriginSet = true ;
						//cout << " SEG = "<< SEG <<"\t";
						//getchar();
					 	} 
					gMielData->SetMiel(seg, gBuffer_energy.at(seg),
					CalibrateMielEnergy(seg,gBuffer_energy.at(seg)),
					CalibrateMielTime(SEG,seg,gBuffer_time[SEG][seg]) ) ;
				}
				
			}
			

			//gammas
			gMielData->SetAptherix(gBuffer_Gamma);
			//Control Measurements
			gMielData->SetHallProbe(gBuffer_Hall);
			gMielData->SetVcontE(gBuffer_VContE);	
			gMielData->SetVContG(gBuffer_VContG);
			gMielData->SetChopper(gBuffer_Chopper);
		
			//FillHist(); 
			if (GoodEvent){
				//cout << " G O O D   E V E N T " << endl;	
				gMielEvent->SetMielData(gMielData); // Calculate positions, patterns, etc..
				gMielEvent->BuildAddBack(); //Calculate AddBack
				//PrintBasicMiel();
				//PrintBuffers();
				//gMielData->Print();				
				//gMielEvent->Print();
				//getchar() ;
				}
		
			gNewTree->Fill();	// fill the tree	
			gMielData->Clear();
			gMielEvent->Clear();
			if (j % 100000 == 0 ) gNewTree->AutoSave("FlushBaskets");  
			}
		printf("\rProcessing events from file %s ... %s (%d total events) (%d Miel events)  Done! \n",inputname.c_str(), BarString, j,GoodMiel);
		
		// Write the new trees in seperate files 
		gNewTree->Write();	// fill the tree	
		gOutputFile->Write();
		gOutputFile->Close(); 

		gNewTree=NULL;
		gOldTree=NULL;		
		gOutputFile=NULL;
		}//end of input files

	}//end 



// #########################################################################################
//                                functions start here 
// #########################################################################################

void ReadTimeCalibration(string inFile) {

  double channel, coeff;

  ifstream input(inFile);
  while (true) {
    input >> channel >> coeff;
    if (input.eof()) break;
    gTimeCalibration.push_back( coeff );
  }

  input.close();

}

void PrintBasicMiel(){

	cout << "======= Print basic Miel Event  ====  \n" ;
	cout << "======================================\n" ; 
	
	cout << "Energy : \n" ;
	cout << "E1" << " \t" << "E2" << " \t" << "E3" << " \t" << "E4" << " \t" << "E5" << " \t" << "E6" << " \n";
	cout << gMiel_st.Miel1E << " \t";
	cout << gMiel_st.Miel2E << " \t";
	cout << gMiel_st.Miel3E << " \t";
	cout << gMiel_st.Miel4E << " \t";
	cout << gMiel_st.Miel5E << " \t";
	cout << gMiel_st.Miel6E << " \n";
	
	cout << "Time : \n" ;
	cout << "T1T2" << " \t" << "T1T3" << " \t" << "T1T4" << " \t" << "T1T5" << " \t" << "T1T6" << " \n" ;
	cout << gMiel_st.MielT1T2 << " \t";
	cout << gMiel_st.MielT1T3 << " \t";
	cout << gMiel_st.MielT1T4 << " \t";
	cout << gMiel_st.MielT1T5 << " \t";
	cout << gMiel_st.MielT1T6 << " \n";

	cout << "T2T3" << " \t" << "T2T4" << " \t" << "T2T5" << " \t" << "T2T6" << " \n" ;
	cout << gMiel_st.MielT2T3 << " \t";
	cout << gMiel_st.MielT2T4 << " \t";
	cout << gMiel_st.MielT2T5 << " \t";
	cout << gMiel_st.MielT2T6 << " \n";
	
	cout << "T3T4" << " \t" << "T3T5" << " \t" << "T3T6" << " \n" ;
	cout << gMiel_st.MielT3T4 << " \t";
	cout << gMiel_st.MielT3T5 << " \t";
	cout << gMiel_st.MielT3T6 << " \n";

	cout << "T4T5" << " \t" << "T4T6" << " \n" ;
	cout << gMiel_st.MielT4T5 << " \t";
	cout << gMiel_st.MielT4T6 << " \n";

	cout << "T5T6" << " \n" ;
	cout << gMiel_st.MielT5T6 << " \n";
	
	cout << "Other : \n" ;	
	cout << "gamma" << " \t" << "hall" << " \t" << "VcontE" << " \t" << "VcontG" << " \n" ;
	cout << gMiel_st.Aptherix << " \t";
	cout << gMiel_st.HallProbe << " \t";
	cout << gMiel_st.VcontE << " \t";
	cout << gMiel_st.VcontG << " \t";
	cout << gMiel_st.Chopper << " \n";
	}
	
	
void PrintBuffers(){

	cout << "==========================\n" ; 
	cout << "======= Print buffers ====  \n" ;
	cout << "==========================\n" ; 

	cout << "Energy size: " << gBuffer_energy.size() <<"\n" ;
	for (unsigned i = 0 ; i < gBuffer_energy.size() ; i++ ) {
	cout << " "<< gBuffer_energy.at(i)<<"\t";
	}
	cout << "\n-------------------------\n" ; 

	cout << "Time: 6x6 Matrix "<<"\n" ;
			
	for (unsigned i = 0 ; i < 6 ; i++ ) {
			for (unsigned j = 0 ; j < 6 ; j++ ) {
			cout << " ("<<i<<","<<j<<")  "<< gBuffer_time[i][j]<<"\t";
			}
			cout << "\n";
	}
	cout << "\n-------------------------\n" ; 
	
	cout << "gBuffer_Gamma " << gBuffer_Gamma <<" \n";
		cout << "gBuffer_Hall " << gBuffer_Hall <<" \n";
			cout << "gBuffer_VContE " << gBuffer_VContE <<" \n";
				cout << "gBuffer_VContG " << gBuffer_VContG <<" \n";
					cout << "gBuffer_Chopper " << gBuffer_Chopper <<" \n";
	cout << "\n-------------------------\n" ; 
}

// ##############################
void ResetBuffers(){

	gBuffer_energy.clear(); 
	gBuffer_energy.resize(6,-1); 
	//ResetTimeBuffer();
	gBuffer_Gamma =   -1;
	gBuffer_Hall =   -1;
	gBuffer_VContE =  -1; 
	gBuffer_VContG =  -1;
    gBuffer_Chopper = -1;	
	
} 

// ##############################
void GetEvent(TTree* OldTree, int i){

	ResetBuffers();
	OldTree->GetEntry(i);
	
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

 		gBuffer_time[0][0] =   0; 
		gBuffer_time[0][1] =   gMiel_st.MielT1T2; 
		gBuffer_time[0][2] =   gMiel_st.MielT1T3; 
		gBuffer_time[0][3] =   gMiel_st.MielT1T4; 
		gBuffer_time[0][4] =   gMiel_st.MielT1T5; 
		gBuffer_time[0][5] =   gMiel_st.MielT1T6;

		gBuffer_time[1][0] =   -gBuffer_time[0][1];
		gBuffer_time[1][1] =   0;		 
		gBuffer_time[1][2] =   gMiel_st.MielT2T3;
		gBuffer_time[1][3] =   gMiel_st.MielT2T4;
		gBuffer_time[1][4] =   gMiel_st.MielT2T5;
		gBuffer_time[1][5] =   gMiel_st.MielT2T6;

		gBuffer_time[2][0] =   -gBuffer_time[0][2];
		gBuffer_time[2][1] =   -gBuffer_time[1][2];
		gBuffer_time[2][2] =   0;				
		gBuffer_time[2][3] =   gMiel_st.MielT3T4;
		gBuffer_time[2][4] =   gMiel_st.MielT3T5;
		gBuffer_time[2][5] =   gMiel_st.MielT3T6;
		
		gBuffer_time[3][0] =   -gBuffer_time[0][3];
		gBuffer_time[3][1] =   -gBuffer_time[1][3];
		gBuffer_time[3][2] =   -gBuffer_time[2][3];
		gBuffer_time[3][3] =   0;
		gBuffer_time[3][4] =   gMiel_st.MielT4T5;
		gBuffer_time[3][5] =   gMiel_st.MielT4T6;
				
		gBuffer_time[4][0] =   -gBuffer_time[0][4];
		gBuffer_time[4][1] =   -gBuffer_time[1][4];
		gBuffer_time[4][2] =   -gBuffer_time[2][4];
		gBuffer_time[4][3] =   -gBuffer_time[3][4];
		gBuffer_time[4][4] =   0;
		gBuffer_time[4][5] =   gMiel_st.MielT5T6;
		
		gBuffer_time[5][0] =   -gBuffer_time[0][5];
		gBuffer_time[5][1] =   -gBuffer_time[1][5];
		gBuffer_time[5][2] =   -gBuffer_time[2][5];
		gBuffer_time[5][3] =   -gBuffer_time[3][5];
		gBuffer_time[5][4] =   -gBuffer_time[4][5];
		gBuffer_time[5][5] =   0;
	}
	else{cout << "wrong size" ; exit(-1);}

		// gamma energies
	gBuffer_Gamma =   gMiel_st.Aptherix;
		//Hall probe 
	gBuffer_Hall  =   gMiel_st.HallProbe;
		//Normalizing parameters
	gBuffer_VContE =  gMiel_st.VcontE; 
	gBuffer_VContG =   gMiel_st.VcontG;	

    gBuffer_Chopper = gMiel_st.Chopper;

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
      gCalibration.push_back(sVector);
    }
    sCalFile.close(); 
  } else cout << "Unable to open file" << endl;

  int size = gCalibration.size();
  printf("\tRead in %d miel channels\n", size);

}

// ##############################
float CalibrateMielEnergy(int segment, int E_charge) {
  if (gEnergyCalibrationRead) {
    float temp = 0;
    for (int i=0; i<gCalibration.at(segment).size(); i++)
      temp += gCalibration.at(segment).at(i) * pow(static_cast<double>(E_charge), i);
    return temp ; 
  } else
    return E_charge;  
}

// ##############################
float CalibrateMielTime(int SEGMENT, int segment, int T_charge){
//need to implement 
return T_charge ; 
}


// ##############################
void ParseInputLine(int argc, char **argv) {

	if(argc < 2) {
	printf(" use : ./struct2class -c <calibration-file> -f <data-root-file> (XXX.root YYY.root .. etc...) \n");
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
		    ReadEnergyCalibration(gCalibrationFilesList.at(i));
		    gEnergyCalibrationRead=true;
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


void ResetTimeBuffer(){

for (int i = 0 ; i < 6 ; i++ )
	for (int j = 0 ; j < 6 ; j++ )
 		gBuffer_time[i][j] = -1; 

}

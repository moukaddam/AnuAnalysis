// #########################################################################################
// compile :
//  g++ struct2class.cxx libAnalysis/libMielData.so\
                -IlibAnalysis --std=c++0x -o struct2class\
                -O2 `root-config --cflags --libs`\
                -lTreePlayer -lgsl -lgslcblas 
// #########################################################################################


//c++
#include <stdio.h> //c
#include <stdlib.h> //c
#include <iostream>
#include <string>
#include <vector>
using namespace std;

//Root 
#include <TFile.h>
#include <TTree.h>
#include <TBranch.h>

//User 
#include "./libAnalysis/TMielData.h"



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

TMielData* gMielData; // new tree, vector based
TTree *gNewTree ;
TTree *gOldTree ;
TFile *gInputFile; 
TFile *gOutputFile;

//Buffer variables
vector <UShort_t>	gBuffer_energy; 
vector <UShort_t>	gBuffer_time; 
UShort_t gBuffer_gamma ; 
UShort_t gBuffer_hall ; 
UShort_t gBuffer_VContE ;  
UShort_t gBuffer_VContG ; 	

//Declare functions
void GetEvent(int i); 
void ResetBuffers(); 
float CalibrateEnergy(int segment, int E_charge); 
float CalibrateTime(int segment, int T_charge); 
void PrintBuffers();
void PrintMiel();

// #########################################################################################
//                                Main function
// #########################################################################################

int main(int argc, char **argv){
   
 if(argc < 2)	{
printf(" use : ./struct2class XXX.root (YYY.root .. etc...) \n");
return 1;
}

bool GoodEvent=false; 
gMielData = new TMielData();

// Iterate through the input files
for(int i=1;i<argc;i++)	{

	printf(" Reading file %s\t", argv[i]);
	string outputname = "Data_";
	string inputname = argv[i];
	outputname.append(inputname);	
	printf(" Output file %s\n", outputname.c_str());
	
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

	//Iterate through events
	Int_t nentries = (Int_t)gOldTree->GetEntries();
	cout << "Tree contains " << nentries <<endl;
	
				
	for (Int_t i=0 ; i < nentries; i++) {
		
		//Get the entry and set the events in the new Tree
		GoodEvent=false; 
		//cout << "Event Number : " << i <<endl;
		//gOldTree->Scan("*","","",1,i);
		GetEvent(i);	

		for (int j = 0 ; j < 6 ; j++ ) {
			if (gBuffer_energy.at(j) > 0) { // keep this good event
				GoodEvent=true; 
				gMielData->SetMiel(j/*segment*/, gBuffer_energy.at(j), CalibrateEnergy(j,gBuffer_energy.at(j)), gBuffer_time.at(j)) ;
			}
		}
		
		if (GoodEvent){
		gMielData->SetAptherix(gBuffer_gamma);
		gMielData->SetHallProbe(gBuffer_hall);
		gMielData->SetVcontE(gBuffer_VContE);	
		gMielData->SetVContG(gBuffer_VContG);
		
		//gMielData->Print();
		gNewTree->Fill();		
		}

		gMielData->Clear();
	}
	
	// Write the new trees in seperate files 
	gNewTree->AutoSave();  
	
	gOutputFile->Write();
	gOutputFile->Close();  
	
} //end of input files 
	
return 0 ; 	
} 


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

// ##############################
float CalibrateEnergy(int segment, int E_charge) {
//need to implement
return E_charge ; 
}

// ##############################
float CalibrateTime(int segment, int T_charge){
//need to implement 
return T_charge ; 
}




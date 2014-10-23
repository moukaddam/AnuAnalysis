/*****************************************************************************
 * Author: Mhd Moukaddam  contact address: moukaddam@triumf.ca      		 *
 *---------------------------------------------------------------------------*
 * Decription: This class stores the full event of singles and add-back
 			   It is the last step of the analysis procedure, 
 			   Any new Ad back schem should go in this class
 *---------------------------------------------------------------------------*
 * Comment:                                                                  *
 *                                                                           *
 *                                                                           *
 *****************************************************************************/

//c++
#include <iostream>
using namespace std;

// User classes
#include "TMiel.h"

//Root 
ClassImp(TMiel)

// -------------------------

TMiel::TMiel() {
	fMielEvent = new TMielEvent(); 
	Clear();
}

TMiel::~TMiel() {
}


void TMiel::Clear() {
	fMielEvent->Clear(); 
	fSum.clear(); 
	fCluster.clear(); 
}



void TMiel::Print() {
	cout << "       - M I E L  -       \n" ;
	cout << "==========================\n" ; 
		
	fMielEvent->Print();

	cout << "\n-----------C L U S T E R--------------\n" ; 
		for (unsigned i = 0 ; i < fSum.size() ; i++ ) {
		fSum.at(i)->Print();
		}
	cout << "\n-----------S U M -------------\n" ; 
		for (unsigned i = 0 ; i < fSum.size() ; i++ ) {
		fSum.at(i)->Print();
		}
	cout << "\n-------------------------\n" ; 
}


void TMiel::BuildHits(){
	
	fSum = fMielEvent->SumHits() ; 
	fCluster = fMielEvent->ClusterHits() ;
	
	//other Add-back schemes CALLS goes here 
	//...
}


void TMiel::SetMielEvent(TMielData* data){ // Take the elecron data from TMielData and store in a the TMielEvent of this class for further procedure 
 
	TMielHit* aMielHit = new TMielHit(); 
	unsigned mult = data->GetMultiplicity() ; 	
	for (unsigned i = 0 ; i < mult ; i++ ) {
		unsigned segment = data->GetSegment(i); 
		double    energy = data->GetEnergy(i);
		int       time   = (int) data->GetTime(i);
		aMielHit->Set(segment, energy, time) ;
		fMielEvent->PushHit(aMielHit);
		//clear
		aMielHit->Clear();
	}
}






	
	



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
   Clear();
}

TMiel::~TMiel() {
}


void TMiel::Clear() {
	fEvent.Clear(); 
	fSum.clear(); 
	fCluster.clear(); 
}



void TMiel::Print() {
	cout << "       - M I E L  -       \n" ;
	cout << "==========================\n" ; 
		
	fEvent->Print();

	cout << "\n-----------C L U S T E R--------------\n" ; 
		for (int i = 0 ; i < fSum.size() ; i++ ) {
		fSum.at(i)->Print();
		}
	cout << "\n-----------S U M -------------\n" ; 
		for (int i = 0 ; i < fSum.size() ; i++ ) {
		fSum.at(i)->Print();
		}
	cout << "\n-------------------------\n" ; 
}


void TMiel::BuildHits(){
	
	fSum = fEvent.SumHits() ; 
	fCluster = fEvent.ClusterHits() ;
	
	//other Add-back schemes goes here 
	//...
}


void TMiel::SetMielEvent(TMielData* data){

// Take the data elecron data from TMiel and store in a TMielEvent for fuurther procedure 
//	use inline void TMielHit::Set(UShort_t Seg, Double_t Energy, UInt_t Time)	{...}
// inline void TMielEvent::SetHits(std::vector<TMielHit*> vec)	{...} 
//


}






	
	



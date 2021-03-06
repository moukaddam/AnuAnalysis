/*****************************************************************************
 * Author: Mhd Moukaddam  contact address: moukaddam@triumf.ca      		 *
 *---------------------------------------------------------------------------*
 * Decription: This class stores the convered data from a root file 
               generated by the dcptoroot.cxx macro. 
               It is intended a an intermediate analysis class to arrange 
               the data in an optimized/condensed (zeros free) root tree 
               structure
 *---------------------------------------------------------------------------*
 * Comment:                                                                  *
 *                                                                           *
 *                                                                           *
 *****************************************************************************/

//c++
#include <iostream>
using namespace std;

// User classes
#include "TMielData.h"

//Root 
ClassImp(TMielData)

// -------------------------

TMielData::TMielData() {
   Clear();
}

TMielData::~TMielData() {
}


void TMielData::Clear() {

	fMielSegment.clear();
	fMielCharge.clear();
	fMielEnergy.clear();
    fMielBRho.clear();
	fMielTime.clear(); 

 	fMielChopper=-1;
	fMielAptherix=-1; 
	fMielHallProbe=-1; 
	fMielVcontE=-1; 
	fMielVcontG=-1; 

}



void TMielData::Print() {

	cout << "    M I E L -- D A T A   \n" ;
	cout << "==========================\n" ; 

	cout << "Miel Segment size : " << fMielSegment.size() <<"\n" ;
	for (unsigned i = 0 ; i < fMielSegment.size() ; i++ ) {
	cout << " "<< fMielSegment.at(i)<<"\t";
	}
	cout << "\n-------------------------\n" ; 
	
	cout << "Miel Charge size : " << fMielCharge.size() <<"\n" ;
	for (unsigned i = 0 ; i < fMielCharge.size() ; i++ ) {
	cout << " "<< fMielCharge.at(i)<<"\t";
	}
	cout << "\n-------------------------\n" ; 

	cout << "Miel Energy size : " << fMielEnergy.size() <<"\n" ;
	for (unsigned i = 0 ; i < fMielEnergy.size() ; i++ ) {
	cout << " "<< fMielEnergy.at(i)<<"\t";
	}
	cout << "\n-------------------------\n" ; 

  cout << "Miel BRho size : " << fMielBRho.size() <<"\n" ;
	for (unsigned i = 0 ; i < fMielBRho.size() ; i++ ) {
	cout << " "<< fMielBRho.at(i)<<"\t";
	}
	cout << "\n-------------------------\n" ; 

	cout << "Miel Time size   : " << fMielTime.size() <<"\n" ;
	for (unsigned i = 0 ; i < fMielTime.size() ; i++ ) {
	cout << " "<< fMielTime.at(i)<<"\t";
	}
	cout << "\n-------------------------\n" ; 

	cout << "Miel Gamma : " << fMielAptherix <<"\n" ;
	cout << "Miel HallProbe : " << fMielHallProbe <<"\n" ;
	cout << "Miel VcontE : " << fMielVcontE <<"\n" ;
	cout << "Miel VcontG : " << fMielVcontG <<"\n" ;
	cout << "Miel Chopper : " << fMielChopper <<"\n" ;
	
	cout << "\n-------------------------\n" ; 

}

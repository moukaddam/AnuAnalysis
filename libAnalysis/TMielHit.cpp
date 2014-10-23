/*****************************************************************************
 * Author: Mhd Moukaddam  contact address: moukaddam@triumf.ca      		 *
 *---------------------------------------------------------------------------*
 * Decription: This class stores the segment energy and time.
               It is the first step of the analysis procedure, th ebuilding stone 
               of any add back scheme 
 *---------------------------------------------------------------------------*
 * Comment:                                                                  *
 *                                                                           *
 *                                                                           *
 *****************************************************************************/

//c++
#include <iostream>
using namespace std;

// User classes
#include "TMielHit.h"

//Root 
ClassImp(TMielHit)

// -------------------------

TMielHit::TMielHit() {
   Clear();
}

TMielHit::~TMielHit() {
}


void TMielHit::Clear() {
	fSegment=-1;
	fEnergy=-1;
	fTime=-1;
	fPosition.SetXYZ(-1,-1,-1);  
}


void CalculatePosition()	{	
fPosition.SetXYZ(1,1,distance_to_target) ;	
fPosition.SetPhi( (fSegment + 0.5)*phi_pitch ) ; 
fPosition.SetPerp(mid_radius) ;
} 	
		

void TMielHit::Print() {
	cout << "    - M I E L  H I T -     \n" ;
	cout << "==========================\n" ; 
	cout << " Segment : " << fSegment <<"\n" ;
	cout << " Energy : " << fEnergy <<"\n" ;
	cout << " Time : " << fTime <<"\n" ;
	cout << " Position : \n" <<"X\tY\tZ\n"<< fPostion.X()<<"\t"<< fPostion.Y() <<"\t"<< fPostion.Z() << endl ;
	cout << "\n-------------------------\n" ; 

}



TMielHit* TMielHit::Copy(TMielHit* hit) {

this->SetEnergy(hit->GetEnergy());
this->SetSegment(hit->GetSegment());
this->SetTime(hit->GetTime());
this->SetPosition(hit->GetPosition()); 
return this; 

}



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
#include "TMielEvent.h"

//Root 
ClassImp(TMielEvent)

// -------------------------

TMielEvent::TMielEvent() {
   Clear();
}

TMielEvent::~TMielEvent() {
}


void TMielEvent::Clear() {
	fHits.clear(); //single
	fPattern =  0 ; 
}



void TMielEvent::Print() {
	cout << "  - M I E L  E V E N T -  \n" ;
	cout << "==========================\n" ; 
	for (int i = 0 ; i < fHits.size() ; i++ ) {
		fHits.at(i)->Print();
		}
	cout << "\n-------------------------\n" ; 

}

unsigned short TMielEvent::GetPattern() {

/*
This functions return the pattern of the fired segments, 1 for hit, 0 for no-hit, it uses the vector sum to tag the different patterns
	if segment 1 and 3 fired : pattern is 101000
The number should be the greatest possible 
	e.g. if multiplicity=1  possible values {001000, 000010 etc..} reduces to 100000 
fold-{patterns}
---------------
	1-{100000}
	2-{110000, 101000, 100100}
	3-{111000, 101100, 101010}
	4-{111100, 111010, 110011}
	5-{111110},
	6-{111111}
*/

Double_t epsilon= 0.001 ; // small length to compare magnitudes a magnitude < epsilon it is practically zero

//simple cases 
	if (fHits.size()==1) return 100000;
	if (fHits.size()==6) return 111111;
	if (fHits.size()==5) return 111110;

//other cases 	 
	if (fHits.size()==2){
	TVector3 A = fHits.at(0)->GetPosition() ; 
	TVector3 B = fHits.at(1)->GetPosition() ;
	// reduce to polar coordinates only
	 A = A.SetXYZ(A.X(),A.Y(),0) ; 
	 B = B.SetXYZ(B.X(),B.Y(),0) ;
	 TVector3 Sum = A + B ; 
	 
	 if (Sum.Mag() > A.Mag ) return 110000 ;
	 else if (Sum.Mag() < epsilon ) return 100100 ;
	 	  else return 101000;

	}

	if (fHits.size()==3){
	TVector3 A = fHits.at(0)->GetPosition() ; 
	TVector3 B = fHits.at(1)->GetPosition() ;
	TVector3 C = fHits.at(2)->GetPosition() ; 
	// reduce to polar coordinates only
	 A = A.SetXYZ(A.X(),A.Y(),0) ; 
	 B = B.SetXYZ(B.X(),B.Y(),0) ;
	 C = C.SetXYZ(C.X(),C.Y(),0) ;
	 TVector3 Sum = A + B + C ; 
	 
	 if (Sum.Mag() > A.Mag ) return 111000 ;
	 else if (Sum.Mag() < epsilon ) return 101010 ;
	 	  else return 110100;
	}


	if (fHits.size()==4){
	TVector3 A = fHits.at(0)->GetPosition() ; 
	TVector3 B = fHits.at(1)->GetPosition() ;
	TVector3 C = fHits.at(2)->GetPosition() ;
	TVector3 D = fHits.at(3)->GetPosition() ; 
	// reduce to polar coordinates only
	 A = A.SetXYZ(A.X(),A.Y(),0) ; 
	 B = B.SetXYZ(B.X(),B.Y(),0) ;
	 C = C.SetXYZ(C.X(),C.Y(),0) ;
	 D = C.SetXYZ(C.X(),C.Y(),0) ;

	 TVector3 Sum = A + B + C + D;
	 
	 if (Sum.Mag() > A.Mag ) return 111100 ;
	 else if (Sum.Mag() < epsilon ) return 1101100 ;
	 	  else return 111010;
	}


}

TMielEvent*  Add(std::vector<TMielHit*> hits) { // Add a vector of hits 

TMielEvent* dummy ; 

//calculate full energy 
double   full = 0  ;
unsigned index = 0  ; // tag maximum energy
for (int i = 0 ; i < hits.size() ; i++ ) {
	full = full + hits.at(i)->GetEnergy();
	if (hits.at(index)->GetEnergy() < hits.at(i)->GetEnergy() ) index=i ;
	}
dummy->SetEnergy(full) ; 
	
	
//calculate segment 
unsigned segment = 0 ; 
for (int i = 0 ; i < hits.size() ; i++ ) {
	segment = segment + hits.at(i)->GetSegment());  // e.g if segment 1+2 => Segment = 12
	}
dummy->SetSegment(segment) ; 
	
	
//calculate time 
dummy->SetTime(hits.at(index)->GetTime();


//calculate position
TVector3 position = hits.begin()->GetPosition(); // copy one position (to keep cylindrical Z and R)
double phi=0 ;  
for (int i = 0 ; i < hits.size() ; i++ ) {
	phi = phi + ( hits.at(i)->GetPosition()->GetPhi() * (hits.at(i)->GetEnergy()/full) );  // weighted position
	}
position.SetPhi(phi);  
dummy->SetPosition(position);
	
	return dummy ; 
}

		//Add-Back schemes 
		
std::vector<TMielHit*> SumHits(){  // Calorimeter mode, sums all the hits and returns a singleton vector

	std::vector<TMielHit*> vec{ Add(fHits) };
	return vec ; 
	}
	
std::vector<TMielHit*> ClusterHits() {  // Cluster mode, only those hits with touching segments are summed

//simple cases return sum of hits 
	if (fHits.size()==1) return SumHits(fHits); 
	if (fHits.size()==6) return SumHits(fHits);
	if (fHits.size()==5) return SumHits(fHits);

//other cases 	 
	if (fHits.size()==2){
		TMielHit* A = fHits.at(0) ; 
		TMielHit* B = fHits.at(1) ;
		// test if the segments are touching
		 if ( fPattern==110000 ) {
			std::vector<TMielHit*> vec; 
			vec.push_back(Add(hits));
			return sum ; 
		   ;
		 else return hits;
		}

	if (fHits.size()==3){
		//simple case 
		if (fPattern==111000) return SumHits(hits) ; // sum of all
		if (fPattern==101010) return hits ; // unchanged 
	    if (fPattern==110100) {
	    //copy hits 
	    std::vector<TMielHit*> hits_new ;
	    //initiate the new hits 

	    //Get the hits one by one 
		TMielHit* A = fHits.at(0) ; 
		TMielHit* B = fHits.at(1) ;
		TMielHit* C = fHits.at(2) ;
		
		std::vector<TMielHit*>  cluster1 ;
	    std::vector<TMielHit*>  cluster2 ; 
	    
	    cluster1.push_back(A) ; 
		if (Contact(A,B)) {
			cluster1.push_back(B) ; // Add hits returns a TMielHit*
			cluster2.push_back(C) ; 
		}
		else {
			cluster1.push_back(C);
			cluster2.push_back(B);
		}
		
		//build the new hits and return it
			//hits_new.push_back(Add(cluster1));
	    	//hits_new.push_back(Add(cluster2));
			return std::vector<TMielHit*> vec{ Add(cluster1), Add(cluster2) };	   	
	    }	 	  
		
	}


	if (fHits.size()==4){
			//simple case 
		if (fPattern==111100) return SumHits(hits) ; // sum of all
		else {
				TMielHit* A = fHits.at(0); 
				TMielHit* B = fHits.at(1);
				TMielHit* C = fHits.at(2);
				TMielHit* D = fHits.at(3); 

				std::vector<TMielHit*>  cluster1 ;
				std::vector<TMielHit*>  cluster2 ; 
	
				cluster1.push_back(A) ; 
				if (fPattern==110110){
	
					if (Contact(A,B)){			
					cluster1.push_back(B) ; // Add hits returns a TMielHit*
					cluster2.push_back(C) ;
					cluster2.push_back(D) ;
					return std::vector<TMielHit*> vec{ Add(cluster1), Add(cluster2) };	
					}
					else if (Contact(A,C)){
						cluster1.push_back(C) ; // Add hits returns a TMielHit*
						cluster2.push_back(B) ;
						cluster2.push_back(D) ;
						return std::vector<TMielHit*> vec{ Add(cluster1), Add(cluster2) };	
						}
						else {
							cluster1.push_back(D) ; // Add hits returns a TMielHit*
							cluster2.push_back(B) ;
							cluster2.push_back(C) ;
							return std::vector<TMielHit*> vec{ Add(cluster1), Add(cluster2) };	
						}	
				}
		
				if(fPattern==111010){
					TVector3 vA = fHits.at(0)->GetPosition() ; 
					TVector3 vB = fHits.at(1)->GetPosition() ;
					TVector3 vC = fHits.at(2)->GetPosition() ;
					TVector3 vD = fHits.at(3)->GetPosition() ; 
		
					// reduce to polar coordinates only
					vA = vA.SetXYZ(vA.X(),vA.Y(),0) ; 
					vB = vB.SetXYZ(vB.X(),vB.Y(),0) ;
					vC = vC.SetXYZ(vC.X(),vC.Y(),0) ;
					vD = vC.SetXYZ(vC.X(),vC.Y(),0) ;

					TVector3 Sum = vA + vB + vC + vD; // the Sum-vector makes an angle of 180 degrees with the position vector of the lonesome segment
		
					if (Sum.Dot(vA) < 0)  { cluster1 = { vA }; cluster2 = { vB,vC,vD }; }
						else if (Sum.Dot(vB) < 0)  { cluster1 = { vB }; cluster2 = { vA,vC,vD }; }
							else if (Sum.Dot(vC) < 0)  { cluster1 = { vC }; cluster2 = { vA,vB,vD }; }
								else { cluster1 = { vD }; cluster2 = { vA,vB,vC }; }
		
		 			return std::vector<TMielHit*> vec{ Add(cluster1), Add(cluster2) };	
				}
			}
	}
	
}
	
bool Contact(TMielHit* SegmentA, TMielHit* SegmentB){
	
	TVector3 A = SegmentA->GetPosition() ; 
	TVector3 B = SegmentB->GetPosition() ;
	// reduce to polar coordinates only
	 A = A.SetXYZ(A.X(),A.Y(),0) ; 
	 B = B.SetXYZ(B.X(),B.Y(),0) ;
	 TVector3 Sum = A + B ; 
	 
	 if (Sum.Mag() > A.Mag ) return true ;
	 else return false;
	
}
	
	
	



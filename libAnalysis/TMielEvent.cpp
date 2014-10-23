/*****************************************************************************
 * Author: Mhd Moukaddam  contact address: moukaddam@triumf.ca      		 *
 *---------------------------------------------------------------------------*
 * Decription: This class is the core of the analysis 
 			A miel event will contain all the information of a hit in the SiLi 
 			All type of Add back schemes should be implemented here.
 			Everything that is unique to an event (such as the singles patterns)
 			Should go here.
 			From this class instance one can withdraw a vector of hit 
 			related to a certain add-back scheme
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
#include "TMielHit.h"

//Root 
ClassImp(TMielEvent)



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
	for (unsigned i = 0 ; i < fHits.size() ; i++ ) {
		fHits.at(i)->Print();
		}
	cout << "\n-------------------------\n" ; 
}


 void TMielEvent::SetHits( vector<TMielHit*> vec)	{	
 fHits = vec ; 
 GetPattern() ;
 } 
 
 void TMielEvent::PushHit(TMielHit* hit) {
 fHits.push_back(hit);
 GetPattern() ; 
 } 
	
unsigned TMielEvent::GetPattern() {

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

Double_t epsilon= 0.001 ; // small length to compare magnitudes, if magnitude < epsilon is practically zero

//simple cases 
	if (fHits.size()==1) return 100000;
	if (fHits.size()==6) return 111111;
	if (fHits.size()==5) return 111110;

//other cases 	 
	if (fHits.size()==2){
	TVector3 A = fHits.at(0)->GetPosition() ; 
	TVector3 B = fHits.at(1)->GetPosition() ;
	// reduce to polar coordinates only
	 A.SetXYZ(A.X(),A.Y(),0) ; 
	 B.SetXYZ(B.X(),B.Y(),0) ;
	 TVector3 Sum = A + B ; 
	 
	 if (Sum.Mag() > A.Mag() ) return 110000 ;
	 else if (Sum.Mag() < epsilon ) return 100100 ;
	 	  else return 101000;

	}

	if (fHits.size()==3){
	TVector3 A = fHits.at(0)->GetPosition() ; 
	TVector3 B = fHits.at(1)->GetPosition() ;
	TVector3 C = fHits.at(2)->GetPosition() ; 
	// reduce to polar coordinates only
	 A.SetXYZ(A.X(),A.Y(),0) ; 
	 B.SetXYZ(B.X(),B.Y(),0) ;
	 C.SetXYZ(C.X(),C.Y(),0) ;
	 TVector3 Sum = A + B + C ; 
	 
	 if (Sum.Mag() > A.Mag() ) return 111000 ;
	 else if (Sum.Mag() < epsilon ) return 101010 ;
	 	  else return 110100;
	}


	if (fHits.size()==4){
	TVector3 A = fHits.at(0)->GetPosition() ; 
	TVector3 B = fHits.at(1)->GetPosition() ;
	TVector3 C = fHits.at(2)->GetPosition() ;
	TVector3 D = fHits.at(3)->GetPosition() ; 
	// reduce to polar coordinates only
	 A.SetXYZ(A.X(),A.Y(),0) ; 
	 B.SetXYZ(B.X(),B.Y(),0) ;
	 C.SetXYZ(C.X(),C.Y(),0) ;
	 D.SetXYZ(D.X(),D.Y(),0) ;

	 TVector3 Sum = A + B + C + D;
	 
	 if (Sum.Mag() > A.Mag() ) return 111100 ;
	 else if (Sum.Mag() < epsilon ) return 1101100 ;
	 	  else return 111010;
	}

	// by default
	return 000000; 
}

TMielHit*  TMielEvent::Add( vector<TMielHit*> hits) { // Add a vector of hits 

TMielHit* aMielHit = new TMielHit(); 

//calculate full energy 
double   full = 0  ;
unsigned index = 0  ; // tag maximum energy
for (unsigned i = 0 ; i < hits.size() ; i++ ) {
	full = full + hits.at(i)->GetEnergy();
	if (hits.at(index)->GetEnergy() < hits.at(i)->GetEnergy() ) index=i ;
	}
aMielHit->SetEnergy(full) ; 
	
	
//calculate segment 
unsigned short segment = 0 ; 
for (unsigned i = 0 ; i < hits.size() ; i++ ) {
	segment = segment + hits.at(i)->GetSegment();  // e.g if segment 1+2 => Segment = 12
	}
aMielHit->SetSegment(segment) ; 
	
	
//calculate time 
aMielHit->SetTime(hits.at(index)->GetTime());


//calculate position
TVector3 position = hits.at(0)->GetPosition(); // copy one position (to keep cylindrical Z and R)
double phi=0 ;  
for (unsigned i = 0 ; i < hits.size() ; i++ ) {
	phi = phi + ( hits.at(i)->GetPosition().Phi() * (hits.at(i)->GetEnergy()/full) );  // weighted position
	}
position.SetPhi(phi);  
aMielHit->SetPosition(position);
	
	return aMielHit ; 
}

		//Add-Back schemes 
		
 vector<TMielHit*> TMielEvent::SumHits(){  // Calorimeter mode, sums all the hits and returns a singleton vector

	 vector<TMielHit*> vec{ Add(fHits) };
	return vec ; 
	}
	
 vector<TMielHit*> TMielEvent::ClusterHits() {  // Cluster mode, only those hits with touching segments are summed

//simple cases return sum of hits 
	if (fHits.size()==1) return  SumHits(); 
	if (fHits.size()==6) return  SumHits();
	if (fHits.size()==5) return  SumHits();


//other cases 	 
	if (fHits.size()==2){
		// test if the segments are touching
		 if ( fPattern==110000 ) {
				return SumHits() ;
				} 
		 else return fHits;
		}
		
		
	if (fHits.size()==3){
		//simple cases 
		if (fPattern==111000) return SumHits() ; // sum of all
		if (fPattern==101010) return fHits; // unchanged 
	    
	    //other case 
	    if (fPattern==110100) {
	    //Get the hits one by one 
		TMielHit* A = fHits.at(0) ; 
		TMielHit* B = fHits.at(1) ;
		TMielHit* C = fHits.at(2) ;

		vector<TMielHit*>  aHitCluster1 ;
		vector<TMielHit*>  aHitCluster2 ; 
	    
	    aHitCluster1.push_back(A) ; 
		if (Contact(A,B)) {
			aHitCluster1.push_back(B) ; 
			aHitCluster2.push_back(C) ; 
		}
		else {
			aHitCluster1.push_back(C);
			aHitCluster2.push_back(B);
		}
		
		//build the new hits and return it
		//hits_new.push_back(Add(aHitCluster1));
		//hits_new.push_back(Add(aHitCluster2));
		vector<TMielHit*> vec{ Add(aHitCluster1), Add(aHitCluster2) }; // Add hits returns a TMielHit*
		return  vec ;	
	    }	 	  	
	}


	if (fHits.size()==4){
			//simple case 
		if (fPattern==111100) return SumHits() ; // sum of all
		else {
				TMielHit* A = fHits.at(0); 
				TMielHit* B = fHits.at(1);
				TMielHit* C = fHits.at(2);
				TMielHit* D = fHits.at(3); 

				vector<TMielHit*>  aHitCluster1 ;
				vector<TMielHit*>  aHitCluster2 ; 
	
				aHitCluster1.push_back(A) ;
				 
				if (fPattern==110110){
					if (Contact(A,B)){			
					aHitCluster1.push_back(B) ; // Add hits returns a TMielHit*
					aHitCluster2.push_back(C) ;
					aHitCluster2.push_back(D) ;
					}
					else if (Contact(A,C)){
						aHitCluster1.push_back(C) ; // Add hits returns a TMielHit*
						aHitCluster2.push_back(B) ;
						aHitCluster2.push_back(D) ;
						}
						else {
							aHitCluster1.push_back(D) ; // Add hits returns a TMielHit*
							aHitCluster2.push_back(B) ;
							aHitCluster2.push_back(C) ;
						}
				vector<TMielHit*> vec{ Add(aHitCluster1), Add(aHitCluster2) }; // Add hits returns a TMielHit*
				return  vec ;		
				}
		
				if(fPattern==111010){
					TVector3 vA = fHits.at(0)->GetPosition() ; 
					TVector3 vB = fHits.at(1)->GetPosition() ;
					TVector3 vC = fHits.at(2)->GetPosition() ;
					TVector3 vD = fHits.at(3)->GetPosition() ; 
		
					// reduce to polar coordinates only
					vA.SetXYZ(vA.X(),vA.Y(),0) ; 
					vB.SetXYZ(vB.X(),vB.Y(),0) ;
					vC.SetXYZ(vC.X(),vC.Y(),0) ;
					vC.SetXYZ(vC.X(),vC.Y(),0) ;

					TVector3 Sum = vA + vB + vC + vD; // the Sum-vector makes an angle of 180 degrees with the position vector of the lonesome segment
		
					if (Sum.Dot(vA) < 0)  				{ aHitCluster1 = { A }; aHitCluster2 = { B,C,D }; }
						else if (Sum.Dot(vB) < 0)  		{ aHitCluster1 = { B }; aHitCluster2 = { A,C,D }; }
							else if (Sum.Dot(vC) < 0)  	{ aHitCluster1 = { C }; aHitCluster2 = { A,B,D }; }
								else 					{ aHitCluster1 = { D }; aHitCluster2 = { A,B,C }; }
		
		 			vector<TMielHit*> vec{ Add(aHitCluster1), Add(aHitCluster2) }; // Add hits returns a TMielHit*
					return  vec ;	
				}// end of case 111010 
			} // end of non-trivial case of 4 multiplicity
	}// end of case = 4 
	
	// by default
	return fHits; // unchanged 
}
	
bool TMielEvent::Contact(TMielHit* SegmentA, TMielHit* SegmentB){
	
	TVector3 A = SegmentA->GetPosition() ; 
	TVector3 B = SegmentB->GetPosition() ;
	// reduce to polar coordinates only
	 A.SetXYZ(A.X(),A.Y(),0) ; 
	 B.SetXYZ(B.X(),B.Y(),0) ;
	 TVector3 Sum = A + B ; 
	 
	 if (Sum.Mag() > A.Mag() ) return true ;
	 else 
	 return false;
	
}
	
	
	



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
#include <algorithm>    // std::sort

// User classes
#include "TMielEvent.h"

//Root 
ClassImp(TMielEvent)



TMielEvent::TMielEvent() {
   Clear();
}

TMielEvent::~TMielEvent() {
}


void TMielEvent::Clear() {

	fHits.clear();
	fCluster.clear();
	fPair.clear(); 	 
	
	fSum.Clear();
	//fPairSum.Clear(); 
		 		
	fPattern =  0 ;
	fPairRatioE1E2 = 0; 
	fRatioE1E2 = 0 ; 
	fConflict = 0 ;
	 
}


void TMielEvent::Print() {
	cout << "==========================\n" ;
	cout << "  - M I E L  E V E N T -  \n" ;
	cout << "==========================\n" ;
	
	cout << " Hit pattern : " << fPattern  << endl ; 
	cout << " Energy ratio for the 2 top energies : " << fRatioE1E2  << endl ; 
	cout << " Energy ratio for the 2 top energies in pair mode : " << fPairRatioE1E2  << endl ; 
	cout << " Conflict in adding the energies  : " << fConflict  << endl ; 	
	
	cout << "\n-----------S I N G L E S-------------\n" ; 
	for (unsigned i = 0 ; i < fHits.size() ; i++ ) {
	fHits.at(i).Print();
	}
	
	cout << "\n-----------S U M -------------\n" ; 
	fSum.Print();
	
	cout << "\n-----------C L U S T E R-------\n" ; 
	for (unsigned i = 0 ; i < fCluster.size() ; i++ ) {
	fCluster.at(i).Print();
	}
	
	cout << "\n-----------P A I R-------------\n" ; 
	for (unsigned i = 0 ; i < fPair.size() ; i++ ) {
	fPair.at(i).Print();
	}
	
	//cout << "\n-----------P A I R  S U M------\n" ; 
	//fPairSum.Print();
	
}


 void TMielEvent::SetHits( vector<TMielHit>* vec)	{	
 fHits = *vec ; 
 CalculatePattern() ;
 } 
 
 void TMielEvent::PushHit(TMielHit hit) {
	fHits.push_back(hit);  
	CalculatePattern() ; 
 } 
	
void TMielEvent::CalculatePattern() {

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

Double_t epsilon= 1E-3 ; // small length to compare magnitudes, if magnitude < epsilon is practically zero

//simple cases 
	if (fHits.size()==1) fPattern =100000;
	if (fHits.size()==6) fPattern =111111;
	if (fHits.size()==5) fPattern =111110;

//other cases 	 
	if (fHits.size()==2){
	TVector3 A = fHits.at(0).GetPosition() ;
	TVector3 B = fHits.at(1).GetPosition() ;
	// reduce to polar coordinates only
	 A.SetXYZ(A.X(),A.Y(),0) ; 
	 B.SetXYZ(B.X(),B.Y(),0) ;
	 A.Unit();
	 B.Unit();
	 TVector3 Sum = A + B ;

	 if (Sum.Mag() > 1.5*A.Mag() ) fPattern =110000 ;  // in case of an angle of 120degree, the magnitude of the Sum is a unit(=Magnitude of A)
	 else if (Sum.Mag() < epsilon ) fPattern =100100 ;
	 	  else fPattern =101000;

	}

	if (fHits.size()==3){
	TVector3 A = fHits.at(0).GetPosition() ; 
	TVector3 B = fHits.at(1).GetPosition() ;
	TVector3 C = fHits.at(2).GetPosition() ; 
	// reduce to polar coordinates only
	 A.SetXYZ(A.X(),A.Y(),0) ; 
	 B.SetXYZ(B.X(),B.Y(),0) ;
	 C.SetXYZ(C.X(),C.Y(),0) ;
	 A.Unit();
	 B.Unit();
	 C.Unit();
	 TVector3 Sum = A + B + C ; 
	 	 
	 //A.Dump();
	 //B.Dump();
	 //C.Dump();
	 //Sum.Dump();
	 
	 if (Sum.Mag() > 1.5*A.Mag() ) fPattern =111000 ;
	 else if (Sum.Mag() < epsilon ) fPattern =101010 ;
	 	  else fPattern =110100;
	}


	if (fHits.size()==4){
	TVector3 A = fHits.at(0).GetPosition() ; 
	TVector3 B = fHits.at(1).GetPosition() ;
	TVector3 C = fHits.at(2).GetPosition() ;
	TVector3 D = fHits.at(3).GetPosition() ; 
	// reduce to polar coordinates only
	 A.SetXYZ(A.X(),A.Y(),0) ; 
	 B.SetXYZ(B.X(),B.Y(),0) ;
	 C.SetXYZ(C.X(),C.Y(),0) ;
	 D.SetXYZ(D.X(),D.Y(),0) ;
	 A.Unit();
	 B.Unit();
	 C.Unit();
	 D.Unit();	 	 
	 TVector3 Sum = A + B + C + D;

	 //A.Dump();
	 //B.Dump();
	 //C.Dump();
	 //D.Dump();
	 //Sum.Dump();
	 	 
	 if (Sum.Mag() > 1.5*A.Mag() ) fPattern =111100 ;
	 else if (Sum.Mag() < epsilon ) fPattern =1101100 ;
	 	  else fPattern =111010;
	}


}

TMielHit  TMielEvent::AddHits( const vector<TMielHit> &hits) { // Add a vector of hits 

	TMielHit aMielHit; 

	//calculate full energy 
	double   full = 0  ;
	unsigned index = 0  ; // tag maximum energy
	for (unsigned i = 0 ; i < hits.size() ; i++ ) {
		full = full + hits.at(i).GetEnergy();
		if (hits.at(index).GetEnergy() < hits.at(i).GetEnergy() ) index=i ;
		}
	aMielHit.SetEnergy(full) ; 
	
	
	//calculate segment	
	//get the segments queue 
	vector<unsigned short> segment ;
	unsigned short finalsegment = 0 ; 
	for (unsigned i = 0 ; i < hits.size() ; i++ ) {
		segment.push_back(hits.at(i).GetSegment()); 
		}
	// sort it	
	sort(segment.begin(), segment.end());	
	// store it
	for (unsigned i = 0 ; i < segment.size() ; i++ ) {
		finalsegment = 10*finalsegment + (segment.at(i) );  // segments{1,2,..6}  e.g if segment 1+4  => Final Segment = (10*0) + (1) = 1 ( 1st iteration ) => (10*1) + (4) = 14 
		}
	aMielHit.SetSegment(finalsegment) ; 


	//calculate time 
	aMielHit.SetTime(hits.at(index).GetTime());


	//calculate position
	TVector3 position = hits.at(0).GetPosition(); // copy one position (to keep cylindrical Z and R)
	double phi=0 ;  
	for (unsigned i = 0 ; i < hits.size() ; i++ ) {
		phi = phi + ( hits.at(i).GetPosition().Phi() * (hits.at(i).GetEnergy()/full) );  // weighted position
		}
	position.SetPhi(phi);  
	aMielHit.SetPosition(position);
	
	return aMielHit ; 
}



	//Add-Back schemes 
		
 void TMielEvent::SumHits(){  // Calorimeter mode, sums all the hits and returns a singleton vector
	fSum = AddHits(fHits) ;
	}
	
	
 void TMielEvent::ClusterHits() {  // Cluster mode, only those hits with touching segments are summed

//simple cases return sum of hits 
	if (fHits.size()==1) { vector<TMielHit> hitvec ; hitvec.push_back(fSum) ; fCluster = hitvec ;}; 
	if (fHits.size()==6) { vector<TMielHit> hitvec ; hitvec.push_back(fSum) ; fCluster = hitvec ;};
	if (fHits.size()==5) { vector<TMielHit> hitvec ; hitvec.push_back(fSum) ; fCluster = hitvec ;};


//other cases 	 
	if (fHits.size()==2){
		// test if the segments are touching
		 if ( fPattern==110000 ) {
				vector<TMielHit> hitvec ; hitvec.push_back(fSum) ; fCluster = hitvec ;
				} 
		 else fCluster = fHits;
		}
		
		
	if (fHits.size()==3){
		//simple cases 
		if (fPattern==111000) { vector<TMielHit> hitvec ; hitvec.push_back(fSum) ; fCluster = hitvec ;} // sum of all
		if (fPattern==101010) fCluster = fHits; // unchanged 
	    
	    //other case 
	    if (fPattern==110100) {
	    //Get the hits one by one 
		TMielHit A = fHits.at(0) ; 
		TMielHit B = fHits.at(1) ;
		TMielHit C = fHits.at(2) ;
			
		vector<TMielHit>  aHitCluster1 ;
		vector<TMielHit>  aHitCluster2 ; 
	    
	    aHitCluster1.push_back(A) ; 
		if (PositionAdjacent(A.GetPosition().XYvector(),B.GetPosition().XYvector())) {
			aHitCluster1.push_back(B) ; 
			aHitCluster2.push_back(C) ; 
		}
		else {
			aHitCluster1.push_back(C);
			aHitCluster2.push_back(B);
		}

		//build the new hits and return it
		 vector<TMielHit> hitvec ; 
		 hitvec.push_back(AddHits(aHitCluster1)) ;
		 hitvec.push_back(AddHits(aHitCluster2)) ;
		 fCluster = hitvec ; 
	    }	 	  	
	}


	if (fHits.size()==4){
			//simple case 
		if (fPattern==111100) { vector<TMielHit> hitvec ; hitvec.push_back(fSum) ; fCluster = hitvec ;} // sum of all
		else {
				TMielHit A = fHits.at(0); 
				TMielHit B = fHits.at(1);
				TMielHit C = fHits.at(2);
				TMielHit D = fHits.at(3); 

				vector<TMielHit>  aHitCluster1 ;
				vector<TMielHit>  aHitCluster2 ; 
	
				aHitCluster1.push_back(A) ;
				 
				if (fPattern==110110){
					if (PositionAdjacent( A.GetPosition().XYvector() , B.GetPosition().XYvector() )){			
					aHitCluster1.push_back(B) ; // Add hits returns a TMielHit*
					aHitCluster2.push_back(C) ;
					aHitCluster2.push_back(D) ;
					}
					else if (PositionAdjacent( A.GetPosition().XYvector() , C.GetPosition().XYvector() )){
						aHitCluster1.push_back(C) ; // Add hits returns a TMielHit*
						aHitCluster2.push_back(B) ;
						aHitCluster2.push_back(D) ;
						}
						else {
							aHitCluster1.push_back(D) ; // Add hits returns a TMielHit*
							aHitCluster2.push_back(B) ;
							aHitCluster2.push_back(C) ;
						}
						
					//build the new hits and return it
					 vector<TMielHit> hitvec ; 
					 hitvec.push_back(AddHits(aHitCluster1)) ;
					 hitvec.push_back(AddHits(aHitCluster2)) ;
					 fCluster = hitvec ; 
				}
		
				if(fPattern==111010){
					TVector3 vA = fHits.at(0).GetPosition() ; 
					TVector3 vB = fHits.at(1).GetPosition() ;
					TVector3 vC = fHits.at(2).GetPosition() ;
					TVector3 vD = fHits.at(3).GetPosition() ; 
		
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
		
					//build the new hits and return it
					 vector<TMielHit> hitvec ; 
					 hitvec.push_back(AddHits(aHitCluster1)) ;
					 hitvec.push_back(AddHits(aHitCluster2)) ;
					 fCluster = hitvec ; 
				}// end of case 111010 
			} // end of non-trivial case of 4 multiplicity
	}// end of case = 4 

}
	
	
	
	
 void TMielEvent::PairHits() {  // Pair mode, hits are summed only if they pass the Adjacent position/time tests, all other are discraded
 
 // if one segment is hit skip event
 if ( fHits.size() <= 1) {
	 fPair = fHits ;
	 fRatioE1E2 = 0 ; 
	 fPairRatioE1E2 = 0 ; 
	 fConflict = 0 ; 
	 return ; 
	 }
 
 // Copy fHits to hits
 vector<TMielHit> hits = fHits ; 
 vector<TMielHit> Queue1 ; 
 vector<TMielHit> Queue2 ; 
 
 // re-arrange the fHits vector according to the energy
 // select the top 2 energies segments, Major-1 and Major-2 and create the pool (a vector) for the other segments
 unsigned index1 = 0 ; // Major Energy 1  
 unsigned index2 = 0 ; // Major Energy 1 
 
 	
 for (unsigned i = 0 ; i < hits.size() ; i++ ) {
 	if (hits.at(i).GetEnergy() > hits.at(index1).GetEnergy()) index1 = i ;  	
 	}
 	Queue1.push_back(hits.at(index1)) ; 
 	hits.erase(hits.begin()+(index1));
 		 	
 for (unsigned i = 0 ; i < hits.size() ; i++ ) {
 	if (hits.at(i).GetEnergy() > hits.at(index2).GetEnergy()) index2 = i ;  	
 	}
 	Queue2.push_back(hits.at(index2)) ;  
 	hits.erase(hits.begin()+(index2));
 	 	 	
 // calculate the energy ratio
 fRatioE1E2 = Queue2.at(0).GetEnergy() / Queue1.at(0).GetEnergy();	
   	
 if (fHits.size() == 2 ) {
	 fPair.push_back(Queue1.at(0)) ;
	 fPair.push_back(Queue2.at(0)) ;
	 fPairRatioE1E2 = fRatioE1E2; 
	 fConflict = 0 ; 
	 return ; 
	 }
	
 //create the decision variable 
 int decision = 0  ;
 bool SolveOnce = 1  ;
 float EnergyQ1 = Queue1.at(0).GetEnergy()  ;
 float EnergyQ2 = Queue2.at(0).GetEnergy()  ;
    
 // iterate on the pool
  for (unsigned i = 0 ; i < hits.size() ; i++ ) {
  
	 //Major 1 
	 	if ( PositionAdjacent( Queue1.at(0).GetPosition().XYvector(), hits.at(i).GetPosition().XYvector() ) ) { // position test Major-1, 	if yes proceed for time 
	 		if ( TimeAdjacent( Queue1.at(0).GetTime(), hits.at(i).GetTime()) ) { // time test Major-1
		 		
		 		EnergyQ1 = EnergyQ1 + hits.at(i).GetEnergy();
		 		decision = 1; 
		 		}
	 		}
	//Major 2  	
	 	if ( PositionAdjacent( Queue2.at(0).GetPosition().XYvector(), hits.at(i).GetPosition().XYvector() ) ) { // position test Major-2, 	if yes proceed for time 
	 		if ( TimeAdjacent( Queue2.at(0).GetTime(), hits.at(i).GetTime()) ) { // time test Major-2
	 			
	 			EnergyQ2 = EnergyQ2 + hits.at(i).GetEnergy();
	 			if (decision == 1)  fConflict = 1 ;  // if decision vector is == 1, Flag a conflict
		 		else decision = 2 ;     
		 		}
	 		}

	// To solve the conflict give the debated energy to the lower energy Q  // N.B. It is physically impossible to have a two conflicts in 6 segments detector 		
		if (SolveOnce && fConflict) {
				SolveOnce = false; 
				if(EnergyQ1 < EnergyQ2) decision = 1 ;
				else decision = 2 ;
				}
		  
	//fill up the energy according to decision  	
		if (decision == 1 ) Queue1.push_back(hits.at(i)) ;
			else if (decision == 2 ) Queue2.push_back(hits.at(i)) ;
			//else this segment is discarded
			
			//Reset decision
			decision = 0;		
			}
 	
//  add up the energies  
TMielHit Major1 = AddHits(Queue1) ; 
TMielHit Major2 = AddHits(Queue2) ; 

 	  	
// calculate the new energy ratio
fPairRatioE1E2 = TMath::Min(Major2.GetEnergy(),Major1.GetEnergy()) / TMath::Max(Major2.GetEnergy(),Major1.GetEnergy()) ; // min/max

   	
 // Fill fPair   
 vector<TMielHit> hitvec ; 
 hitvec.push_back(Major1) ;
 hitvec.push_back(Major2) ;
 fPair = hitvec ;	

 return ; 	   
 
 }
 
/*
void TMielEvent::PairSumHits(){  // Calorimeter mode for pairs, sums all the hits from fPair and returns a singleton vector

cout << " PairSumHits : Hello  " << endl ;
getchar() ; 

	fPairSum = AddHits(fPair) ;  
	}
*/
 
bool TMielEvent::PositionAdjacent(const TVector2& A, const TVector2& B){ // reduced to polar coordinates only

	TVector2 Sum = A + B ; 

	 if (Sum.Mod() > 1.5*A.Mod() )
	 	return true ;
	 else 
	 	return false;
	
}


bool TMielEvent::TimeAdjacent(float timeA, float timeB){
	 
	 if ( TMath::Abs(timeA - timeB) <  gTimeThreshold ) {
	 return true ;
	 }
	 else  return false;	
}



//void TMielEvent::BuildAddBack(){
//BuildAddBack(true /*cluster*/, false /*pair*/, false /*pairsum*/); //default 
//}

	
void TMielEvent::BuildAddBack(bool cluster, bool pair, bool pairsum){
	
	//order is strict 
	SumHits() ;  // always true 
	 				
	if (cluster) 	{ ClusterHits(); };
	if (pair) 		{ PairHits() ; }
	
	//other Add-back schemes CALLS goes here 
	//if (pairSum) PairSumHits() ; 
	//...
}
	
	
void TMielEvent::SetMielData(TMielData* data){ // Take the elecron data from TMielData and store in a the TMielEvent of this class for further procedure 

	TMielHit aMielHit ; 
	unsigned mult = data->GetMultiplicity() ;
	for (unsigned i = 0 ; i < mult ; i++ ) {
		
		unsigned segment = data->GetSegment(i) + 1 ; //DCP segments{0,1,..5} => Analysed segments{1,2,..6} 
		double    energy = data->GetEnergy(i);
		int       time   = (int) data->GetTime(i);
		
		aMielHit.Set(segment, energy, time) ;		
		PushHit(aMielHit); // store it in fHits
		aMielHit.Clear();
	}
}





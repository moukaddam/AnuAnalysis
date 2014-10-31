#ifndef __MielEVENT__
#define __MielEVENT__


//c++
#include <vector>
using namespace std;

//Root
#include "TObject.h"
#include "TVector3.h"
#include "TMath.h"

//User
#include "TMielHit.h"
#include "TMielData.h"


//time threshold 
const float gTimeThreshold = 100 ; // experimental value 


class TMielEvent : public TObject {

	private:
		
	//Singles
	vector<TMielHit>	fHits;  //Singles
	
	//Add-back schemes
	TMielHit			fSum; 		// sum of all segments (generates a multiplicity 1 )
	//TMielHit			fPairSum; 	// sum of all segments from pair mode (generates a multiplicity 1 )
	
	vector<TMielHit>	fCluster; 	// cluster mode (generates a multiplicity 1, 2 and 3 )
	vector<TMielHit>	fPair; 		// pair mode (generates a multiplicity of 1 or 2 )    N.B : This mode discard some hits on purpose


	//flags 
	int 				fPattern;
	float               fRatioE1E2;     // Ratio of energy between the segments having the highest energy if it's a "real" pair it should go from ~1 to ~0.5
	int                 fConflict; 		// equal to 1 when a segement can be added to the two major segments, to solve the conflict, add to the lower in energy
	float               fPairRatioE1E2; // Same as fRatioE1E2 but made for fPair

	public:
// C O N S T R U C T O R / D E S T R U C T O R
	TMielEvent();
	~TMielEvent();

// F U N C T I O N  M E M B E R S
	void		Clear();
	void		Print();
	bool    	TimeAdjacent(float timeA, float timeB); // test if A an B are neighbouring segments 
	bool    	PositionAdjacent(const TVector2& A, const TVector2& B); // test if A an B are neighbouring segments
	void    	CalculatePattern() ;
	void		BuildAddBack();	
	
	TMielHit    AddHits(const vector<TMielHit>& ); // used in add-backs
	
	//Add-Back schemes
	void SumHits() ; 		// Calorimeter mode, sums all the hits, it will fill vector fSum
	void ClusterHits() ; 	// Cluster mode, only those hits with touching segments, it will fill vector fCluster
	void PairHits() ; 		// Pair mode, hits are summed only if they pass the Adjacent position/time tests, all other are discraded
	//void PairSumHits() ;  	// Calorimeter mode using the fPair vector
	
// S E T T E R S (We pass pointers )
	void SetMielData(TMielData* data); 
	void SetHits(vector<TMielHit>* vec) ;	
	void PushHit(TMielHit)	;

// G E T T E R S (we get objects)
	int 	GetPattern() 						{	return fPattern	; 	} 	
	inline TMielHit GetHit(UShort_t i )			{	return fHits.at(i);	}
	inline vector<TMielHit> GetHits()			{	return fHits ;		} 	
	inline TMielHit GetCluster()				{	return fSum ;		} 			
	inline vector<TMielHit> GetSum()			{	return fCluster ;	} 	


	ClassDef(TMielEvent,2)  // MielHit structure
};

#endif

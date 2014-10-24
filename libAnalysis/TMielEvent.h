#ifndef __MielEVENT__
#define __MielEVENT__

//c++
#include <vector>
using namespace std;
//Root
#include "TObject.h"
#include "TVector3.h"
//User
#include "TMielHit.h"
#include "TMielData.h"

class TMielEvent : public TObject {

	private:
		
	//flags 
	int fPattern;
	
	//Singles
	vector<TMielHit>	fHits; 
	
	//Add-back schemes
	TMielHit			fSum; // calorimeter mode sum of all segments (generates a multiplicity 1 )
	vector<TMielHit>	fCluster; //cluster mode (generates a multiplicity 1, 2 and 3 )
		
	public:
// C O N S T R U C T O R / D E S T R U C T O R
	TMielEvent();
	~TMielEvent();

// F U N C T I O N  M E M B E R S
	void		Clear();
	void		Print(); 
	bool    	Contact(const TMielHit &A, const TMielHit &B); // test if A an B are neighbouring segments
	void    	CalculatePattern() ;
	void		BuildAddBack();	
	
	TMielHit    Add(const vector<TMielHit>& ); // used in add-backs
	//Add-Back schemes
	void SumHits() ; // Calorimeter mode, sums all the hits, it will fill vector fSum
	void ClusterHits() ; // Cluster mode, only those hits with touching segments, it will fill vector fCluster
	
		
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


	ClassDef(TMielEvent,1)  // MielHit structure
};

#endif

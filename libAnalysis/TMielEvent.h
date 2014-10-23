#ifndef __MielEVENT__
#define __MielEVENT__

#include <vector>
#include "TObject.h"
#include "TMielHit.h"
#include "TVector3.h"

class TMielEvent : public TObject {

	private:
	std::vector<TMielHit*>	fHits; //singles
	//flags 
	UShort_t fPattern;

	public:
// C O N S T R U C T O R / D E S T R U C T O R
	TMielEvent();
	~TMielEvent();

// F U N C T I O N  M E M B E R S
	void	Clear();
	void	Print(); 
	bool    Contact(TMielHit* A, TMielHit* B); // test if A an B are neighbouring segments
	TMielHit*    Add(std::vector<TMielHit*>);
	
// S E T T E R S
	inline void SetHits(std::vector<TMielHit*> vec)	{	fHits = vec ; GetPattern() ;} 
	inline void PushHit(TMielHit*)					{	fHits.push_back(TMielHit*);	GetPattern() ; } 

// G E T T E R S
	unsigned short GetPattern() ; 	
	inline TMielHit* GetHit(UShort_t i )		{	return fHits.at(i);	}
	inline std::vector<TMielHit*> GetHits()		{	return fHits ;	} 	
	
	//Add-Back schemes 
	std::vector<TMielHit*> SumHits() ; // Calorimeter mode, sums all the hits
	std::vector<TMielHit*> ClusterHits() ; // Cluster mode, only those hits with touching segments

	ClassDef(TMielEvent,1)  // MielHit structure
};

#endif

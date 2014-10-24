#ifndef __MielEVENT__
#define __MielEVENT__

//c++
#include <vector>
//Root
#include "TObject.h"
#include "TVector3.h"
//User
#include "TMielHit.h"

class TMielEvent : public TObject {

	private:
	std::vector<TMielHit*>	fHits; //singles
	int fPattern;	           //flag

	public:
// C O N S T R U C T O R / D E S T R U C T O R
	TMielEvent();
	~TMielEvent();

// F U N C T I O N  M E M B E R S
	void	Clear();
	void	Print(); 
	bool    Contact(TMielHit* A, TMielHit* B); // test if A an B are neighbouring segments
	TMielHit*    Add(std::vector<TMielHit*>);
	void   CalculatePattern() ;
// S E T T E R S
	void SetHits(std::vector<TMielHit*> vec) ;	
	void PushHit(TMielHit*)	;

// G E T T E R S
	int GetPattern() 							{	return fPattern	; } 	
	inline TMielHit* GetHit(UShort_t i )		{	return fHits.at(i);	}
	inline std::vector<TMielHit*> GetHits()		{	return fHits ;	} 	
	
	//Add-Back schemes 
	std::vector<TMielHit*> SumHits() ; // Calorimeter mode, sums all the hits
	std::vector<TMielHit*> ClusterHits() ; // Cluster mode, only those hits with touching segments

	ClassDef(TMielEvent,1)  // MielHit structure
};

#endif

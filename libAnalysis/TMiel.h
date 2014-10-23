#ifndef __MIEL__
#define __MIEL__

//c++
#include <vector>
//Root
#include "TObject.h"
#include "TVector3.h"
//User
#include "TMielHit.h"
#include "TMielData.h"
#include "TMielEvent.h"

class TMiel : public TObject {

	private:
	TMielEvent	fMielEvent; //Already contains singles and hit pattern
	std::vector<TMielHit*>	fSum; //sum, calorimeter mode
	std::vector<TMielHit*>	fCluster; //cluster mode


	public:
// C O N S T R U C T O R / D E S T R U C T O R
	TMiel();
	~TMiel();

// F U N C T I O N  M E M B E R S
	void	Clear();
	void	Print(); 
	void	BuildHits();
	
// S E T T E R S
	void SetMielEvent(TMielData* data); 

// G E T T E R S



	ClassDef(TMiel,1)  // MielHit structure
};

#endif

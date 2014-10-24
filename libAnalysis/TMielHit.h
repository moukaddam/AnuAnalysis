#ifndef __MielHIT__
#define __MielHIT__


#include "TObject.h"
#include "TVector3.h"
#include "TMath.h"

const int    nb_segment = 6.0 ; 
const double phi_pitch = (360.0/nb_segment)*TMath::DegToRad() ; 
const double mid_radius = 1.0 ;  // need correct value
const double distance_to_target = 15.0 ; // need correct value

class TMielHit : public TObject {

	private:
	UShort_t	fSegment;
	Double_t	fEnergy;
	Int_t		fTime; 
	TVector3	fPosition; 

	void CalculatePosition() ;
		
	public:
// C O N S T R U C T O R / D E S T R U C T O R
	TMielHit();
	TMielHit(UShort_t Seg, Double_t Energy, UInt_t Time);
	~TMielHit();

// F U N C T I O N  M E M B E R S
	void	Clear(); 
	void	Print(); 
	TMielHit* Copy(TMielHit* hit) ; 
	
// S E T T E R S
	       void SetSegment(UShort_t Seg) ; 	
	inline void SetEnergy(Double_t Energy)	{	fEnergy=(Energy);	}		
	inline void SetTime(Int_t Time)			{	fTime=(Time);		}		
	inline void SetPosition(TVector3 pos)	{	fPosition=(pos);	}	
	
	// Set segment, energy and time Simultaneously	
	inline void Set(UShort_t Seg, Double_t Energy, UInt_t Time)	{
				SetSegment(Seg); 
				SetEnergy(Energy);	
				SetTime(Time);		
				}
		
	public:
// G E T T E R S
	inline UShort_t GetSegment()	{	return fSegment;	}	
	inline Double_t GetEnergy()		{	return fEnergy;		}
	inline Double_t GetTime()		{	return fTime;		}
	inline TVector3 GetPosition()	{	return fPosition;	}		
	

	ClassDef(TMielHit,1)  // MielHit structure
};

#endif

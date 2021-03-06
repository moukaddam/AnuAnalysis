#ifndef __MielDATA__
#define __MielDATA__


#include <vector>
#include "TObject.h"


class TMielData : public TObject {

	private:
	//multi-fold variables
	std::vector<UShort_t>	fMielSegment;
	std::vector<Int_t>		fMielCharge;
	std::vector<Double_t>	fMielEnergy;
	std::vector<Double_t> 	fMielBRho;
	std::vector<Double_t>	fMielTime; 

	//recalculated variables 
	Double_t		fMielAptherix; 
	Double_t		fMielHallProbe; 
	Double_t		fMielVcontE; 
	
	//copied values 
	Int_t		fMielVcontG; 
	Int_t   	fMielChopper;

	public:
// C O N S T R U C T O R / D E S T R U C T O R
	TMielData();
	~TMielData();

// F U N C T I O N  M E M B E R S
	void	Clear(); 
	void	Print(); 

// S E T T E R S
	inline void SetSegment(UShort_t Seg)		{	fMielSegment.push_back(Seg);	} 
	inline void SetCharge(Int_t Charge)			{	fMielCharge.push_back(Charge);	} 	
	inline void SetEnergy(Double_t Energy)		{	fMielEnergy.push_back(Energy);	}		
    inline void SetBRho(Double_t BRho)        	{   fMielBRho.push_back(BRho);    	}
	inline void SetTime(Double_t Time)	    	{	fMielTime.push_back(Time);		}		

	inline void SetAptherix(Double_t Aptherix)		{	fMielAptherix=(Aptherix);	} 
	inline void SetHallProbe(Double_t HallProbe)	{	fMielHallProbe=(HallProbe);	} 	
	inline void SetVcontE(Double_t VcontE)			{	fMielVcontE=(VcontE);		}
			
	inline void SetVContG(Int_t VcontG)			{	fMielVcontG=(VcontG);		}	
    inline void SetChopper(Int_t Chopper) 		{ 	fMielChopper=(Chopper); 	}

	// Set all Simultaneously	
	inline void SetMiel(UShort_t Seg, UInt_t Charge, Double_t Energy, Double_t BRho, Double_t Time)	{
				SetSegment(Seg); 
				SetCharge(Charge);	
				SetEnergy(Energy);	
        		SetBRho(BRho);
				SetTime(Time);		
	}

// G E T T E R S
	inline UInt_t	GetMultiplicity()		{	return fMielSegment.size();	}
	
	inline UShort_t GetSegment(UInt_t i)	{	return fMielSegment.at(i);	}	
	inline UInt_t 	GetCharge(UInt_t i)		{	return fMielCharge.at(i);	}		
	inline Double_t GetEnergy(UInt_t i)		{	return fMielEnergy.at(i);	}
    inline Double_t GetBRho(UInt_t i)   	{ 	return fMielBRho.at(i); 	}
	inline Double_t GetTime(UInt_t i)		{	return fMielTime.at(i);		}
		
	inline Double_t GetAptherix()			{	return fMielAptherix;	}	
	inline Double_t GetHallProbe()			{	return fMielHallProbe;	}		
	inline Double_t GetVcontE()				{	return fMielVcontE;		}
	
	inline UShort_t GetVcontG()				{	return fMielVcontG;		}
  	inline UShort_t GetChopper()  			{ 	return fMielChopper; 	}
	

	ClassDef(TMielData,1)  // MielData structure
};

#endif

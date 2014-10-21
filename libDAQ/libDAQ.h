#ifndef LIBDAQ_H
#define LIBDAQ_H

#include "TObjArray.h"
#include "TTree.h"
#include "TBranch.h"

class TDAQADC : public TObject
{
  private:
  protected:
    int FInterface, FADCNumber;
    char *FFieldName, *FFieldDesc;
    int FNumChannels;
  public:
    TDAQADC(void);
    TDAQADC(const TDAQADC &src);
    TDAQADC(int Interface, int ADCNumber, const char *FieldName, const char *FieldDesc, int NumChannels);
    ~TDAQADC(void);
    
    TDAQADC &operator=(const TDAQADC &src);

    int GetInterface(void) const;
    int GetADCNumber(void) const;
    const char *GetFieldName(void) const;
    const char *GetFieldDesc(void) const;
    int GetNumChannels(void) const;

    void SetInterface(int Interface);
    void SetADCNumber(int ADCNumber);
    void SetFieldName(const char *FieldName);
    void SetFieldDesc(const char *FieldDesc);
    void SetNumChannels(int NumChannels);
    
    ClassDef(TDAQADC,0)
};

class TDAQBranch : public TObject
{
  private:
  protected:
    TObjArray FADCs;
    char *FBranchName, *FBranchDesc;
  public:
    TDAQBranch(void);
    TDAQBranch(const char *BranchName, const char *BranchDesc);
    TDAQBranch(const TDAQBranch &src);
    ~TDAQBranch(void);
    
    TDAQBranch &operator=(const TDAQBranch &src);
    
    TObjArray *GetADCs(void);
    const char *GetBranchName(void) const;
    const char *GetBranchDesc(void) const;

    void SetBranchName(const char *BranchName);
    void SetBranchDesc(const char *BranchDesc);
    
    TBranch *InitTree(TTree *Tree);

    ClassDef(TDAQBranch,0)
};

class TDAQConfig : public TObject
{
  private:
  protected:
    TObjArray FBranches;
  public:
    TDAQConfig(void);
    TDAQConfig(const TDAQConfig &src);
    ~TDAQConfig(void);

    TDAQConfig &operator=(const TDAQConfig &src);
    
    TObjArray *GetBranches(void);
    void InitTree(TTree *Tree);

    ClassDef(TDAQConfig,0)
};

class TDCPReader
{
  private:
  protected:
    TDAQConfig *FDAQConfig;
  public:
    TDCPReader(TDAQConfig *DAQConfig);
    void FillTreeFromDCPFile(TTree *Tree, const char *DCPFilename);
};

#endif

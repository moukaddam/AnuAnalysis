#include <stdio.h>
//#include <stdint.h>
#include "libDAQ.h"
#include "TIterator.h"
// TK (Jul-2013)
#include <stdint.h>
// end TK

//Mhd (21-May 2014) 
 #include <iostream>
 #include <assert.h>
 #include <limits.h>
 #include <alloca.h>
 #include <cstdlib>

// #define DBG_NOISY
#define RUN_SILENT

// -----------------------------------------------------------------------------
// TDAQADC - Ecapsulates the data for a particular ADC.
// -----------------------------------------------------------------------------

inline char *strdup_safe(const char *s)
{
  if (s == NULL)
    return NULL;
  else
    return strdup(s);
}

TDAQADC::TDAQADC(void)
{
  FInterface = -1;
  FADCNumber = -1;
  FFieldName = NULL;
  FFieldDesc = NULL;
  FNumChannels = 0;
}

TDAQADC::TDAQADC(const TDAQADC &src):TObject(src)
{
  FInterface = src.FInterface;
  FADCNumber = src.FADCNumber;
  FFieldName = strdup_safe(src.FFieldName);
  FFieldDesc = strdup_safe(src.FFieldDesc);
  FNumChannels = src.FNumChannels;
}

TDAQADC::TDAQADC(int Interface, int ADCNumber, const char *FieldName, const char *FieldDesc, int NumChannels)
{
  FInterface = Interface;
  FADCNumber = ADCNumber;
  FFieldName = strdup_safe(FieldName);
  FFieldDesc = strdup_safe(FieldDesc);
  FNumChannels = NumChannels;
}

TDAQADC::~TDAQADC(void)
{
  SetFieldName(NULL);
  SetFieldDesc(NULL);
}

TDAQADC &TDAQADC::operator=(const TDAQADC &src)
{
  if (this != &src)
  {
    SetInterface(src.FInterface);
    SetADCNumber(src.FADCNumber);
    SetFieldName(src.FFieldName);
    SetFieldDesc(src.FFieldDesc);
    SetNumChannels(src.FNumChannels);
  }
  return *this;
}

int TDAQADC::GetInterface(void) const
{
  return FInterface;
}

int TDAQADC::GetADCNumber(void) const
{
  return FADCNumber;
}

const char *TDAQADC::GetFieldName(void) const
{
  return FFieldName;
}

const char *TDAQADC::GetFieldDesc(void) const
{
  return FFieldDesc;
}

int TDAQADC::GetNumChannels(void) const
{
  return FNumChannels;
}

void TDAQADC::SetInterface(int Interface)
{
  FInterface = Interface;
}

void TDAQADC::SetADCNumber(int ADCNumber)
{
  FADCNumber = ADCNumber;
}

void TDAQADC::SetFieldName(const char *FieldName)
{
  if (FFieldName != NULL) free(FFieldName);
  FFieldName = strdup_safe(FieldName);
}

void TDAQADC::SetFieldDesc(const char *FieldDesc)
{
  if (FFieldDesc != NULL) free(FFieldDesc);
  FFieldDesc = strdup_safe(FieldDesc);
}

void TDAQADC::SetNumChannels(int NumChannels)
{
  FNumChannels = NumChannels;
}

// -----------------------------------------------------------------------------
// TDAQBranch - Groups the ADCs into branches.
// -----------------------------------------------------------------------------

TDAQBranch::TDAQBranch(void)
{
  FADCs.SetOwner();
  FBranchName = NULL;
  FBranchDesc = NULL;
}

TDAQBranch::TDAQBranch(const char *BranchName, const char *BranchDesc)
{
  FADCs.SetOwner();
  FBranchName = strdup_safe(BranchName);
  FBranchDesc = strdup_safe(BranchDesc);
}

TDAQBranch::TDAQBranch(const TDAQBranch &src):TObject(src)
{
  FADCs.SetOwner();
  FBranchName = strdup_safe(src.FBranchName);
  FBranchDesc = strdup_safe(src.FBranchDesc);

  // Deep-copy all the ADCs.
  TIterator *ADCIter = src.FADCs.MakeIterator();
  while(TDAQADC *ADC = (TDAQADC *)(ADCIter->Next()))
    FADCs.Add(new TDAQADC(*ADC));
}

TDAQBranch::~TDAQBranch(void)
{
  SetBranchName(NULL);
  SetBranchDesc(NULL);
}

TDAQBranch &TDAQBranch::operator=(const TDAQBranch &src)
{
  if (this != &src)
  {
    SetBranchName(src.GetBranchName());
    SetBranchDesc(src.GetBranchDesc());
    
    // Clear out any existing ADCs.
    FADCs.Clear();
    
    // Deep-copy all the ADCs.
    TIterator *ADCIter = src.FADCs.MakeIterator();
    while(TDAQADC *ADC = (TDAQADC *)(ADCIter->Next()))
      FADCs.Add(new TDAQADC(*ADC));
  }
  return *this;
}

TObjArray *TDAQBranch::GetADCs(void)
{
  return &FADCs;
}

const char *TDAQBranch::GetBranchName(void) const
{
  return FBranchName;
}

const char *TDAQBranch::GetBranchDesc(void) const
{
  return FBranchDesc;
}

void TDAQBranch::SetBranchName(const char *BranchName)
{
  if (FBranchName != NULL) free(FBranchName);
  FBranchName = strdup_safe(BranchName);
}

void TDAQBranch::SetBranchDesc(const char *BranchDesc)
{
  if (FBranchDesc != NULL) free(FBranchDesc);
  FBranchDesc = strdup_safe(BranchDesc);
}

TBranch *TDAQBranch::InitTree(TTree *Tree)
{
  TIterator *ADCIter = GetADCs()->MakeIterator();

  // Each leaf uses space strlen(FieldName) plus space for the seperator (the
  // trailing '/' for the first leaf, the prepended ':' for subsequent leaves.
  int LeafListLen = 0;
  while(TDAQADC *ADC = (TDAQADC *)(ADCIter->Next()))
    LeafListLen += strlen(ADC->GetFieldName()) + 1;
  LeafListLen += 2; // for the size character 's' and the null terminator.
  
  // Build up the leaf string.
  char *LeafList = new char[LeafListLen];
  LeafList[0] = '\0';
  ADCIter->Reset();
  while(TDAQADC *ADC = (TDAQADC *)(ADCIter->Next()))
  {
    if (LeafList[0] == '\0')
    {
      strcat(LeafList, ADC->GetFieldName());
      strcat(LeafList, "/s");
    }
    else
    {
      strcat(LeafList, ":");
      strcat(LeafList, ADC->GetFieldName());
    }
  }
  
  // Create the branch.
#ifdef DBG_NOISY
  printf("Tree->Branch(\"%s\", NULL, \"%s\");\n", GetBranchName(), LeafList);
#endif
  
  TBranch *RetVal = Tree->Branch(GetBranchName(), 0, (const char *)LeafList); // MHD : 25 October 2014
  //  Changes : Added 32000, casting of leaflist, Replace branch address "NULL" by zero

  // Done!
  delete[] LeafList;
  delete ADCIter;
  return RetVal;
}

// -----------------------------------------------------------------------------
// TDAQConfig - Contains the configuration of the DAQ system.
// -----------------------------------------------------------------------------

TDAQConfig::TDAQConfig(void)
{
  FBranches.SetOwner();
}

TDAQConfig::TDAQConfig(const TDAQConfig &src):TObject(src)
{
  FBranches.SetOwner();
  
  // Deep-copy all the branches.
  TIterator *BranchIter = src.FBranches.MakeIterator();
  while(TDAQBranch *Branch = (TDAQBranch *)(BranchIter->Next()))
    FBranches.Add(new TDAQBranch(*Branch));
}

TDAQConfig::~TDAQConfig(void)
{
  // Nothing to do.
}

TDAQConfig &TDAQConfig::operator=(const TDAQConfig &src)
{
  if (this != &src)
  {
    // Clear out any existing branches.
    FBranches.Clear();
    
    // Deep-copy all the branches.
    TIterator *BranchIter = src.FBranches.MakeIterator();
    while(TDAQBranch *Branch = (TDAQBranch *)(BranchIter->Next()))
      FBranches.Add(new TDAQBranch(*Branch));
  }
  return *this;
}

TObjArray *TDAQConfig::GetBranches(void)
{
  return &FBranches;
}

void TDAQConfig::InitTree(TTree *Tree)
{
  TIterator *BranchIter = GetBranches()->MakeIterator();

  while(TDAQBranch *Branch = (TDAQBranch *)(BranchIter->Next()))
    Branch->InitTree(Tree);
}

// -----------------------------------------------------------------------------
// TDCPReader - Imports the contents of a DCP file into the tree.
// -----------------------------------------------------------------------------

TDCPReader::TDCPReader(TDAQConfig *DAQConfig)
{
  FDAQConfig = DAQConfig;
}

void TDCPReader::FillTreeFromDCPFile(TTree *Tree, const char *DCPFilename)
{
  // Step 1: Open the file.
  FILE *InFile = fopen(DCPFilename, "rb");

  if (InFile == NULL)
  {
    printf("Could not open file \"%s\".\n", DCPFilename);
    return;
  }
  fseek(InFile, 0, SEEK_END);
  long InFileSize = ftell(InFile);
  long NumBuffers = InFileSize / 8192;
  fseek(InFile, 0, SEEK_SET);

  // Step 2: Allocate memory, etc
  int NumSpurious[16][16];
  int NumInvalid[16][16];
  int NumChannels[16][16];
  int ADCDataOffsets[16][16];
  uint16_t RawData[16*16];
  uint16_t EventBuffer[4096];

  memset(NumSpurious, 0, sizeof(NumSpurious));
  memset(NumInvalid, 0, sizeof(NumInvalid));
  memset(NumChannels, 0, sizeof(NumChannels));
  memset(ADCDataOffsets, 0, sizeof(ADCDataOffsets));

  // Disable everything in the tree.
  Tree->SetBranchStatus("*", 0);
  
  // Step 3: Fill the interface/ADC array. Also, note where to move data to in
  // the raw data array.
  int SingleInterface = -1;
  int RawDataOffset = 0;

  TIterator *BranchIter = FDAQConfig->GetBranches()->MakeIterator();

  while(TDAQBranch *Branch = (TDAQBranch *)(BranchIter->Next()))
  {
    // Set the pointer for this branch.
    TBranch *ROOTBranch = Tree->GetBranch(Branch->GetBranchName());
    if (ROOTBranch == NULL)
    {
      printf("Branch \"%s\" does not exist in the tree, skiping.\n", Branch->GetBranchName());
      continue;
    }
    ROOTBranch->ResetBit(kDoNotProcess);
    ROOTBranch->SetAddress(&(RawData[RawDataOffset]));

#ifdef DBG_NOISY
    printf("Branch \"%s\" raw data offset = %d\n", Branch->GetBranchName(), RawDataOffset);
#endif

    // Process each ADC
    TIterator *ADCIter = Branch->GetADCs()->MakeIterator();

    while(TDAQADC *ADC = (TDAQADC *)(ADCIter->Next()))
    {
      // See if this interface/ADC position has already been assigned.
      int IntfNum = ADC->GetInterface();
      int ADCNum = ADC->GetADCNumber();
      if (NumChannels[IntfNum][ADCNum] != 0)
        printf("Interface %d, ADC %d already assigned. Skipping assignment of \"%s\" to this ADC.\n",
          IntfNum, ADCNum, ADC->GetFieldName());
      else
      {
        // Note where the raw data should go.
        ADCDataOffsets[IntfNum][ADCNum] = RawDataOffset;
#ifdef DBG_NOISY
        printf("  ADC \"%s\" (%d:%d) raw data offset = %d\n", ADC->GetFieldName(), IntfNum, ADCNum, RawDataOffset);
#endif
        // Note the number of channels for this ADC.
        NumChannels[IntfNum][ADCNum] = ADC->GetNumChannels();
        // See if we're operating on a single interface.
        if (SingleInterface != IntfNum)
        {
          if (SingleInterface == -1)
            SingleInterface = IntfNum;
          else
            SingleInterface = -2;
        }
      }
      RawDataOffset++;
    }
  }

  // Step 4: Process all the buffers.
  char BarString[23] = "[                    ]";
  long BuffersDone = 0;
  long EventsDone = 0;

  int NumChars = printf("Processing events ... %s (%ld events added)", BarString, EventsDone);
  fflush(stdout);
  for (BuffersDone = 0; BuffersDone < NumBuffers; BuffersDone++)
  {
    // Read the buffer
    if (fread(EventBuffer, sizeof(EventBuffer), 1, InFile) != 1)
    {
      printf("\nFailed to read buffer %ld!\n", BuffersDone);
      break;
    }

    // Process all the events in the buffer
    int NumEvents = EventBuffer[13];
    int CurBufPos = 16;

    for (int EventLoop = 0; EventLoop < NumEvents; EventLoop++)
    {
      // Zero out the raw data.
      memset(RawData, 0, RawDataOffset * sizeof(RawData[0]));

      // Get the interface mask for this event.
      uint16_t InterfaceMask;
      if (SingleInterface >= 0)
        InterfaceMask = 1 << SingleInterface;
      else
        InterfaceMask = EventBuffer[CurBufPos++];

      // Loop over all the interfaces.
      for (int InterfaceLoop = 0; InterfaceLoop < 16; InterfaceLoop++)
      {
        if ((InterfaceMask & 1) != 0)
        {
          // Get the ADC mask for this interface for this event.
          uint16_t ADCMask = EventBuffer[CurBufPos++];

          // Loop over all the ADCs.
          for (int ADCLoop = 0; ADCLoop < 16; ADCLoop++)
          {
            if ((ADCMask & 1) != 0)
            {
              uint16_t ADCData = EventBuffer[CurBufPos++];

              // Filter off and log any spurious or invalid values.
              if (NumChannels[InterfaceLoop][ADCLoop] == 0)
              {
                // Spurious event.
                NumSpurious[InterfaceLoop][ADCLoop]++;
              }
              else if (ADCData >= NumChannels[InterfaceLoop][ADCLoop])
              {
                // Invalid event.
                NumInvalid[InterfaceLoop][ADCLoop]++;
              }
              else
              {
                // Valid event. Set the corresponding value in the array.
                RawData[ADCDataOffsets[InterfaceLoop][ADCLoop]] = ADCData;
              }
            }
            ADCMask = ADCMask >> 1;
          }
        }
        InterfaceMask = InterfaceMask >> 1;
      }

      // Add this event to the tree.
      Tree->Fill();
    }

    EventsDone += NumEvents;

    // Update the progress bar.
    if (((BuffersDone + 1) % 16) == 0)
    {
      int OldProgBar = 20 * (BuffersDone - 15) / NumBuffers;
      int NewProgBar = 20 * (BuffersDone + 1) / NumBuffers;
      for (int FillLoop = OldProgBar; FillLoop < NewProgBar; FillLoop++)
        BarString[FillLoop+1] = '=';
      NumChars = printf("\rProcessing events ... %s (%ld events added)", BarString, EventsDone);
      fflush(stdout);
    }
  }

  // All done! Tidy up and return. Also, show any spurious/invalid events.
#ifdef RUN_SILENT
  char *BlankLine = new char[NumChars + 1];
  memset(BlankLine, ' ', NumChars);
  BlankLine[NumChars] = '\0';
  printf("\r%s\r", BlankLine);
  delete[] BlankLine;
#else
  printf("\rProcessing events ... completed (%ld buffers processed, %ld events added)\n",
    BuffersDone, EventsDone);
#endif
  fflush(stdout);
  fclose(InFile);

  // Show any invalid or spurious events
  for (int InterfaceLoop = 0; InterfaceLoop < 16; InterfaceLoop++)
  {
    for (int ADCLoop = 0; ADCLoop < 16; ADCLoop++)
    {
      if ((NumSpurious[InterfaceLoop][ADCLoop] > 0) || (NumInvalid[InterfaceLoop][ADCLoop] > 0))
      {
        printf("Interface %2d, ADC %2d: %d spurious, %d invalid\n",
          InterfaceLoop, ADCLoop, NumSpurious[InterfaceLoop][ADCLoop],
          NumInvalid[InterfaceLoop][ADCLoop]);
      }
    }
  }
}

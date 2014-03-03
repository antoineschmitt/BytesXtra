#ifndef INITGUID        /* INITGUID causes the actual GUIDs to be declared, */ 
	#define INITGUID      /* not just references to them. Must be declared in */
#endif                  /* only one file. */          

#include "cregster.h"
#include "dversion.h"
#include "xclassver.h"

BEGIN_DEFINE_CLASS_INTERFACE(CRegister, IMoaRegister) 
END_DEFINE_CLASS_INTERFACE


static char msgTable[] = { 
	"xtra BytesXtra\n" \
  "-- Version " VER_VERSION_STRING "\n"
	"* newBytes integer size -- creates a bytes object of given size\n"
	"* newBytesFromString string str -- converts a string (including trailing 0) to a bytes object \n"
	"-- bytes.size -- get and set the size\n"
	"-- bytes[int index] -- get and set value\n"
	"-- bytes.data[int index1..int index2] -- get and set values\n"
	"-- bytes.toString() -- creates a string from a bytes object (must be 0 terminated)\n"
	"-- bytes.offset(bytes2 [,int startIndex]) -- returns the first index (after startIndex if present, default 1) of bytes2 in aBytes, or 0 if not found\n"
	"-- bytes.copyFrom(bytes srcBytes, int bytesCount, int srcOffset, int dstOffset) -- copies bytesCount bytes from srcOffset in srcBytes into bytes starting at dstOffset\n"
};


/* ---------------------------------------------------- MoaCreate_CRegister */
STDMETHODIMP MoaCreate_CRegister(CRegister FAR * This)
{
  MoaError err = kMoaErr_NoErr;
  return(err);
}

/* --------------------------------------------------- MoaDestroy_CRegister */
STDMETHODIMP_(void) MoaDestroy_CRegister(CRegister FAR * This)
{
  return;
}

/* ---------------------------------- CRegister_IMoaRegister Create/Destroy */
CRegister_IMoaRegister::CRegister_IMoaRegister(MoaError FAR * pErr)
	{ *pErr = (kMoaErr_NoErr); }
CRegister_IMoaRegister::~CRegister_IMoaRegister() {}


/* ------------------------------------------- CRegister_IMoaRegister::Register */
STDMETHODIMP_(MoaError) CRegister_IMoaRegister::Register(PIMoaCache pCache, 
	PIMoaDict pXtraDict)
{

	/* variable declaration */
	MoaError 	err = kMoaErr_NoErr;
	PIMoaDict 	pRegDict;

	/* Register the scripting xtra */
	err = pCache->AddRegistryEntry(pXtraDict, &CLSID_CScript, &IID_IMoaMmXScript, &pRegDict);
			
	if ( err == kMoaErr_NoErr)
	{
		/* Register the method table */
		err = pRegDict->Put(kMoaMmDictType_MessageTable, msgTable, 0, kMoaMmDictKey_MessageTable);

		/* Mark this Xtra "Safe for Shockwave" */
		MoaBool bItsSafe = TRUE;
		err = pRegDict->Put(kMoaMmDictType_SafeForShockwave,
			&bItsSafe, 	sizeof( bItsSafe ), kMoaMmDictKey_SafeForShockwave);

	}

	return(err);
}

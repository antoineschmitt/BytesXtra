/* bytesValue.h

  Defines the 'bytes' datatype interface. See attached documentation for usage details.
  
  (c) Antoine Schmitt 2008
  www.schmittmachine.com
*/

#ifndef _bytesValueH_
#define _bytesValueH_

#include "moaxtra.h"
#include "mmixval.h"

/* --------------------------------------------------- */

// 020B4D2A-2987-4AFB-B050-13220C5FDFD6
DEFINE_GUID(CLSID_Bytes, 0x020B4D2A, 0x2987, 0x4AFB, 0xB0, 0x50, 0x13, 0x22, 0x0C, 0x5F, 0xDF, 0xD6);

EXTERN_BEGIN_DEFINE_CLASS_INSTANCE_VARS(Bytes)
	PIMoaMmValue pValue;
	unsigned char * pData;
	MoaLong pSize;
	MoaLong pAllocSize;
EXTERN_END_DEFINE_CLASS_INSTANCE_VARS


/*****************************************************************************
 * Xtra Moa XValue Interface - Class definition
 ****************************************************************************/

EXTERN_BEGIN_DEFINE_CLASS_INTERFACE(Bytes, IMoaMmXValue)
// ------------ INHERITED FUNCTIONS
		EXTERN_DEFINE_METHOD(MoaError, GetProp, (ConstPMoaMmValue selfRef,
										MoaMmSymbol propName,
										MoaLong	indexCount,
										ConstPMoaMmValue pIndexValues,
										PMoaMmValue	pResult))		
		EXTERN_DEFINE_METHOD(MoaError, SetProp, (MoaMmSymbol propName,
										MoaLong	indexCount,
										ConstPMoaMmValue pIndexValues,
										ConstPMoaMmValue pNewValue))		
		EXTERN_DEFINE_METHOD(MoaError, CallHandler, (PMoaMmCallInfo callPtr))		
		EXTERN_DEFINE_METHOD(MoaError, Ilk, (PMoaMmValue pArgument, PMoaMmValue	pResult))		
		EXTERN_DEFINE_METHOD(MoaError, StringRep, (PMoaMmValue pStringVal))		
		EXTERN_DEFINE_METHOD(MoaError, SymbolRep, (PMoaMmSymbol	pSymbol))		
		EXTERN_DEFINE_METHOD(MoaError, SetData, (PMoaVoid pDatum))	// Argument is the size	
		
// ------------ own methods - not private so it can be called by the lingo handler
		EXTERN_DEFINE_METHOD(unsigned char, getAt, (int index)) // index starting from 1 !
		EXTERN_DEFINE_METHOD(void, setAt, (int index, unsigned char val)) // index starting from 1 !
		EXTERN_DEFINE_METHOD(int, size, ()) // the size
		EXTERN_DEFINE_METHOD(void, copyDataTo, (void * buf, MoaLong bufSiz))
		EXTERN_DEFINE_METHOD(void, copyDataFrom, (void * buf, MoaLong bufSiz))
		EXTERN_DEFINE_METHOD(void, resizeTo, (MoaLong newSiz))
		EXTERN_DEFINE_METHOD(unsigned char *, data, ()) // pointer to the data, use with caution

// ------------ UNUSED FUNCTIONS
		EXTERN_DEFINE_METHOD(MoaError, GetPropCount, (MoaLong * pCount))		
		EXTERN_DEFINE_METHOD(MoaError, GetPropDescriptionByIndex, (MoaLong index, PMoaMmValueDescription pDescription))		
		EXTERN_DEFINE_METHOD(MoaError, GetCount, (MoaMmSymbol propName, MoaLong	* pCount))		
		EXTERN_DEFINE_METHOD(MoaError, AccessPropRef, (ConstPMoaMmValue	selfRef,
										MoaMmSymbol	propName,
										MoaLong	indexCount,
										ConstPMoaMmValue pIndexValues,
										PMoaMmValue	pResult))		
		EXTERN_DEFINE_METHOD(MoaError, GetContents, (ConstPMoaMmValue selfRef, PMoaMmValue	pResult))		
		EXTERN_DEFINE_METHOD(MoaError, SetContents, (PMoaMmValue pNewValue))		
		EXTERN_DEFINE_METHOD(MoaError, SetContentsBefore, (PMoaMmValue pNewValue))		
		EXTERN_DEFINE_METHOD(MoaError, SetContentsAfter, (PMoaMmValue pNewValue))		
		EXTERN_DEFINE_METHOD(MoaError, IntegerRep, (PMoaLong pIntVal))		
		EXTERN_DEFINE_METHOD(MoaError, StreamOut, (PIMoaStream2	pStream))		
		EXTERN_DEFINE_METHOD(MoaError, StreamIn, (PIMoaStream2 pStream))


EXTERN_END_DEFINE_CLASS_INTERFACE

#endif /* _bytesValueH_ */

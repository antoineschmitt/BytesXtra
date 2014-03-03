#ifndef _H_cscript
#define _H_cscript

#include "moaxtra.h"
#include "mmixscrp.h"

#include "bytesValue.h"

// 91829627-FB61-4B8A-B3C9-D7F3FA49D49C
DEFINE_GUID(CLSID(CScript), 0x91829627, 0xFB61, 0x4B8A, 0xB3, 0xC9, 0xD7, 0xF3, 0xFA, 0x49, 0xD4, 0x9C);

EXTERN_BEGIN_DEFINE_CLASS_INSTANCE_VARS(CScript)
	PIMoaMmValue				pMmValue;
	PIMoaMmXValueUtils fpXUtils;
EXTERN_END_DEFINE_CLASS_INSTANCE_VARS

EXTERN_BEGIN_DEFINE_CLASS_INTERFACE(CScript, IMoaMmXScript)
	//--- global fcts
	EXTERN_DEFINE_METHOD(MoaError, newBytes, (PMoaMmCallInfo))
	EXTERN_DEFINE_METHOD(MoaError, newBytesFromString, (PMoaMmCallInfo))

	EXTERN_DEFINE_METHOD(MoaError, Call, (PMoaMmCallInfo))		
EXTERN_END_DEFINE_CLASS_INTERFACE


enum 
{
m_newBytes = 0,
m_newBytesFromString,

m_XXXX
};

#endif /* _H_cscript */

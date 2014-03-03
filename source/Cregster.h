
#ifndef _H_cregster
#define _H_cregster

#include "moaxtra.h"
#include "cscript.h"

// A2A3700B-8A60-42E2-B698-7BD8143A4F38
DEFINE_GUID(CLSID(CRegister), 0xA2A3700B, 0x8A60, 0x42E2, 0xB6, 0x98, 0x7B, 0xD8, 0x14, 0x3A, 0x4F, 0x38);

EXTERN_BEGIN_DEFINE_CLASS_INSTANCE_VARS(CRegister) 
EXTERN_END_DEFINE_CLASS_INSTANCE_VARS

EXTERN_BEGIN_DEFINE_CLASS_INTERFACE(CRegister, IMoaRegister) 
	EXTERN_DEFINE_METHOD(MoaError, Register, (PIMoaCache, PIMoaDict))
EXTERN_END_DEFINE_CLASS_INTERFACE


#endif /* _H_cregster */

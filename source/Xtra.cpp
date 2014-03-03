#include "Cscript.h"
#include "Cregster.h"

#include "dversion.h"
#include "xclassver.h"

/*****************************************************************************
 *  XTRA DEFINITION
 *  ---------------
 *  The Xtra definition specfies the classes and interfaces defined by your
 *  MOA Xtra.
 *
 *  Syntax:
 *	BEGIN_XTRA_DEFINES_CLASS(<class-name>,<version>)
 *	CLASS_DEFINES_INTERFACE(<class-name>, <interface-name>,<version>) 
 ****************************************************************************/ 
BEGIN_XTRA
	BEGIN_XTRA_DEFINES_CLASS(CRegister, XTRA_CLASS_VERSION)
		CLASS_DEFINES_INTERFACE(CRegister, IMoaRegister, XTRA_CLASS_VERSION)
	END_XTRA_DEFINES_CLASS
	BEGIN_XTRA_DEFINES_CLASS(CScript, XTRA_CLASS_VERSION)
		CLASS_DEFINES_INTERFACE(CScript, IMoaMmXScript, XTRA_CLASS_VERSION)
	END_XTRA_DEFINES_CLASS
	BEGIN_XTRA_DEFINES_CLASS(Bytes, XTRA_CLASS_VERSION)
		CLASS_DEFINES_INTERFACE(Bytes, IMoaMmXValue, XTRA_CLASS_VERSION)
	END_XTRA_DEFINES_CLASS
END_XTRA


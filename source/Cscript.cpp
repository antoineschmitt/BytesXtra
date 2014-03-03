
#include "cscript.h"
#include "moatry.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "bytesValue.h"


BEGIN_DEFINE_CLASS_INTERFACE(CScript, IMoaMmXScript)
END_DEFINE_CLASS_INTERFACE

/* ----------------------------------- Interface Create/Destroy */
CScript_IMoaMmXScript::CScript_IMoaMmXScript(MoaError FAR * pErr)
	{ *pErr = (kMoaErr_NoErr); }
CScript_IMoaMmXScript::~CScript_IMoaMmXScript() {}



//**********************************
//***************** CREATE/DESTROY
//**********************************


STDMETHODIMP MoaCreate_CScript(CScript FAR * pObj) {
moa_try
	
	pObj->pCallback->QueryInterface(&IID_IMoaMmValue, (PPMoaVoid)&pObj->pMmValue);
	ThrowErr(pObj->pCallback->QueryInterface(&IID_IMoaMmXValueUtils, (PPMoaVoid)&pObj->fpXUtils));	
			
moa_catch
moa_catch_end
moa_try_end
}

STDMETHODIMP_(void) MoaDestroy_CScript(CScript FAR * pObj) {
			
	if (pObj->pMmValue) pObj->pMmValue->Release();
	pObj->pMmValue = NULL;
	if (pObj->fpXUtils) pObj->fpXUtils->Release();
	pObj->fpXUtils = NULL;
}

//**********************************
//*************** Main functions 
//**********************************

STDMETHODIMP
CScript_IMoaMmXScript::newBytes(PMoaDrCallInfo callPtr)
{
	
	PIMoaMmXValue pNewXVal = NULL;
	MoaMmValue value;
	GetArgByIndex(1, &value);
	MoaMmValueType checkType = kMoaMmValueType_Integer;
	 MoaMmValueType valueType;
	pObj->pMmValue->ValueType(&value, &valueType);
	if (valueType != checkType) {
		pObj->pMmValue->IntegerToValue(0, &callPtr->resultValue);
		return kMoaMmErr_IntegerExpected;
	}

	MoaLong nbBytes;
	pObj->pMmValue->ValueToInteger(&value, &nbBytes);

	// create new bytes object
	pObj->pCallback->MoaCreateInstance(&CLSID_Bytes, &IID_IMoaMmXValue, (PPMoaVoid)&pNewXVal);
	pNewXVal->SetData((PMoaVoid)nbBytes);
	
	// convert to value
	pObj->fpXUtils->IXValueToValue(pNewXVal, &callPtr->resultValue);
	pNewXVal->Release();	// now the resultValue is holding this object

	return kMoaErr_NoErr;
}

STDMETHODIMP
CScript_IMoaMmXScript::newBytesFromString(PMoaDrCallInfo callPtr) {
	MoaError	err = kMoaErr_NoErr;
	MoaMmValue value;
	GetArgByIndex(1, &value);

	MoaMmValueType checkType = kMoaMmValueType_String;
	 MoaMmValueType valueType;
	pObj->pMmValue->ValueType(&value, &valueType);
	if (valueType != checkType) {
		pObj->pMmValue->IntegerToValue(0, &callPtr->resultValue);
		return kMoaMmErr_StringExpected;
	}
	
	char theString[1024];
	pObj->pMmValue->ValueToString(&value, theString, 1024);
	
	// create new bytes object
	PIMoaMmXValue pNewXVal = NULL;
	pObj->pCallback->MoaCreateInstance(&CLSID_Bytes, &IID_IMoaMmXValue, (PPMoaVoid)&pNewXVal);

	MoaLong nbBytes = strlen(theString);
	pNewXVal->SetData((PMoaVoid)(nbBytes+1));
	((Bytes_IMoaMmXValue *)pNewXVal)->copyDataFrom(theString, nbBytes+1);

	// convert to value
	pObj->fpXUtils->IXValueToValue(pNewXVal, &callPtr->resultValue);
	pNewXVal->Release();	// now the resultValue is holding this object

	return kMoaErr_NoErr;
}


/* -------------------------------------------- CScript_IMoaMmXScript::Call */
STDMETHODIMP CScript_IMoaMmXScript::Call(PMoaMmCallInfo callPtr) {
	MoaError err = kMoaErr_NoErr;

	switch	( callPtr->methodSelector ) {
	
		case m_newBytes:
			newBytes(callPtr);
			break;			

		case m_newBytesFromString:
			newBytesFromString(callPtr);
			break;			
			
	}
	
	return err;
}




#include "bytesValue.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "moatry.h"


// Bytes implementation

MoaError MoaCreate_Bytes (Bytes * pObj) {
moa_try
		
	pObj->pValue = NULL;
	pObj->pData = NULL;
	pObj->pSize = 0;
	pObj->pAllocSize = 0;
	
	ThrowErr (pObj->pCallback->QueryInterface(&IID_IMoaMmValue, (PPMoaVoid) &pObj->pValue));

moa_catch
moa_catch_end
moa_try_end
}

void MoaDestroy_Bytes(Bytes * pObj)
{
	if (pObj->pData)
		pObj->pCalloc->NRFree(pObj->pData);
	pObj->pData = NULL;

	pObj->pValue->Release();
}

/* ============================================================================= */
/*  Construction/Destruction in Bytes_IMoaMmXValue */
/* ============================================================================= */

BEGIN_DEFINE_CLASS_INTERFACE(Bytes, IMoaMmXValue)
END_DEFINE_CLASS_INTERFACE

Bytes_IMoaMmXValue::Bytes_IMoaMmXValue(MoaError * pError) { *pError = kMoaErr_NoErr; }	
Bytes_IMoaMmXValue::~Bytes_IMoaMmXValue() { }

/* ============================================================================= */
/*  Method Implementation in Bytes_IMoaMmXValue */
/* ============================================================================= */

// Argument is the size
MoaError Bytes_IMoaMmXValue::SetData( PMoaVoid nbBytes ) {
	pObj->pSize = pObj->pAllocSize = (int)nbBytes;

	if (pObj->pAllocSize > 0) {
		pObj->pData = (unsigned char *)pObj->pCalloc->NRAlloc(pObj->pAllocSize);
		memset(pObj->pData, 0, pObj->pAllocSize);
	}
	return kMoaErr_NoErr;
}
								  

MoaError Bytes_IMoaMmXValue::GetProp( ConstPMoaMmValue selfRef, MoaMmSymbol propName, MoaLong indexCount, ConstPMoaMmValue pIndexValues, PMoaMmValue pResult ) {
	MoaError err;
	MoaMmSymbol sym;

	if (indexCount==0) {			
		err = pObj->pValue->StringToSymbol("size", &sym);
		if (err==kMoaErr_NoErr && propName==sym)
			return pObj->pValue->IntegerToValue(pObj->pSize, pResult);
				
	} else {
	  // indexCount is 1 or 2
		// for lingo (not javascript) indexed access, we use a hidden "data" property
		err = pObj->pValue->StringToSymbol("data", &sym);
		if (err==kMoaErr_NoErr && propName==sym) {
			MoaLong index1, index2;
			err = pObj->pValue->ValueToInteger(&pIndexValues[0], &index1);
			if (err!=kMoaErr_NoErr) return err;
			if (index1<1 || index1>pObj->pSize) return kMoaMmErr_ArgOutOfRange;
			if (indexCount==2) {
				err = pObj->pValue->ValueToInteger(&pIndexValues[1], &index2);
				if (err!=kMoaErr_NoErr) return err;
				if (index2<index1 || index2>pObj->pSize) return kMoaMmErr_ArgOutOfRange;
			}
				
			if (indexCount == 1)
					return pObj->pValue->IntegerToValue(getAt(index1), pResult);
			
			if (indexCount==2) {
				// range : a list
				PIMoaMmList pList = NULL;
				MoaMmValue xList;
				int ix;
				err = pObj->pCallback->QueryInterface(&IID_IMoaMmList, (PPMoaVoid)&pList);
				if (err!=kMoaErr_NoErr) goto exit_list;
				
				err = pList->NewListValue(&xList);
				if (err!=kMoaErr_NoErr) goto exit_list;
				
				for (ix=index1; ix<=index2; ix++) {
					MoaMmValue aValue;
					err = pObj->pValue->IntegerToValue(getAt(ix), &aValue);
					if (err!=kMoaErr_NoErr) goto exit_list;
					
					err = pList->AppendValueToList(&xList, &aValue);
					pObj->pValue->ValueRelease(&aValue);
					if (err!=kMoaErr_NoErr) goto exit_list;
				}
				*pResult = xList;
exit_list:
				if (pList) pList->Release();
				if (err != kMoaErr_NoErr) pObj->pValue->ValueRelease(&xList);
				return err;
			}
		}
	}
	
	return kMoaMmErr_PropertyNotFound;
}

MoaError Bytes_IMoaMmXValue::SetProp( MoaMmSymbol propName, MoaLong indexCount, ConstPMoaMmValue pIndexValues, ConstPMoaMmValue pNewValue ) {
	MoaError err = kMoaMmErr_CannotSetProperty;
	MoaMmSymbol sym;

	if (indexCount==0) {
		err = pObj->pValue->StringToSymbol("size", &sym);
		if (err==kMoaErr_NoErr && propName==sym) {
			MoaMmValueType typ;
			err = pObj->pValue->ValueType(pNewValue, &typ);
			if (err==kMoaErr_NoErr && typ==kMoaMmValueType_Integer) {
				MoaLong index1;
				pObj->pValue->ValueToInteger(pNewValue, &index1);
				if (index1<0) return kMoaMmErr_ArgOutOfRange;
				
				resizeTo(index1);

				return err;
			}
			err = kMoaMmErr_IntegerExpected;
		}
		return err;
	}
			
	// for lingo (not javascript) indexed access, we use a hidden "data" property
	err = pObj->pValue->StringToSymbol("data", &sym);
	if (err==kMoaErr_NoErr && propName==sym) {
		MoaLong index1, index2;
		err = pObj->pValue->ValueToInteger(&pIndexValues[0], &index1);
		if (err!=kMoaErr_NoErr) return err;
		if (index1 < 1) return kMoaMmErr_ArgOutOfRange;
					
		if (indexCount == 2) {
			err = pObj->pValue->ValueToInteger(&pIndexValues[1], &index2);
			if (err!=kMoaErr_NoErr) return err;
			if (index2<index1) return kMoaMmErr_ArgOutOfRange;
		}
		
		// ------
		MoaLong tmpInt;
		err = pObj->pValue->ValueToInteger(pNewValue, &tmpInt);
		
		setAt(index1, tmpInt);
		
		// range ?
		if (err==kMoaErr_NoErr && indexCount==2) {
			for (int ix = index1+1; ix<=index2; ix++)
				 	setAt(ix, tmpInt);
		}
		return err;
	}

	return err;
}

static MoaLong S_GetArrayIndex(PIMoaMmValue pValue, PMoaMmValue pValu) {
	MoaLong iResult = -1;
	MoaMmValueType vtype = kMoaMmValueType_Void;
	pValue->ValueType(pValu, &vtype);
	if (vtype==kMoaMmValueType_Integer)
		pValue->ValueToInteger( pValu, &iResult );
	return iResult;
}

MoaError S_GetBytesObjectInto(Bytes * pObj, PMoaMmValue pValu, Bytes_IMoaMmXValue **pXValParam) {
	// fills pXValParam with a valid XValParam from pValu
	
	// get IID_IMoaMmXValueUtils
	PIMoaMmXValueUtils fpXUtils;
	MoaError err = pObj->pCallback->QueryInterface(&IID_IMoaMmXValueUtils, (PPMoaVoid)&fpXUtils);
	if (err != kMoaErr_NoErr)
		return err;

  // is it an XValue ?
	if (!fpXUtils->valueIsIXValue(pValu))
		// no
		return kMoaMmErr_NotIMoaMmXValue;

	// yes, convert it to a XValue
	fpXUtils->ValueToIXValue(pValu, (PIMoaMmXValue*)pXValParam); // this AddRefs *pXValParam : must be released after
	
	fpXUtils->Release(); // not used anymore
	
	// See if it's _our_ type of XValue object
	MoaMmValue ilkValue;
	MoaMmSymbol ilkSym, xvalSym;
	(*pXValParam)->Ilk(NULL, &ilkValue);
	pObj->pValue->ValueToSymbol(&ilkValue, &ilkSym);
	pObj->pValue->ValueRelease(&ilkValue);
	pObj->pValue->StringToSymbol("bytes", &xvalSym);
	if (xvalSym != ilkSym) {
		// not a bytes value
		(*pXValParam)->Release();
		// return error
		return kMoaMmErr_ValueTypeMismatch;
	}
	// ok
	return kMoaErr_NoErr;
}

MoaError Bytes_IMoaMmXValue::CallHandler( PMoaMmCallInfo callPtr ) {
	MoaError err;
	MoaMmSymbol sym;

	// Attention callPtr->pArgs[0] is the value itself

	// ---- toString
	err = pObj->pValue->StringToSymbol("toString", &sym);
	if (err==kMoaErr_NoErr && callPtr->methodSelector==sym) {
		char *buffer = (char *)malloc(pObj->pSize+1);
		memset(buffer, 0, pObj->pSize+1);
		copyDataTo(buffer, pObj->pSize);
		
		pObj->pValue->StringToValue(buffer, &callPtr->resultValue);
		
		free(buffer);
		return kMoaErr_NoErr;
	}


	// ---- offset
	err = pObj->pValue->StringToSymbol("offset", &sym);
	if (err==kMoaErr_NoErr && callPtr->methodSelector==sym) {
		
		// get other bytes object
		Bytes_IMoaMmXValue *otherBytes = NULL;
		err = S_GetBytesObjectInto(pObj, &callPtr->pArgs[1], &otherBytes);
		if (err != kMoaErr_NoErr) return err;
		
		// our other bytes object
		MoaLong len2 = otherBytes->size();
		MoaLong len1 = pObj->pSize;

		// second optional argument
		MoaLong startOffset = 0;
		if (callPtr->nargs > 2) {
			// startOffset
			startOffset = S_GetArrayIndex(pObj->pValue, &callPtr->pArgs[2]);
			if (startOffset < 0) return kMoaMmErr_IntegerExpected;
			if (startOffset == 0) return kMoaMmErr_ArgOutOfRange;
			startOffset--; // lingo to C++
		}
		
		MoaLong off = 0;
		if (startOffset + len2 <= len1) {
			// ok, lets compare
			unsigned char *data2 = otherBytes->data();
			for (int i = startOffset; i <= len1 - len2; i++)
				if (!memcmp(pObj->pData+i, data2, len2)) {
					//found
					off = i;
					break;
				}
		}
		off++; // C++ to lingo
		pObj->pValue->IntegerToValue(off, &callPtr->resultValue);
		
		otherBytes->Release();
		return kMoaErr_NoErr;
	}

	// ---- copyFrom
	err = pObj->pValue->StringToSymbol("copyFrom", &sym);
	if (err==kMoaErr_NoErr && callPtr->methodSelector==sym) {
		
		// get other bytes object
		Bytes_IMoaMmXValue *otherBytes = NULL;
		err = S_GetBytesObjectInto(pObj, &callPtr->pArgs[1], &otherBytes);
		if (err != kMoaErr_NoErr) return err;
		
		// our other bytes object
		MoaLong len2 = otherBytes->size();
		
		// other arguments
		MoaLong bytesCount = len2, srcOffset = 0, dstOffset = 0;

		// check types and get arguments
		MoaMmValueType checkType = kMoaMmValueType_Integer;
		
		if (callPtr->nargs > 2) {
			// dstOffset
			dstOffset = S_GetArrayIndex(pObj->pValue, &callPtr->pArgs[2]);
			if (dstOffset < 0) return kMoaMmErr_IntegerExpected;
			if (dstOffset == 0) return kMoaMmErr_ArgOutOfRange;
			dstOffset--; //lingo to C

			if (callPtr->nargs > 3) {
				// bytesCount
				bytesCount = S_GetArrayIndex(pObj->pValue, &callPtr->pArgs[3]);
				if (bytesCount < 0) return kMoaMmErr_IntegerExpected;
				if (bytesCount == 0) return kMoaMmErr_ArgOutOfRange;
				
				if (callPtr->nargs > 4) {
					// srcOffset
					srcOffset = S_GetArrayIndex(pObj->pValue, &callPtr->pArgs[4]);
					if (srcOffset < 0) return kMoaMmErr_IntegerExpected;
					if (srcOffset == 0) return kMoaMmErr_ArgOutOfRange;
					srcOffset--; //lingo to C
				}
			}
		}
		
		if (dstOffset + bytesCount > pObj->pSize) return kMoaMmErr_ArgOutOfRange;
		if (srcOffset + bytesCount > len2) return kMoaMmErr_ArgOutOfRange;
	
		// ok, lets copy
		unsigned char *data2 = otherBytes->data();
		memcpy(pObj->pData + dstOffset, data2 + srcOffset, bytesCount);
		
		pObj->pValue->IntegerToValue(bytesCount, &callPtr->resultValue);
		
		otherBytes->Release();
		return kMoaErr_NoErr;
	}

	// this is how bracket[] access is forwarded to this object 
	err = pObj->pValue->StringToSymbol("getat", &sym);
	if (err==kMoaErr_NoErr && callPtr->methodSelector==sym) {
		MoaLong index = S_GetArrayIndex(pObj->pValue, &callPtr->pArgs[1]);
		if (index < 0) return kMoaMmErr_IntegerExpected;
		if (index < 1) return kMoaMmErr_ArgOutOfRange;
		if (index > pObj->pSize) return kMoaMmErr_ArgOutOfRange;

		return pObj->pValue->IntegerToValue(getAt(index), &callPtr->resultValue);
	}
	
	// this is how [] access is forwarded to this object 
	err = pObj->pValue->StringToSymbol("setat", &sym);
	if (err==kMoaErr_NoErr && callPtr->methodSelector==sym) {
		MoaLong index = S_GetArrayIndex(pObj->pValue, &callPtr->pArgs[1]);
		if (index < 0) return kMoaMmErr_IntegerExpected;
		if (index < 1) return kMoaMmErr_ArgOutOfRange;
		
		MoaLong tmpInt;
		pObj->pValue->ValueToInteger(&callPtr->pArgs[2], &tmpInt);
		setAt(index, tmpInt);
		return kMoaErr_NoErr;
	}
	
	err = kMoaMmErr_FunctionNotFound;
	return err;
}

MoaError Bytes_IMoaMmXValue::Ilk( PMoaMmValue pArgument, PMoaMmValue pResult ) {
	MoaMmSymbol sym;
	MoaError err;
	if (pArgument != NULL)
		err = kMoaMmErr_AccessNotSupported;
	
	err = SymbolRep(&sym);
	if (err==kMoaErr_NoErr)
		err = pObj->pValue->SymbolToValue(sym, pResult);
	return err;
}

MoaError Bytes_IMoaMmXValue::StringRep( PMoaMmValue pStringVal ) {
moa_try
	MoaChar stringBuf[64];
	sprintf(stringBuf, "<bytes size=%ld>", pObj->pSize);
	// could add the "description" here - but watch for buffer overflow!
	ThrowErr( pObj->pValue->StringToValue( stringBuf, pStringVal) );

moa_catch
moa_catch_end
moa_try_end
}

MoaError Bytes_IMoaMmXValue::SymbolRep( PMoaMmSymbol pSymbol ) {
	return pObj->pValue->StringToSymbol("bytes", pSymbol);
}

void Bytes_IMoaMmXValue::copyDataTo( void * buf, MoaLong bufSiz ) {
	if (bufSiz > pObj->pSize) bufSiz = pObj->pSize;
	memcpy(buf, pObj->pData, bufSiz);
}

void Bytes_IMoaMmXValue::copyDataFrom( void * buf, MoaLong bufSiz ) {
	if (bufSiz > pObj->pSize) bufSiz = pObj->pSize;
	memcpy(pObj->pData, buf, bufSiz);
}

int Bytes_IMoaMmXValue::size() {
	return pObj->pSize;
}

unsigned char * Bytes_IMoaMmXValue::data() {
	return pObj->pData;
}

void Bytes_IMoaMmXValue::resizeTo( MoaLong newSiz ) {
	if (pObj->pSize == newSiz) return;
	
	if (newSiz > pObj->pSize) {
		if (newSiz > pObj->pAllocSize) {
			// double alloced size until enough
			if (pObj->pAllocSize == 0) pObj->pAllocSize = 1;
			while (pObj->pAllocSize < newSiz) pObj->pAllocSize *= 2;
			
			// switch
			unsigned char * newData = (unsigned char *)pObj->pCalloc->NRAlloc(pObj->pAllocSize); // alloc new
			memset(newData, 0, pObj->pAllocSize);
			memcpy(newData, pObj->pData, pObj->pSize); // copy old
			pObj->pCalloc->NRFree(pObj->pData);
			pObj->pData = newData;
		}
		memset(pObj->pData + pObj->pSize+1, 0, newSiz - pObj->pSize - 1); // set new part to 0 in case it was not resized
		pObj->pSize = newSiz;
		
	} else {
		// optimize a bit : reduce alloced size
		if (newSiz == 0) {
			pObj->pSize = pObj->pAllocSize = 0;
			pObj->pCalloc->NRFree(pObj->pData);
			pObj->pData = NULL;
		} else {
			if (newSiz < pObj->pAllocSize/2) {
				// divide by 2 until too small
				while (pObj->pAllocSize >= newSiz) pObj->pAllocSize /= 2;
				pObj->pAllocSize *= 2;
				
				// switch
				unsigned char * newData = (unsigned char *)pObj->pCalloc->NRAlloc(pObj->pAllocSize); // alloc new
				memset(newData, 0, pObj->pAllocSize);
				memcpy(newData, pObj->pData, newSiz); // copy old
				pObj->pCalloc->NRFree(pObj->pData);
				pObj->pData = newData;
			}
			pObj->pSize = newSiz;
		}
	}
}

unsigned char Bytes_IMoaMmXValue::getAt(int index) {
	if (index < 1) return 0;
	if (index > pObj->pSize) return 0; 
	return pObj->pData[index-1];
}

void Bytes_IMoaMmXValue::setAt(int index, unsigned char val) {
	if (index < 1) return;
	if (index > pObj->pSize) resizeTo(index);; 
	pObj->pData[index-1] = val;
}

// --------- UNUSED FUNCTIONS -------------
MoaError Bytes_IMoaMmXValue::IntegerRep( PMoaLong pIntVal ) {
	*pIntVal = 0;
	return kMoaMmErr_AccessNotSupported;
}

MoaError Bytes_IMoaMmXValue::StreamOut( PIMoaStream2 pStream ) {
	return kMoaMmErr_AccessNotSupported;
}
  
MoaError Bytes_IMoaMmXValue::StreamIn( PIMoaStream2 pStream ) {
	return kMoaMmErr_AccessNotSupported;
}

MoaError Bytes_IMoaMmXValue::GetPropCount( MoaLong * pCount ) {
	*pCount = 0;
	return kMoaMmErr_AccessNotSupported;
}

MoaError Bytes_IMoaMmXValue::GetPropDescriptionByIndex( MoaLong index, PMoaMmValueDescription pDescription ) {
	return kMoaMmErr_AccessNotSupported;
}

MoaError Bytes_IMoaMmXValue::GetCount( MoaMmSymbol propName, MoaLong * pCount ) {
	*pCount = 0;
	return kMoaErr_NoErr;
}

MoaError Bytes_IMoaMmXValue::AccessPropRef( ConstPMoaMmValue selfRef, MoaMmSymbol propName, MoaLong indexCount, ConstPMoaMmValue pIndexValues, PMoaMmValue pResult ){
	return kMoaMmErr_AccessNotSupported;
}

MoaError Bytes_IMoaMmXValue::GetContents( ConstPMoaMmValue selfRef, PMoaMmValue pResult ) {
	return kMoaMmErr_AccessNotSupported;
}

MoaError Bytes_IMoaMmXValue::SetContents( PMoaMmValue pNewValue ) {
	return kMoaMmErr_AccessNotSupported;
}

MoaError Bytes_IMoaMmXValue::SetContentsBefore( PMoaMmValue pNewValue ) {
	return kMoaMmErr_AccessNotSupported;
}

MoaError Bytes_IMoaMmXValue::SetContentsAfter( PMoaMmValue pNewValue ) {
	return kMoaMmErr_AccessNotSupported;
}


#include "..\..\..\include\EmbPython\SprEPFoundation.h"
#include "..\..\..\include\EmbPython\SprEPUtility.h"
#include "..\..\..\include\EmbPython\SprEPBase.h"

void SPR_CDECL PyUTTimerFunc(int id, void* arg){
	//	PyEval_InitThreads();
	PyGILState_STATE state = PyGILState_Ensure();
	PyObject_CallObject((PyObject*)arg, NULL);
	PyGILState_Release(state);
}

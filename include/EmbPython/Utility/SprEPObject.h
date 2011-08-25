#ifndef SPREPOBJECT_H
#define SPREPOBJECT_H

#include <Python/Python.h>
#include <Springhead.h>
#include "SprEPFoundation.h"

using namespace Spr;




/********************************EPObject*************************************/
#define EPObject_Check(ob) (((PyObject*)ob)->ob_type == &EPObjectType)
#define EPObject_Ptr(ob) (((EPObject*)ob)->ptr)
#define EPObject_Cast(ob,dest) ((dest*)((EPObject*)ob)->ptr)
///////////////////////////////////�I�u�W�F�N�g�̐錾////////////
typedef struct
{
	PyObject_HEAD
	const void *ptr;
	MemoryManager mm;
	
} EPObject;

/////////////////////////////////////���\�b�h�o�^�p
///void�̃��\�b�h�̒�`

PyObject*  EPObject_new(PyTypeObject *type,PyObject *args, PyObject *kwds);
EPObject* newEPObject();
EPObject* newEPObject(const void*);

//////////////////////////////////////�I�u�W�F�N�g�̃^�C�v�錾
extern PyTypeObject EPObjectType;
//////////////////////////////////////�I�u�W�F�N�g�i���W���[���́j�������֐�
PyMODINIT_FUNC initEPObject(void) ;

#endif
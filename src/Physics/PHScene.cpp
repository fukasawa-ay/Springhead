#include "Physics.h"
#ifdef USE_HDRSTOP
#pragma hdrstop
#endif
#include <sstream>

namespace Spr{;

//----------------------------------------------------------------------------
//	PHScene
OBJECTIMP(PHScene, Scene);
IF_IMP(PHScene, Object);

PHScene::PHScene(PHSdkIf* s, const PHSceneDesc& desc):PHSceneDesc(desc){
	Init();
	sdk = s;
}
PHScene::PHScene(){
	Init();
}
void PHScene::Init(){
	engines.scene = this;
	Scene::Clear();

	solids = DBG_NEW PHSolidContainer;
	engines.Add(solids);
	PHSolidInitializer* si = DBG_NEW PHSolidInitializer;
	si->container = solids;
	engines.Add(si);
	
	PHGravityEngine* ge = DBG_NEW PHGravityEngine;
	engines.Add(ge);
	ge->accel = gravity;

	switch(contactSolver){
	case SOLVER_PENALTY:{
		PHPenaltyEngine* pe = DBG_NEW PHPenaltyEngine;
		engines.Add(pe);
		}break;
	case SOLVER_CONSTRAINT:{
		PHConstraintEngine* ce = DBG_NEW PHConstraintEngine;
		engines.Add(ce);
		}break;
	default: assert(false);
	}

	count = 0;
}

PHSdkIf* PHScene::GetSdk(){
	return sdk;	
}

PHSolidIf* PHScene::CreateSolid(const PHSolidDesc& desc){
	UTRef<PHSolid> s = DBG_NEW PHSolid(desc, (PHSceneIf*)this);

	solids->AddChildObject(s, this);

	switch(contactSolver){
	case SOLVER_PENALTY:{
		PHPenaltyEngine* pe;
		engines.Find(pe);
		assert(pe);
		pe->Add(s);
		}break;
	case SOLVER_CONSTRAINT:{
		PHConstraintEngine* ce;
		engines.Find(ce);
		assert(ce);
		ce->Add(s);
		}break;
	default: assert(false);
	}
	return s;
}
PHSolidIf* PHScene::CreateSolid(){
	PHSolidDesc def;
	return CreateSolid(def);
}
CDShapeIf* PHScene::CreateShape(const CDShapeDesc& desc){
	CDShapeIf* rv = sdk->CreateShape(desc);
	return rv;
}

int PHScene::NSolids(){
	return solids->solids.size();
}
PHSolidIf** PHScene::GetSolids(){
	return (PHSolidIf**)&*solids->solids.begin();
}

PHJointIf* PHScene::CreateJoint(PHSolidIf* lhs, PHSolidIf* rhs, const PHJointDesc& desc){
	PHConstraintEngine* ce;
	engines.Find(ce);
	assert(ce);
	return ce->AddJoint((PHSolid*)lhs, (PHSolid*)rhs, desc);	
}

void PHScene::Clear(){
	engines.Clear();
	Init();
}

void PHScene::SetTimeStep(double dt){
	timeStep = dt;
}

void PHScene::Step(){
	ClearForce();
	GenerateForce();
	Integrate();
}
void PHScene::ClearForce(){
	engines.ClearForce();
}
#define FP_ERROR_MASK	(_EM_INEXACT|_EM_UNDERFLOW)
void PHScene::GenerateForce(){
//	_controlfp(FP_ERROR_MASK, _MCW_EM);	//	コメントをはずすと例外が起きる．要調査
	engines.GenerateForce();
//	_controlfp(_MCW_EM, _MCW_EM);
}
void PHScene::Integrate(){
//	_controlfp(FP_ERROR_MASK, _MCW_EM);
	engines.Integrate();
//	_controlfp(_MCW_EM, _MCW_EM);
//	time += timeStep;
	count++;
}

void PHScene::EnableContact(PHSolidIf* lhs, PHSolidIf* rhs, bool bEnable){
	PHConstraintEngine* ce;
	engines.Find(ce);
	assert(ce);
	ce->EnableContact((PHSolid*)lhs, (PHSolid*)rhs, bEnable);
}

void PHScene::SetGravity(Vec3f accel){
	PHGravityEngine* ge;
	engines.Find(ge);
	assert(ge);
	ge->accel = accel;
}
Vec3f PHScene::GetGravity(){
	PHGravityEngine* ge;
	engines.Find(ge);
	assert(ge);
	return ge->accel;
}
ObjectIf* PHScene::CreateObject(const IfInfo* info, const void* desc){
	if (info->Inherit(CDShapeIf::GetIfInfoStatic())){
		return CreateShape(*(CDShapeDesc*)desc);
	}else if (info->Inherit(PHJointIf::GetIfInfoStatic())){
//		return CreateJoint(*(PHJointDesc*)desc);
		assert(0);
	}else if(info == PHSolidIf::GetIfInfoStatic()){
		return CreateSolid(*(const PHSolidDesc*)desc);
	}
	return NULL;
}


}

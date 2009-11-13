/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */

#include <Framework/FWInteractPointer.h>

#ifdef USE_HDRSTOP
#pragma hdrstop
#endif

namespace Spr{;

FWInteractPointer::FWInteractPointer(){ Init(); }
FWInteractPointer::FWInteractPointer(const FWInteractPointerDesc& desc) : FWInteractPointerDesc(desc){ Init(); }

void FWInteractPointer::Init(){
	bForce = false;
	bVibration = false;
	CalcCorrectionSpringDamper();
}
void FWInteractPointer::SetPointerSolid(PHSolidIf* solid){
	pointerSolid = solid;
}
PHSolidIf* FWInteractPointer::GetPointerSolid(){
	return pointerSolid; 
}
void FWInteractPointer::SetHI(HIBaseIf* hi){
	humanInterface  = hi;
}
HIBaseIf* FWInteractPointer::GetHI(){
	return humanInterface; 
}
void FWInteractPointer::SetPosScale(double s){
	posScale = s; 
	CalcCorrectionSpringDamper();
}
double FWInteractPointer::GetPosScale(){
	return posScale; 
}
void FWInteractPointer::SetForceScale(double s){
	forceScale = s; 
}
double FWInteractPointer::GetForceScale(){
	return forceScale; 
}
void FWInteractPointer::SetLocalRange(double r){
	localRange = r; 
}
double FWInteractPointer::GetLocalRange(){
	return localRange; 
}
void  FWInteractPointer::SetDefaultPosition(Posed p){
	defaultPosition = p;
}

Posed  FWInteractPointer::GetDefaultPosition(){
	return defaultPosition; 
}

void  FWInteractPointer::SetPointersCalibPosition(Posed p){
	pointersCalibPosition = p;
}

Posed  FWInteractPointer::GetPointersCalibPosition(){
	return pointersCalibPosition; 
}

void FWInteractPointer::CalcCorrectionSpringDamper(){
	correctionSpringK = springK / posScale;
	correctionDamperD = damperD / posScale;
} 

void FWInteractPointer::EnableForce(bool b){
	bForce = b;
}
void FWInteractPointer::EnableVibration(bool b){
	bVibration = b;
}
void FWInteractPointer::Calibration(){
	GetHI()->Calibration();
}

}
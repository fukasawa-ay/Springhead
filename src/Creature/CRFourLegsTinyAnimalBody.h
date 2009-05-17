/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#ifndef CRFOURLEGSTINYANIMALBODY_H
#define CRFOURLEGSTINYANIMALBODY_H

#include <Springhead.h>

#include <Foundation/Object.h>
#include <Physics/PHConstraint.h>
#include <Physics/PhysicsDecl.hpp>

#include <vector>

#include "CRBody.h"

//@{
namespace Spr{;

// --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---
// CRFourLegsTinyAnimalBody
// ヒンジジョイントを用いた哺乳類モデル・クラスの実装（未実装：中身はTrunkFootHumanBody）
class CRFourLegsTinyAnimalBody : public CRBody, public CRFourLegsTinyAnimalBodyDesc {
private:
	double massFF;		//< 前足の質量
	double massFL;		//< 前脚の質量
	double massRF;		//< 後足の質量
	double massRL;		//< 後脚の質量
	double massBody;	//< 体幹の質量

	void CreateBody();
	void CreateFrontLegs(LREnum lr);
	void CreateRearLegs(LREnum lr);
	void InitBody();
	void InitFrontLeg0(LREnum lr);
	void InitFrontLeg1(LREnum lr);
	void InitRearLeg0(LREnum lr);
	void InitRearLeg1(LREnum lr);
	void InitLegs();

	void InitContact();
	void InitControlMode(PHJointDesc::PHControlMode m = PHJointDesc::MODE_POSITION);	//< ボディの制御モードを設定する．

public:
	SPR_OBJECTDEF(CRFourLegsTinyAnimalBody);
	ACCESS_DESC(CRFourLegsTinyAnimalBody);

	CRFourLegsTinyAnimalBody(){
		massFF		= 10;
		massFL		= 20;
		massRF		= 10;
		massRL		= 20;
		massBody	= 50;
	}
	CRFourLegsTinyAnimalBody(const CRFourLegsTinyAnimalBodyDesc& desc, CRCreatureIf* c=NULL) 
		: CRFourLegsTinyAnimalBodyDesc(desc) 
		, CRBody((const CRBodyDesc&)desc, c)
	{
		solids.resize(CRFourLegsTinyAnimalBodyDesc::SO_NSOLIDS);
		joints.resize(CRFourLegsTinyAnimalBodyDesc::JO_NJOINTS);
		massFF		= 1.5;
		massFL		= 1.5;
		massRF		= 1.5;
		massRL		= 1.5;
		massBody	= 10;
		InitBody();
		InitLegs();
		InitContact();
		InitControlMode();
		Init();
	}

	/** @brief 初期化を実行する
	*/
	virtual void Init();
	virtual Vec2d GetSwingLimit(){return rangeFrontSwing;}
	virtual Vec2d GetTwistLimit(){return rangeFrontTwist;}

};

}
//@}

#endif//CRFourLegsTinyAnimalBODY_H

/*
 *  Copyright (c) 2003-2006, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#ifndef PHBALLJOINT_H
#define PHBALLJOINT_H

#include <SprPhysics.h>
#include <Physics/PHJoint.h>
#include <Physics/PHTreeNode.h>

namespace Spr{;


///	ボールジョイントに対応するツリーノード
class PHBallJointNode : public PHTreeNodeND<3>{
public:
	OBJECTDEF(PHBallJointNode, PHTreeNode);
	/// スイング・ツイスト角の時間変化率から角速度へのヤコビアン
	Matrix3d	Jst;

	PHBallJoint* GetJoint(){return DCAST(PHBallJoint, PHTreeNodeND<3>::GetJoint());}
	virtual void CompJointJacobian();
	virtual void CompJointCoriolisAccel();
	virtual void UpdateJointPosition(double dt);
	virtual void CompRelativePosition();
	virtual void CompRelativeVelocity();
	virtual void ModifyJacobian();
	virtual void CompBias();
	virtual void Projection(double& f, int i);
	PHBallJointNode(const PHBallJointNodeDesc& desc = PHBallJointNodeDesc()){}
};



class PHBallJoint : public PHJointND<3>, public PHBallJointIfInit{
public:
	OBJECTDEF(PHBallJoint, PHJoint);
	struct OnLimit{
		bool onLower;
		bool onUpper;
		bool& operator [] (int i){
			return i==0 ? onLower : onUpper;
		}
	};
	LimitST limit;		//	可動域
	OnLimit onLimit[3];				//	可動域制限にかかっていると true
// メモ↑-------------------------------------------------------------------------------------
//	Swing軸：onLimit[0]
//	Swing角：onLimit[1]
//	Twist角：onLimit[2]
//	である。例えばSwing軸のonLowerを得るためにはonLimit[0][0] or onLimit[0].onLowerと指定する
// -------------------------------------------------------------------------------------------	

	Vec3d spring, damper;
	SwingTwist goal;
	
	SwingTwist	angle; //, velocity;	///< スイング・ツイスト角表現の角度と角速度
	//Vec3d		torque;
	/// 角速度からスイング・ツイスト角の時間変化率へのヤコビアン
	Matrix3d	Jstinv;
	/// インタフェースの実装
	//virtual PHConstraintDesc::ConstraintType GetConstraintType(){return PHJointDesc::BALLJOINT;}

	virtual void	SetSwingDirRange(double l, double u){limit.upper.SwingDir() = u; limit.lower.SwingDir() = l;}
	virtual void	GetSwingDirRange(double& l, double& u){l = limit.lower.SwingDir(); u = limit.upper.SwingDir();}

	virtual void	SetSwingRange(double l, double u){limit.upper.Swing() = u; limit.lower.Swing() = l;}
	virtual void	GetSwingRange(double& l, double& u){l = limit.lower.Swing(); u = limit.upper.Swing();}

	virtual void	SetTwistRange(double l, double u){limit.lower.Twist() = l; limit.upper.Twist() = u;}
	virtual void	GetTwistRange(double& l, double& u){l = limit.lower.Twist(); u = limit.upper.Twist();}

	virtual void	SetMotorTorque(const Vec3d& t){torque = t;}
	virtual Vec3d	GetMotorTorque(){return torque;}
	virtual Vec3d	GetAngle(){return position;}
	virtual Vec3d	GetVelocity(){return velocity;}
	
	/// 仮想関数のオーバライド
	virtual bool GetDesc(void* desc);
	virtual void SetDesc(const void* desc);
	virtual void AddMotorTorque(){f.w() = torque * scene->GetTimeStep();}
	virtual void SetConstrainedIndex(bool* con);
	virtual void ModifyJacobian();
	virtual void CompBias();
	virtual void Projection(double& f, int k);
	virtual void UpdateJointState();
	virtual void CompError();
	
	virtual PHTreeNode* CreateTreeNode(){
		return DBG_NEW PHBallJointNode();
	}
	PHBallJoint(const PHBallJointDesc& desc = PHBallJointDesc());
};

}

#endif

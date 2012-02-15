#ifndef PH_HAPTICPOINTER_H
#define PH_HAPTICPOINTER_H

#include <Springhead.h>
#include <Physics/PHSolid.h>

namespace Spr{;

class PHSolidsForHaptic;
class PHSolidPairForHaptic;
class PHSolidPairsForHaptic;
class PHShapePairForHaptic;

struct PHIntermediateRepresentation :public UTRefCount{
	int solidID;				///< 剛体のID(PHSolidsForHapitcとPHSolidPairForHaptic)と侵入しているか
	PHSolidPairForHaptic* solidPair;	///< PHSolidPairForHapticへのポインタ
	double depth;				///< 侵入量
	Vec3d normal;				///< 面法線
	Vec3d r;					///< 力覚ポインタの重心からの接触点までのベクトル
	Vec3d pointerPointW;		///< 力覚ポンタの侵入点(world)
	Vec3d contactPointW;		///< 中間表現面にvertexを投影した位置(world)
	Vec3d pointerPointVel;		///< 力覚ポインタ側の接触点の速度(world)
	Vec3d contactPointVel;		///< 剛体側の接触点の速度(world)
	Posed interpolation_pose;	///< 形状の補間姿勢
	double f;					///< 侵入解除計算で接触点が並進移動に作用する大きさ
	float mu;
	float mu0;
	PHIntermediateRepresentation(){
		solidID = -1;
		f = 0.0;
		solidPair = NULL;
	}
};
typedef PHIntermediateRepresentation PHIr;
typedef std::vector< PHIr* > PHIrs;

//----------------------------------------------------------------------------
// PHHapticPointer
struct PHHapticPointerSt{
	PHIrs allIrs;
	Vec3d last_dr;
	Vec3d last_dtheta;
	Posed proxyPose;	// 摩擦計算用のプロキシ
	Posed lastProxyPose;
	Posed targetProxy;	// 反力計算用のプロキシ
};
class PHHapticPointer : public PHHapticPointerSt, public PHSolid{
	SPR_OBJECTDEF(PHHapticPointer);
protected:
	int pointerID;
	int pointerSolidID;
	float localRange;
	double worldScale;
	double posScale;
	Posed defaultPose;
	double rotaionalWeight;

public:
	bool bDebugControl;
	bool bForce;
	bool bVibration;
	std::vector<int> neighborSolidIDs;
	PHSolid hiSolid;
	HIBaseIf* humanInterface;
	PHHapticPointer();
	PHHapticPointer(const PHHapticPointer& p);

	void	SetPointerID(int id){ pointerID = id; }
	int		GetPointerID(){ return pointerID; }
	void	SetSolidID(int id){ pointerSolidID = id; }
	int		GetSolidID(){ return pointerSolidID; }
	void	SetHumanInterface(HIBaseIf* hi){ humanInterface = hi; }
	void	UpdateInterface(float dt);
	void	UpdateDirect();
	void	SetForce(SpatialVector f);
	void	SetLocalRange(float r){ localRange = r; } 
	float	GetLocalRange(){ return localRange; }
	void	SetPosScale(double scale){ posScale = scale; }
	double	GetPosScale(){ return posScale; }
	void	SetWorldScale(double scale){ worldScale = scale; }
	double	GetWorldScale(){ return worldScale; }
	void	SetRotationalWeight(double w){ rotaionalWeight = w; }
	double	GetRotationalWeight(){ return rotaionalWeight ; }
	void	SetDefaultPose(Posed p){ defaultPose = p; }
	Posed	GetDefaultPose(){ return defaultPose; }
	void	EnableForce(bool b){ bForce = b; }
	void	DisplayVibration(bool b){ bVibration = b; }
};
class PHHapticPointers : public std::vector< UTRef< PHHapticPointer > >{};

}
#endif
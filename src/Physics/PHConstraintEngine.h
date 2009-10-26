/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#ifndef PHCONSTRAINTENGINE_H
#define PHCONSTRAINTENGINE_H

#include "PHContactDetector.h"
#include "PHConstraint.h"
#include "PHGear.h"
#include "PHPathJoint.h"
#include "PHBallJoint.h"
#include "PH3ElementBallJoint.h"
#include <Collision/CDDetectorImp.h>

namespace Spr{;

class PHSolid;
class PHJoint;
class PHRootNode;
class PHConstraintEngine;

///	形状の組
class PHShapePairForLCP: public CDShapePair{
public:
	SPR_OBJECTDEF(PHShapePairForLCP);
	std::vector<Vec3d>	section;	///< 交差断面の頂点．個々がPHContactPointとなる．可視化のために保持
	///	接触解析．接触部分の切り口を求めて，切り口を構成する凸多角形の頂点をengineに拘束として追加する．
	void EnumVertex(PHConstraintEngine* engine, unsigned ct, PHSolid* solid0, PHSolid* solid1);
	int NSectionVertexes(){return (int)section.size();}		//(sectionの数を返す）
	Vec3d GetSectionVertex(int i){return section[i];}	//(i番目のsectionを返す）

	///	接触面積．接触形状の頂点座標から面積を計算
	double GetContactDimension(){
		Vec3d area;
		unsigned int i;
		for(i = 0; i < section.size()-2; i++){
			Vec3d vec1 = section[i+1] - section[0];
			Vec3d vec2 = section[i+2] - section[0];
			area += cross(vec1, vec2);
		}
		return area.norm() / 2;
	}

	/// 接触面の単位法線ベクトル
	Vec3d GetNormalVector(){
		Vec3d vec1 = section[1] - section[0];
		Vec3d vec2 = section[2] - section[0];
		Vec3d normal = cross(vec1, vec2);
		return normal / normal.norm();
	}

};

/// Solidの組
class PHConstraintEngine;

class PHSolidPairForLCP : public PHSolidPair<PHShapePairForLCP, PHConstraintEngine>, public Object{
public:
	SPR_OBJECTDEF(PHSolidPairForLCP);
	virtual void OnDetect(PHShapePairForLCP* cp, PHConstraintEngine* engine, unsigned ct, double dt);
	virtual void OnContDetect(PHShapePairForLCP* cp, PHConstraintEngine* engine, unsigned ct, double dt);
	int	GetContactState(int i, int j){return shapePairs.item(i, j)->state;}
	unsigned GetLastContactCount(int i, int j){return shapePairs.item(i, j)->lastContactCount;}
	Vec3d GetCommonPoint(int i, int j){return shapePairs.item(i, j)->commonPoint;}
	double GetContactDepth(int i, int j){return shapePairs.item(i, j)->depth;}
	PHShapePairForLCPIf* GetShapePair(int i, int j){return shapePairs.item(i, j)->Cast();}
};

struct PHConstraintsSt{
	std::vector<PHConstraintState> joints;
	std::vector<PHConstraintState> gears;
	std::vector<PH3ElementState> threeElements;
	
};

class PHConstraintEngine : public PHContactDetector<PHShapePairForLCP, PHSolidPairForLCP, PHConstraintEngine>{
	friend class PHConstraint;
	friend class PHShapePairForLCP;
	SPR_OBJECTDEF1(PHConstraintEngine, PHEngine);
public:
	typedef PHContactDetector<PHShapePairForLCP, PHSolidPairForLCP, PHConstraintEngine> Detector;
	int		numIter;					///< 速度更新LCPの反復回数
	int		numIterCorrection;			///< 誤差修正LCPの反復回数
	int		numIterContactCorrection;	///< 接触点の誤差修正LCPの反復回数
	double	velCorrectionRate;			///< 速度のLCPで関節拘束の誤差を修正する場合の誤差修正比率
	double	posCorrectionRate;			///< 位置のLCPで，関節拘束の誤差を修正する場合の誤差修正比率
	double  contactCorrectionRate;		///< 接触の侵入解消のための，速度のLCPでの補正比率．
	double	shrinkRate;					///< LCP初期値を前回の解に対して縮小させる比率
	double	shrinkRateCorrection;
	double	freezeThreshold;			///< 剛体がフリーズする閾値
	bool	bGearNodeReady;				///< ギアノードがうまく構成されているかのフラグ．ノードやギアを追加・削除するたびにfalseになる
	bool	bSaveConstraints;			///< SaveState, LoadStateに， constraints を含めるかどうか．本来不要だが，f, Fが変化する．
	bool	bUpdateAllState;			///< 剛体の速度，位置の全ての状態を更新する．

	PHConstraintEngine();
	~PHConstraintEngine();
	
	PHJoint* CreateJoint(const IfInfo* ii, const PHJointDesc& desc, PHSolid* lhs = NULL, PHSolid* rhs = NULL);	///< 関節の追加する
	PHRootNode* CreateRootNode(const PHRootNodeDesc& desc, PHSolid* solid = NULL);	///< ツリー構造のルートノードを作成
	PHTreeNode* CreateTreeNode(const PHTreeNodeDesc& desc, PHTreeNode* parent = NULL, PHSolid* solid = NULL);	///< ツリー構造の中間ノードを作成
	PHGear*		CreateGear(const PHGearDesc& desc, PHJoint1D* lhs = NULL, PHJoint1D* rhs = NULL);	///< ギアを作成
	PHPath*		CreatePath(const PHPathDesc& desc);
	void		UpdateGearNode();
	virtual int GetPriority() const {return SGBP_CONSTRAINTENGINE;}
	virtual void Step();			///< 
	virtual void StepPart1();		///< 
	virtual void StepPart2();		///< 
	//virtual void Dynamics(double dt, int ct);		///< 
	//virtual void Correction(double dt, int ct);		///< 
	void UpdateSolids(bool bVelOnly);	///< 結果をSolidに反映する. bVelOnly == trueならば結果の速度のみをSolidに反映させ，位置はそのまま．
	void UpdateOnlyVelocity();			///< obsolete. UpdateSolids(true)を使用のこと
	void Clear();

	PHConstraints	points;			///< 接触点の配列
	PHConstraints	joints;			///< 関節の配列
	typedef std::vector< UTRef<PHBallJoint> > PHBallJoints;
	PHBallJoints	ballJoints;		///< 球関節の配列
	typedef std::vector< UTRef<PHRootNode> > PHRootNodes;
	PHRootNodes		trees;			///< Articulated Body Systemの配列
	PHGears			gears;
	typedef std::vector< UTRef<PHPath> > PHPaths;
	PHPaths			paths;
	typedef std::vector< UTRef<PH3ElementBallJoint> > PH3ElementBallJoints;
	PH3ElementBallJoints			threeBallJoints;
	
	void SetupLCP();				///< 速度更新LCPの準備
	void IterateLCP();				///< 速度更新LCPの一度の反復
	void SetupCorrectionLCP();		///< 誤差修正LCPの準備
	void IterateCorrectionLCP();	///< 誤差修正LCPの一度の反復

	virtual PHConstraintsIf* GetContactPoints();
	virtual void	SetVelCorrectionRate(double value){velCorrectionRate = value;}
	virtual double	GetVelCorrectionRate(){return velCorrectionRate;}
	virtual void	SetPosCorrectionRate(double value){posCorrectionRate = value;}
	virtual double	GetPosCorrectionRate(){return posCorrectionRate;}
	virtual void	SetContactCorrectionRate(double value){contactCorrectionRate = value;}
	virtual double	GetContactCorrectionRate(){return contactCorrectionRate;}
	virtual void	SetBSaveConstraints(bool value){bSaveConstraints = value;}
	virtual void	SetUpdateAllSolidState(bool flag){bUpdateAllState = flag;}
	virtual void	SetShrinkRate(double data){shrinkRate = data;}
	virtual double	GetShrinkRate(){return shrinkRate;}
	virtual void	SetShrinkRateCorrection(double data){shrinkRateCorrection = data;}
	virtual double	GetShrinkRateCorrection(){return shrinkRateCorrection;}
	virtual bool	AddChildObject(ObjectIf* o);
	virtual bool	DelChildObject(ObjectIf* o);

	virtual size_t	GetStateSize() const;
	virtual void	ConstructState(void* m) const;
	virtual void	DestructState(void* m) const ;
	virtual bool	GetState(void* s) const ;
	virtual void	SetState(const void* s);
};

}	//	namespace Spr
#endif

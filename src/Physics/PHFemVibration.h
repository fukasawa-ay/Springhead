/*
 *  Copyright (c) 2003-2012, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */

#ifndef PH_FEM_VIBRATION_H
#define PH_FEM_VIBRATION_H

#include <Physics/PHFemMeshNew.h>
#include <Foundation/Object.h>

namespace Spr{;

class PHFemVibration : public PHFemVibrationDesc, public PHFem{
public:
	SPR_OBJECTDEF(PHFemVibration);
	ACCESS_DESC(PHFemVibration);
	typedef VMatrixRow< double > VMatrixRd;
	typedef VVector< double > VVectord;

	double vdt;
	PHFemVibrationDesc::INTEGRATION_MODE integration_mode;
	VMatrixRd matKIni;	// 全体剛性行列の初期値
	VMatrixRd matMIni;	// 全体質量行列の初期値
	VMatrixRd matMInv;	// 全体質量行列の逆行列
	VMatrixRd matCIni;	// 全体減衰行列の初期値
	VVectord xdl;		// 頂点変位(u = (x1, ..., xn-1, y1, ..., yn-1, z1, ..., zn-1)
	VVectord vl;		// 頂点速度
	VVectord fl;		// 計算用の外力

	PHFemVibration(const PHFemVibrationDesc& desc = PHFemVibrationDesc());
	/// 初期化
	virtual void Init();
	//virtual void CompStiffnessMatrix();
	//virtual void CompMassMatrix();
	//virtual void CompDampingMatrix();
	/// シミュレーションステップ
	virtual void Step();

	/// 時間積分
	/// _M:質量行列、_K:剛性行列、_C:減衰行列、_f:外力、_dt:積分刻み、_xd:変位、_v:速度
	virtual void ExplicitEuler(const VMatrixRd& _MInv, const VMatrixRd& _K, const VMatrixRd& _C, 
		const VVectord& _f, const double& _dt, VVectord& _xd, VVectord& _v);
	virtual void ImplicitEuler(const VMatrixRd& _MInv, const VMatrixRd& _K, const VMatrixRd& _C, 
		const VVectord& _f, const double& _dt, VVectord& _xd, VVectord& _v);
	virtual void NewmarkBeta(const VMatrixRd& _M, const VMatrixRd& _K, const VMatrixRd& _C, 
		const VVectord& _f, const double& _dt, VVectord& _xd, VVectord& _v, const double b = 1.0 /6.0);
	virtual void ModalAnalysis(const VMatrixRd& _M, const VMatrixRd& _K, const VMatrixRd& _C, 
		const VVectord& _f, const double& _dt, VVectord& _xd, VVectord& _v, const int nmode);

	/// 固有値解析
	virtual void SubSpace(const VMatrixRd& _M, const VMatrixRd& _K, 
							const int nmode, const double epsilon, VVectord& e, VMatrixRd& v);

	/// 各種設定
	void SetTimeStep(double dt){ vdt = dt; }
	double GetTimeStep(){ return vdt; }
	void SetYoungModulus(double value){ young = value; }
	double GetYoungModulus(){ return young; }
	void SetPoissonsRatio(double value){ poisson = value; }
	double GetPoissonsRatio(){ return poisson; }
	void SetDensity(double value) { density = value; }
	double GetDensity(){ return density; }
	void SetAlpha(double value){ alpha = value; }
	double GetAlpha(){ return alpha; }
	void SetBeta(double value){ beta = value; }
	double GetBeta(){ return beta; }
	void SetIntegrationMode(PHFemVibrationDesc::INTEGRATION_MODE mode);

	// FemVertexから頂点変位を取得し、計算できる形に変換する
	void GetVerticesDisplacement(VVectord& _xd);
	// FemVertexに頂点変位を加える
	void UpdateVerticesPosition(VVectord& _xd);

	// 境界条件を加える(行列と番号）
	bool AddBoundaryCondition(VMatrixRd& mat, const int id);
	// 境界条件を加える(頂点）
	bool AddBoundaryCondition(const int vtxId, const Vec3i dof);
	// 境界条件を加える(頂点順）
	bool AddBoundaryCondition(const VVector< Vec3i > bcs); 

	// 頂点に力を加える（ワールド座標系）
	bool AddVertexForce(int vtxId, Vec3d fW);
	// 頂点群に力を加える（ワールド座標系）
	bool AddVertexForce(VVector< Vec3d > fWs);

	// scilabデバック
	bool IsScilabStarted;
	//void ScilabDeterminant(VMatrixRd& _M, VMatrixRd& _K, VMatrixRd& _K);
	void ScilabEigenValueAnalysis(VMatrixRd& _M, VMatrixRd& _K);
};

}

#endif
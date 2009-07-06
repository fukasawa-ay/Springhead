/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
/**
 *	@file GRDebugRender.h
 *	@brief デバッグ情報レンダラー（剛体、面）　　
*/
#ifndef GRDEBUGRENDER_H
#define GRDEBUGRENDER_H

#include "GRRender.h"

namespace Spr{;

/**	@class	GRDebugRender
    @brief	デバッグ情報レンダラーの実装　 */
class GRDebugRender:public GRRender{
	SPR_OBJECTDEF(GRDebugRender);
protected:
	int matSampleCount;
	std::vector<GRMaterialDesc> matSample;		/// レンダラーで用意してある材質(24種類)

	bool	modeSolid, modeWire;				/// 描画モード
	bool	renderWorldAxis, renderAxis, renderForce, renderContact, renderGrid, renderIK;
	float	scaleAxis, scaleForce, scaleIK;
	double  gridY, gridSpan;

public:
	/**  コンストラクタ  */
	GRDebugRender();
	virtual void DrawScene(PHSceneIf* scene);
	virtual void DrawWorldAxis(PHSceneIf* scene);
	virtual void DrawGrid(double y, double span);
	virtual void DrawSolid(PHSolidIf* so);
	virtual void DrawConstraint(PHConstraintIf* con);
	virtual void DrawContact(PHContactPointIf* con);
	virtual void DrawIK(PHIKEngineIf* ikEngine);
	virtual void SetMaterialSample(GRDebugRenderIf::TMaterialSample matname);
	virtual void SetRenderMode(bool solid = true, bool wire = false){ modeSolid = solid; modeWire = wire; }
	virtual void EnableRenderWorldAxis( bool enable =  true){
		renderWorldAxis = enable;
	}
	virtual void EnableRenderAxis(bool enable = true, float scale = 1.0f){
		renderAxis = enable;
		scaleAxis = scale;
	}
	virtual void EnableRenderForce(bool enable = true, float scale = 1.0f){
		renderForce = enable;
		scaleForce = scale;
	}
	virtual void EnableRenderContact(bool enable = true){ renderContact = enable; }

	virtual void EnableGrid(bool enable = true, double y = 0.0, double span = 0.5){
		renderGrid = enable; gridY = y; gridSpan = span;
	}

	virtual void EnableRenderIK(bool enable = true, float scale = 1.0f) {
		renderIK = enable;
		scaleIK = scale;
	}

	/// カプセルの描画
	void DrawCapsule(CDCapsuleIf* cap, bool solid);
	/// 不均等カプセル (RoundCone)の描画
	void DrawRoundCone(CDRoundConeIf* rc, bool solid);
	/// メッシュの描画
	void DrawMesh(CDConvexMeshIf* mesh, bool solid);
	/// 座標軸の描画
	void DrawAxis(bool solid);
	void DrawCoordinateAxis(bool solid);
	/// 線分の描画
	void DrawLine(const Vec3d& p0, const Vec3d& p1);
	/// 面の描画
	void DrawFaceSolid(CDFaceIf* face, Vec3f * base);
	void DrawFaceWire(CDFaceIf* face, Vec3f * base);
	
	
};

}
#endif

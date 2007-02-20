/*
 *  Copyright (c) 2003-2006, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
/**
 *	@file SprGRFrame.h
 *	@brief シーングラフのノード
*/

/**	\addtogroup	gpGraphics	*/
//@{

#ifndef SPR_GRFrame_H
#define SPR_GRFrame_H
#include <SprFoundation.h>

namespace Spr{;

struct GRRenderIf;

/** @brief ビジュアルのディスクリプタ */
struct GRVisualDesc{
	enum VisualType{
		FRAME,
		MESH,
		MATERIAL,
		CAMERA,
		LIGHT
	} type;
};

/**	@brief	グラフィックスで表示に影響を与えるもの	*/
struct GRVisualIf: public NamedObjectIf{
	IF_DEF(GRVisual);
	///	レンダリング．子ノード，弟ノードのレンダリングより前に呼ばれる．
	virtual void Render(GRRenderIf* r)=0;
	///	レンダリング終了処理．子ノード，弟ノードのレンダリングが終わってから呼ばれる．
	virtual void Rendered(GRRenderIf* r)=0;
};
		
///	DirectX の Frame の変換を表す FrameTransformMatrix ノードを読むためのDesc
struct GRFrameTransformMatrix{
	Affinef transform;
};

///	@brief GRFrame のDesc．座標系を指定する
struct GRFrameDesc : GRVisualDesc{
	Affinef transform;
	GRFrameDesc(){ type = FRAME; }
};
	
/**	@brief	グラフィックスシーングラフのツリーのノード．座標系を持つ．*/
struct GRFrameIf: public GRVisualIf{
	IF_DEF(GRFrame);

	/** @brief 親フレームを取得する */
	virtual GRFrameIf* GetParent()=0;

	/** @brief 親フレームを設定する */
	virtual void SetParent(GRFrameIf* fr)=0;

	/** @brief 子ノードの数を取得 */
	virtual int NChildren() = 0;

	/** @brief 子ノードの配列を取得 */
	virtual GRVisualIf** GetChildren() = 0;

	/** @brief 親ノードとの相対変換を取得 */
	virtual Affinef GetTransform()=0;

	/** @brief ワールドフレームとの相対変換を取得 */
	virtual Affinef GetWorldTransform()=0;

	/** @brief 親ノードとの相対変換を設定 */
	virtual void SetTransform(Affinef& af)=0;

	virtual void Print(std::ostream& os) const =0;
};

//@}
}
#endif

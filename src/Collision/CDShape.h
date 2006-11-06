/*
 *  Copyright (c) 2003-2006, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#ifndef PHSHAPE_H
#define PHSHAPE_H

#include <SprCollision.h>
#include <Foundation/Object.h>

namespace Spr{;

/// 形状
/// 形状自体は位置情報を持たない．位置情報は形状を参照する各剛体が持つ．
class CDShape : public InheritNamedObject<CDShapeIf, NamedObject>{
public:
	PHMaterial material;

	OBJECT_DEF_ABST(CDShape);
	//Posef GetPose() const { return pose; }
	//void SetPose(const Posef& p){ pose = p; }
	
	virtual void CalcBBox(Vec3f& bbmin, Vec3f& bbmax, const Posed& pose)=0;
};

/// 形状への参照と剛体に対する相対位置
struct CDShapeRefWithPose{
	CDShape* shape;
	Posed pose;
};

template <class intf, class base>
struct InheritCDShape:public InheritNamedObject<intf, base>{
	//void SetPose(const Posef& p){ base::SetPose(p); }
	//Posef GetPose() const { return base::GetPose(); }
};

}	//	namespace Spr
#endif

/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */

#ifndef SPR_HI_XBOX360CONTROLLER_H
#define SPR_HI_XBOX360CONTROLLER_H

#include <HumanInterface/SprHIBase.h>

namespace Spr{;

struct HIXbox360ControllerIf : public HIHapticIf{
	SPR_VIFDEF(HIXbox360Controller);
	bool Init();
};

struct HIXbox360ControllerDesc{
	SPR_DESCDEF(HIXbox360Controller);
};
}

#endif

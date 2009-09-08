#ifndef FWINTERACT_SCENE_H
#define FWINTERACT_SCENE_H

#include <Framework/SprFWInteractScene.h>
#include <Foundation/Scene.h>
#include <Framework/FWInteractPointer.h>
#include <Framework/FWInteractSolid.h>
#include <Framework/FWInteractAdaptee.h>

namespace Spr{;

class SPR_DLL FWInteractScene : public Scene, public FWInteractSceneDesc{
protected:
	FWInteractPointerIf*		curINPointer;
	FWInteractPointers			interactPointers;
	FWInteractSolids			interactSolids;
	UTRef<FWInteractAdaptee>	interactAdaptee;
public:
	SPR_OBJECTDEF(FWInteractScene);
	FWInteractScene();
	FWInteractScene(const FWInteractSceneDesc& desc);

	FWSceneIf* GetScene();

	/// Adaptee関係
	void					CreateINAdaptee(FWInteractMode inMode);
	FWInteractAdaptee*		GetINAdaptee();
	FWInteractMode			GetINMode();
	FWHapticMode			GetHMode();
	void					SetHMode(FWHapticMode hMode);

	/// INPointer関係
	FWInteractPointerIf*	CreateINPointer(const FWInteractPointerDesc& desc);
	FWInteractPointerIf*	GetINPointer(int i = -1);
	FWInteractPointers*		GetINPointers();
	int						NINPointers();

	/// INSolid関係
	FWInteractSolid*		GetINSolid(int i);
	FWInteractSolids*		GetINSolids();
	int						NINSolids();


	/// 以下の関数はadapteeをとおして実装が呼ばれる
	void Init();
	void Clear();
	void Step();
	void CallBackHapticLoop();
	void BeginKeyboard();
	void EndKeyboard();

};


}
#endif
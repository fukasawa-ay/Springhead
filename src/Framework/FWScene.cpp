#include "FWScene.h"
#include "FWObject.h"
#include <Graphics/GRDebugRender.h>
#include <Graphics/GRScene.h>
#include <Physics/PHScene.h>

#ifdef USE_HDRSTOP
#pragma hdrstop
#endif

namespace Spr{;

IF_OBJECT_IMP(FWScene, Scene);

FWScene::FWScene(const FWSceneDesc& d/*=FWSceneDesc()*/)
: phScene(NULL), grScene(NULL)
{
}
NamedObjectIf* FWScene::FindObject(UTString name, UTString cls){
	//	余分にphScene,grSceneの2つのシーンを検索するので，NameManager::FindObjectとちょっと違う．
	//	2つのシーンは，Sdkに所有されているので，普通にNameManagerとして，FWSceneを指定することはできない．

	//	まず自分と子孫を検索
	NamedObjectIf* rv = FindObjectFromDescendant(name, cls);
	if (rv) return rv;
	//	先祖を検索
	rv = FindObjectFromAncestor(name, cls);
	if (rv) return rv;

	//	なければ，phSceneとgrSceneについて自分と子孫を探す。
	rv = DCAST(PHScene, phScene)->FindObjectFromDescendant(name, cls);
	if (rv) return rv;
	rv = DCAST(GRScene, grScene)->FindObjectFromDescendant(name, cls);
	if (rv) return rv;

	//	それでもないならば、namespaceを削って、もう一度検索
	size_t pos = name.find('/');
	if (pos != UTString::npos){	//	 名前空間の指定がある場合
		UTString n = name.substr(pos+1);
		rv = FindObject(n, cls);
	}
	return rv;
}

ObjectIf* FWScene::CreateObject(const IfInfo* info, const void* desc){
	ObjectIf* rv = baseType::CreateObject(info, desc);
	if (!rv && phScene) rv = phScene->CreateObject(info, desc);
	if (!rv && grScene) rv = grScene->CreateObject(info, desc);
	return rv;
}
bool FWScene::AddChildObject(ObjectIf* o){
	bool rv = false;
	if (!rv) {
		FWObjectIf* obj = DCAST(FWObjectIf, o);
		if (obj) {
			fwObjects.push_back(obj);
			rv = true;
		}
	}
	if (!rv) {
		PHScene* obj = DCAST(PHScene, o);
		if (obj) {
			phScene = obj;
			rv = true;
		}
	}
	if (!rv) {
		GRScene* obj = DCAST(GRScene, o);
		if (obj) {
			grScene = obj;
			rv = true;
		}
	}
	if (!rv && phScene) {
		rv = phScene->AddChildObject(o);
	}
	if (!rv && grScene) {
		rv = grScene->AddChildObject(o);
	}
	return rv;
}

HIForceDevice6D* FWScene::GetHumanInterface(size_t pos){
	if (pos < humanInterfaces.size()) return humanInterfaces[pos];
	return NULL;
}

size_t FWScene::NChildObject() const{
	return fwObjects.size() + (grScene?1:0) + (phScene?1:0);
}

ObjectIf* FWScene::GetChildObject(size_t pos){
	if (pos < fwObjects.size()) return fwObjects[pos];
	if (pos - fwObjects.size() == 0) return phScene ? (ObjectIf*)phScene : (ObjectIf*)grScene;
	if (pos - fwObjects.size() == 1) return phScene ? grScene : NULL;
	return NULL;
}

FWSceneIf* SPR_CDECL CreateFWScene(){
	FWSceneIf* rv = DBG_NEW FWScene;
	return rv;
}

FWSceneIf* SPR_CDECL CreateFWScene(const void* desc){
	FWSceneIf* rv = DBG_NEW FWScene(*(FWSceneDesc*)desc);
	return rv;
}
void FWScene::Sync(){
	//	オブジェクト位置・姿勢の同期
	for(FWObjects::iterator it = fwObjects.begin(); it!=fwObjects.end(); ++it){
		DCAST(FWObject, *it)->Sync();
	}
	
	//	カメラの同期
	HIForceDevice6D* device = GetHumanInterface(HI_CAMERACONTROLLER);
	GRCameraIf* camera = grScene->GetCamera();
	if (grScene && camera && device){
		Posed pose;
		pose.Pos() = device->GetPos();
		pose.Ori() = device->GetOri();
		if (!camera->GetFrame()){
			GRSceneIf* scene = DCAST(GRSceneIf, camera->GetNameManager());
			if (scene) camera->SetFrame(scene->CreateFrame(GRFrameDesc()));
		}
		if (camera->GetFrame()){
			Affinef af;
			pose.ToAffine(af);
			camera->GetFrame()->SetTransform(af);
		}
	}
}
void FWScene::Step(){
	phScene->Step();
}
void FWScene::Draw(GRRenderIf* grRender, bool debug/*=false*/){
	Sync();
	if (debug){
		GRDebugRenderIf* render = DCAST(GRDebugRenderIf, grRender);
		if (render){
			GRCameraIf* cam = NULL;
			if (grScene) cam = grScene->GetCamera();
			if (cam) cam->Render(render);
			
			GRLightDesc ld;
			ld.diffuse = Vec4f(1,1,1,1) * 0.8f;
			ld.specular = Vec4f(1,1,1,1) * 0.8f;
			ld.ambient = Vec4f(1,1,1,1) * 0.4f;
			ld.position = Vec4f(1,1,1,0);

			render->PushLight(ld);
			render->DrawScene(phScene);
			render->PopLight();
			if (cam) cam->Rendered(render);
		}
	}else{
		grScene->Render(grRender);
	}
}

}

/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#include "Framework.h"
#include "Framework/FWSdk.h"
#include "Framework/FWOldSpringheadNode.h"
#include "Physics/PHSdk.h"
#include "Physics/PHScene.h"
#include "Graphics/GRSdk.h"
#include "Graphics/GRScene.h"
#ifdef USE_HDRSTOP
#pragma hdrstop
#endif
#include <stdlib.h>
#include <GL/glut.h>

using namespace std;

namespace Spr{;

void SPR_CDECL FWRegisterTypeDescs();
void SPR_CDECL FWSdkIf::RegisterSdk(){
	static bool bFirst = true;
	if (!bFirst) return;
	bFirst=false;
	FWRegisterTypeDescs();
	FWRegisterOldSpringheadNode();

	FWSdkIf::GetIfInfoStatic()->RegisterFactory(DBG_NEW FactoryImp(FWScene));
	FWSceneIf::GetIfInfoStatic()->RegisterFactory(DBG_NEW FactoryImp(FWObject));
	FWSceneIf::GetIfInfoStatic()->RegisterFactory(DBG_NEW FactoryImp(PHSdk));
	FWSceneIf::GetIfInfoStatic()->RegisterFactory(DBG_NEW FactoryImp(PHScene));
	FWSceneIf::GetIfInfoStatic()->RegisterFactory(DBG_NEW FactoryImp(GRSdk));
	FWSceneIf::GetIfInfoStatic()->RegisterFactory(DBG_NEW FactoryImp(GRScene));
}

FWSdkIf* SPR_CDECL FWSdkIf::CreateSdk(){
	FWSdk* rv = DBG_NEW FWSdk;
	FWSdkIf::RegisterSdk();
	PHSdkIf::RegisterSdk();
	GRSdkIf::RegisterSdk();
	return rv->Cast();
}

//----------------------------------------------------------------------------
//	FWSdk
FWSdk::FWSdk(){
	name="fwSdk";
	CreateSdks();
	curScene = NULL;
	curRender = NULL;
	debugMode = false;
}
void FWSdk::CreateSdks(){
	phSdk = PHSdkIf::CreateSdk();
	DCAST(PHSdk, phSdk)->SetNameManager(this);
	phSdk->SetName("phSdk");
	grSdk = GRSdkIf::CreateSdk();
	DCAST(GRSdk, grSdk)->SetNameManager(this);
	grSdk->SetName("grSdk");
	fiSdk = FISdkIf::CreateSdk();
}

FWSdk::~FWSdk(){
}
FWSceneIf* FWSdk::CreateScene(const PHSceneDesc& phdesc, const GRSceneDesc& grdesc){
	FWSceneDesc desc;
	FWSceneIf* scene = DCAST(FWSceneIf, CreateObject(FWSceneIf::GetIfInfoStatic(), &desc));
	scene->SetPHScene(GetPHSdk()->CreateScene(phdesc));
	scene->SetGRScene(GetGRSdk()->CreateScene(grdesc));
	AddChildObject(scene);
	return scene;
}
bool FWSdk::LoadScene(UTString filename){
	//	デフォルトの先祖オブジェクトをを設定
	//	これらのCreateObjectが呼ばれてシーングラフが作られる。
	ObjectIfs objs;
	objs.Push(GetGRSdk());	//	GRSdk
	objs.Push(GetPHSdk());	//	PHSdk
	//	FWSdk	FWScene は FWSdkの子になるので、FWSdkを最後にPushする必要がある。
	objs.Push(Cast());
	int first = NScene();	//	ロードされるFWSceneの位置を覚えておく

	//	ファイルローダーの作成
	UTRef<FIFileXIf> fiFileX = GetFISdk()->CreateFileX();
	//	ファイルのロード
	if (! fiFileX->Load(objs, filename.data()) ) {
		DSTR << "Error: Cannot load file " << filename.c_str() << std::endl;
		//exit(EXIT_FAILURE);
		return false;
	}
	//	ロードしたシーンを取得
	DSTR << "Loaded " << NScene() - first << " scenes." << std::endl;
	DSTR << "LoadFile Complete." << std::endl;
	for(int i=first; i<NScene(); ++i){
		curScene = GetScene(i);
		curScene->Print(DSTR);
	}
	return true;
}
bool FWSdk::SaveScene(UTString filename){
	// 保存
	ObjectIfs objs;
	for(unsigned int i=0; i<scenes.size(); ++i){
		objs.push_back(scenes[i]->Cast());
	}
	UTRef<FIFileXIf> fiFileX = GetFISdk()->CreateFileX();
	if(!fiFileX->Save(objs, filename.c_str())){
		DSTR << "Error: Cannot save file " << filename.c_str() << std::endl;
		return false;
	}
	return true;
}

FWSceneIf* FWSdk::GetScene(int i){
	if(i == -1)return curScene;
    if(0 <= i && i < NScene())
		return scenes[i];
	return NULL;
}
void FWSdk::MergeScene(FWSceneIf* scene0, FWSceneIf* scene1){
	if(scene0 == scene1)
		return;
	DSTR << "merging " << scene0->GetName() << " and " << scene1->GetName() << endl;
	
	Scenes::iterator it0, it1;
	it0 = find(scenes.begin(), scenes.end(), scene0);
	it1 = find(scenes.begin(), scenes.end(), scene1);
	if(it0 == scenes.end() || it1 == scenes.end())
		return;
	// PHSceneのマージ
	if(scene0->GetPHScene()){
		if(scene1->GetPHScene())
			GetPHSdk()->MergeScene(scene0->GetPHScene(), scene1->GetPHScene());
	}
	else if(scene1->GetPHScene())
		scene0->SetPHScene(scene1->GetPHScene());
	// GRSceneのマージ
	if(scene0->GetGRScene()){
		if(scene1->GetGRScene())
			GetGRSdk()->MergeScene(scene0->GetGRScene(), scene1->GetGRScene());
	}
	else if(scene1->GetGRScene())
		scene0->SetGRScene(scene1->GetGRScene());

	// FWObjectのマージ
	for(int i = 0; i < scene1->NObject(); i++){
		scene0->AddChildObject(scene1->GetObjects()[i]);
	}
	if(curScene == scene1)
		curScene = scene0;

	scenes.erase(it1);

}

GRRenderIf*	FWSdk::CreateRender(){
	GRRenderIf* render = GetGRSdk()->CreateDebugRender();
	GRDeviceIf* dev = GetGRSdk()->CreateDeviceGL();
	dev->Init();
	render->SetDevice(dev);

	//	仮の視点。このあとうまくシーンが設定されれば、シーンのカメラに上書きされる。
	Affinef view;
	view.Pos() = Vec3f(0.0, 3.0, 3.0);
	view.LookAtGL(Vec3f(0.0, 0.0, 0.0), Vec3f(0.0, 1.0, 0.0));
	view = view.inv();		
	render->SetViewMatrix(view);

	renders.push_back(render);
	curRender = render;
	return render;
}

GRRenderIf* FWSdk::GetRender(int i){
	if(i == -1)return curRender;
    if(0 <= i && i < NRender())
		return renders[i];
	return NULL;
}

bool FWSdk::AddChildObject(ObjectIf* o){
	FWSceneIf* s = DCAST(FWSceneIf, o);
	if (s){
		if (std::find(scenes.begin(), scenes.end(), s) == scenes.end()){
			scenes.push_back(s);
			curScene = s;
			return true;
		}
	}
	PHSdkIf* ps = DCAST(PHSdkIf, o);
	if (ps) {
		phSdk = ps;
		return true;
	}
	GRSdkIf* gs = DCAST(GRSdkIf, o);
	if (gs) {
		grSdk = gs;
		return true;
	}
	return false;
}

bool FWSdk::DelChildObject(ObjectIf* o){
	FWSceneIf* s = DCAST(FWSceneIf, o);
	if(s){
		Scenes::iterator it = std::find(scenes.begin(), scenes.end(), s);
		if(it != scenes.end()){
			scenes.erase(it);
			if(curScene == s)
				curScene = (scenes.empty() ? NULL : scenes[0]);
			return true;
		}
	}
	return false;
}

void FWSdk::Clear(){
	// 一度全てをクリアしてSDKを作り直す
	Sdk::Clear();
	phSdk = NULL;
	grSdk = NULL;
	fiSdk = NULL;
	scenes.clear();
	renders.clear();
	curScene = NULL;
	curRender = NULL;
	CreateSdks();
}
void FWSdk::Step(){
	if (curScene)
		curScene->Step();
}

void FWSdk::Draw(){
	if(!curRender)return;
	curRender->ClearBuffer();
	curRender->BeginScene();
	if (curScene)
		curScene->Draw(curRender, debugMode);
	curRender->EndScene();
}
void FWSdk::Reshape(int w, int h){
	if (curRender)
		curRender->Reshape(Vec2f(), Vec2f(w,h));
}

}

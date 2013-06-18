/*
 *  Copyright (c) 2003-2012, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */

#include <Graphics/GRMesh.h>
#include <Foundation/UTLoadHandler.h>
#include "FWFemMeshNew.h"
#include "FWSprTetgen.h"

#ifdef USE_HDRSTOP
#pragma hdrstop
#endif

#ifdef _DEBUG
# define PDEBUG(x)	x
#else
# define PDEBUG(x)
#endif

namespace Spr{;

FWFemMeshNew::FWFemMeshNew(const FWFemMeshNewDesc& d){
	grFemMesh = NULL;
	// p: piecewise linear comlex, q:2.1が正四面体の歪み(1以上〜？以下）、a:粗さ
	//meshRoughness = "pq2.1a0.002";
	//meshRoughness = "pq2.1a0.01";
	//meshRoughness = "pq2.1a1.0";
	meshRoughness = "pq2.1a0.01";//cube
		//meshRoughness = "pq3.1a0.1";//cube飛んでく
	//meshRoughness = "pq2.1a0.00005";
	//meshRoughness = "pq2.1a0.02";
	//eshRoughness = "pq2.1a0.1";
	SetDesc(&d);
	texture_mode = 2;		//	テクスチャ表示の初期値：温度
}

size_t FWFemMeshNew::NChildObject() const{
	return FWObject::NChildObject() + (grFemMesh ? 1 : 0) + (phFemMesh ? 1 : 0);
}

ObjectIf* FWFemMeshNew::GetChildObject(size_t pos){
	if(pos < FWObject::NChildObject()){
		return FWObject::GetChildObject(pos);
	}else{
		pos -= FWObject::NChildObject();
	}	
	if(grFemMesh){
		if (pos == 0) return grFemMesh->Cast();
		else pos --;
	}
	if(phFemMesh){
		if (pos == 0) return phFemMesh->Cast();
		else pos --;
	}
	return NULL;
}

bool FWFemMeshNew::AddChildObject(ObjectIf* o){
	PHFemMeshNew* pm = o->Cast();
	if (pm){
		phFemMesh = pm;
		phFemMesh->SetPHSolid(GetPHSolid());
		return true;
	}
	GRMesh* mesh = o->Cast();
	if (mesh){
		grFemMesh = mesh;
		return true;
	}
	return FWObject::AddChildObject(o);
}

void FWFemMeshNew::Loaded(UTLoadContext*){
	if(grFemMesh){
		if(!phFemMesh){
			// phFemMeshがNULLの場合、新たに生成する。
			// 計算モジュールは作られないので別に作る必要がある（未実装）。
			phFemMesh = DBG_NEW PHFemMeshNew();
			// PHFemEngineに追加登録	
			if(GetPHSolid() && GetPHSolid()->GetScene()){
				GetPHSolid()->GetScene()->AddChildObject(phFemMesh->Cast());
				phFemMesh->SetPHSolid(GetPHSolid());
			}
			CreatePHFemMeshFromGRMesh();		
		}else if(phFemMesh->vertices.size() == 0){
			// phFemMeshの頂点が空の場合
			// tetgenによるメッシュの生成
			CreatePHFemMeshFromGRMesh();
		}
	}

	if (grFrame){
		grFrame->DelChildObject(grFemMesh->Cast());
		CreateGRFromPH();
		grFrame->AddChildObject(grFemMesh->Cast());
	}

	phFemMesh->Init();
}

bool FWFemMeshNew::CreatePHFemMeshFromGRMesh(){
	//	呼び出された時点でgrFemMesh にグラフィクスのメッシュが入っている。
	//	grFemMeshを四面体メッシュに変換して、phFemMeshをつくる。
	
	//TetGenで四面体メッシュ化
	Vec3d* vtxsOut=NULL;
	int* tetsOut=NULL;
	int nVtxsOut=0, nTetsOut=0;
	std::vector<Vec3d> vtxsIn;
	for(unsigned i = 0; i < grFemMesh->vertices.size(); ++i) vtxsIn.push_back(grFemMesh->vertices[i]);
	// swithes q+(半径/最短辺) (e.g. = q1.0~2.0) a 最大の体積 
	sprTetgen(nVtxsOut, vtxsOut, nTetsOut, tetsOut, (int)grFemMesh->vertices.size(), &vtxsIn[0], (int)grFemMesh->faces.size(), &grFemMesh->faces[0], meshRoughness);
	
	//	phFemMesh用のディスクリプタpmdに値を入れていく
	PHFemMeshNewDesc pmd;
	for(int i=0; i < nVtxsOut; i++){
		pmd.vertices.push_back(vtxsOut[i]);
	} 
	pmd.tets.assign(tetsOut, tetsOut + nTetsOut*4);
	phFemMesh->SetDesc(&pmd);

	return true;
}

struct FaceMap{
	FaceMap(){
		vtxs[0] = vtxs[1] = vtxs[2] = -1;
		face = -1;
	}
	int vtxs[3];
	int& operator [](int i) {return vtxs[i]; }
	int face;
};

void FWFemMeshNew::CreateGRFromPH(){
	// 3Dテクスチャを使えるようにするために、grFemMeshを再構築する？
	// 3Dテクスチャを使わない場合は必要ない？

	//	頂点の対応表を用意
	std::vector<int> vtxMap;
	vtxMap.resize(phFemMesh->vertices.size(), -1);
	for(unsigned i=0; i<phFemMesh->surfaceVertices.size(); ++i){
		vtxMap[phFemMesh->surfaceVertices[i]] = i;
	}
	GRMeshDesc gmd;	//	新しいGRMeshのデスクリプタ
	//	表面の頂点を設定
	for(unsigned i=0; i< phFemMesh->surfaceVertices.size(); ++i) 
		gmd.vertices.push_back(phFemMesh->vertices[ phFemMesh->surfaceVertices[i] ].pos);
	//	表面の三角形を設定
	for(unsigned i=0; i< phFemMesh->nSurfaceFace; ++i) {
		GRMeshFace f;
		f.nVertices = 3;
		for(int j=0; j<3; ++j){
			f.indices[j] = vtxMap[phFemMesh->faces[i].vertexIDs[j]];
		}
		gmd.faces.push_back(f);
	}
	//	phFemMeshの三角形とgrFemMeshの三角形の対応表をつくる	重なっている面積が最大のものが対応する面
	//	まず、法線が近いものを探し、面1と面2上の頂点の距離が近いものに限り、重なっている面積を求める。
	std::vector<Vec3f> pnormals(gmd.faces.size());
	for(unsigned pf=0; pf<gmd.faces.size(); ++pf){
		assert(gmd.faces[pf].nVertices == 3);
		pnormals[pf] = ((gmd.vertices[gmd.faces[pf].indices[2]] - gmd.vertices[gmd.faces[pf].indices[0]]) %
		(gmd.vertices[gmd.faces[pf].indices[1]] - gmd.vertices[gmd.faces[pf].indices[0]])).unit();
	}
	std::vector<Vec3f> gnormals(grFemMesh->faces.size());
	struct FaceWall{
		Vec3f wall[4];
	};
	std::vector<FaceWall> gWalls(gnormals.size());
	for(unsigned gf=0; gf<gnormals.size(); ++gf){
		gnormals[gf] = ((grFemMesh->vertices[grFemMesh->faces[gf].indices[2]] - grFemMesh->vertices[grFemMesh->faces[gf].indices[0]]) %
			(grFemMesh->vertices[grFemMesh->faces[gf].indices[1]] - grFemMesh->vertices[grFemMesh->faces[gf].indices[0]])).unit();
		int nv = grFemMesh->faces[gf].nVertices;
		for(int i=0; i<nv; ++i){
			gWalls[gf].wall[i] = ((grFemMesh->vertices[grFemMesh->faces[gf].indices[(i+1)%nv]] - grFemMesh->vertices[grFemMesh->faces[gf].indices[i]]) % gnormals[gf]).unit();
		}
	}
	std::vector< FaceMap > pFaceMap(pnormals.size());
	for(unsigned pf=0; pf<pnormals.size(); ++pf){
		//	物理の平面(pf)と同一平面に載っているグラフィクスの平面を列挙
		std::vector<int> gfCands;
		for(unsigned gf=0; gf<gnormals.size(); ++gf){
			if (pnormals[pf] * gnormals[gf] > 0.999){	//	法線が遠いのはだめ
				int pv;
				for(pv=0; pv<3; ++pv){
					double d = gnormals[gf] * (gmd.vertices[gmd.faces[pf].indices[pv]] - grFemMesh->vertices[grFemMesh->faces[gf].indices[0]]);
					if (d*d > 1e-8) break;	//	距離が離れすぎているのはだめ
				}
				if (pv==3) gfCands.push_back(gf);
			}
		}
		//	pfの各頂点に対応するgfを見つける
		std::vector<int> gfForPv[3];
		for(unsigned pv=0; pv<3; ++pv){
			for(unsigned i=0; i<gfCands.size(); ++i){
				int gf = gfCands[i];
				int gv;
				for(gv=0; gv<grFemMesh->faces[gf].nVertices; ++gv){
					double d = gWalls[gf].wall[gv] * (gmd.vertices[gmd.faces[pf].indices[pv]] - grFemMesh->vertices[grFemMesh->faces[gf].indices[gv]]);
					if (d < -1e-6) break;
				}
				if (gv == grFemMesh->faces[gf].nVertices){
					gfForPv[pv].push_back(gf);
				}
			}
		}
		//	３物理頂点が、１つのグラフィクス三角形を共有する場合、共有するものを優先
		std::vector< std::pair<int, int> > votes;
		for(int i=0; i<3; ++i){
			for(unsigned j=0; j<gfForPv[i].size(); ++j){
				unsigned k;
				for(k=0; k<votes.size(); ++k){
					if (votes[k].second == gfForPv[i][j]){
						votes[k].first ++;
						break;
					}
				}
				if (k==votes.size()) votes.push_back(std::make_pair(1, gfForPv[i][j]));
			}
		}
		std::sort(votes.begin(), votes.end(), std::greater<std::pair<int, int> >());
		for(int i=0; i<3; ++i){
			for(unsigned j=0; j<votes.size(); ++j){
				for(unsigned k=0; k<gfForPv[i].size(); ++k){
					if (votes[j].second == gfForPv[i][k]){
						pFaceMap[pf][i] = gfForPv[i][k];
						goto nextPv;
					}
				}
			}
			nextPv:;
		}
		if (pFaceMap[pf][0] == pFaceMap[pf][1]) pFaceMap[pf].face = pFaceMap[pf][0];
		else if (pFaceMap[pf][1] == pFaceMap[pf][2]) pFaceMap[pf].face = pFaceMap[pf][1];
		else if (pFaceMap[pf][2] == pFaceMap[pf][0]) pFaceMap[pf].face = pFaceMap[pf][2];
		else pFaceMap[pf].face = pFaceMap[pf][0];
		if (pFaceMap[pf].face == -1){
			return;
		}
	}
#if 0
	DSTR << "FaceMap PHtoGR:\n";
	for(unsigned i=0; i<pFaceMap.size(); ++i)
		DSTR << pFaceMap[i].face << ":" << pFaceMap[i][0] << " " << pFaceMap[i][1] << " " << pFaceMap[i][2] << ", ";
	DSTR << std::endl;
#endif

	//	対応表に応じてマテリアルリストを設定。
	gmd.materialList.resize(grFemMesh->materialList.size() ? pFaceMap.size() : 0);
	for(unsigned pf=0; pf<gmd.materialList.size(); ++pf){
		gmd.materialList[pf] = grFemMesh->materialList[pFaceMap[pf].face];
	}

	//	新しく作るGRMeshの頂点からphFemMeshの頂点への対応表
	vertexIdMap.resize(gmd.vertices.size(), -1);
	//	対応表に応じて、頂点のテクスチャ座標を作成
	//		phの１点がgrの頂点複数に対応する場合がある。
	//		その場合は頂点のコピーを作る必要がある。
	assert(grFemMesh->faceNormals.size());	// ロードしたファイルに法線情報が含まれていない
	std::vector<bool> vtxUsed(gmd.vertices.size(), false);
	for(unsigned pf=0; pf<pFaceMap.size(); ++pf){		
		for(unsigned i=0; i<3; ++i){
			int pv = gmd.faces[pf].indices[i];
			//	テクスチャ座標を計算
			Vec2f texCoord;
			Vec3f normal;
			GRMeshFace& gFace = grFemMesh->faces[pFaceMap[pf][i]];
			GRMeshFace& gFaceNormal = grFemMesh->faceNormals[pFaceMap[pf].face];
			GRMeshFace* gNormal = NULL;
			if (grFemMesh->normals.size()){
				gNormal = &gFace;
				if (grFemMesh->faceNormals.size()) gNormal = &grFemMesh->faceNormals[pFaceMap[pf].face];
			}
			if (gFace.nVertices == 3){
				Vec3f weight;
				Matrix3f vtxs;
				for(unsigned j=0; j<3; ++j){
					vtxs.col(j) = grFemMesh->vertices[gFace.indices[j]];
				}
				int tmp[3];
				vtxs.gauss(weight, gmd.vertices[pv], tmp);
				for(unsigned j=0; j<3; ++j){
					if (weight[j] <= -0.001){
						DSTR << "グラフィクスの3頂点の外側に物理の頂点があります。" << std::endl; 
						for(unsigned k=0; k<3; ++k){
							for(unsigned ax=0; ax<3; ++ax){
								DSTR << grFemMesh->vertices[gFace.indices[k]][ax];
								DSTR << (ax==2 ? "\n" : "\t");
							}
						}
						for(unsigned ax=0; ax<3; ++ax){
							DSTR << gmd.vertices[pv][ax];
							DSTR << (ax==2 ? "\n" : "\t");
						}
					}
					//assert(grFemMesh->texCoords.size() == 0);	// ロードしたファイルにUVマッピングの情報がない
					texCoord += weight[j] * grFemMesh->texCoords[gFace.indices[j]];
					if(gNormal) normal += weight[j] * grFemMesh->normals[gNormal->indices[j]];
				}
			}else{	//	4頂点
				//	どの３頂点で近似すると一番良いかを調べ、その３頂点を補間
				Vec3f weight[4];
				Matrix3f vtxs[4];
				double wMin[4];
				double wMinMax = -DBL_MAX;
				int maxId=-1;
				for(int j=0; j<4; ++j){
					for(int k=0; k<3; ++k){
						vtxs[j].col(k) = grFemMesh->vertices[gFace.indices[k<j ? k : k+1]];
					}
					int tmp[3];
					vtxs[j].gauss(weight[j], gmd.vertices[pv], tmp);
					wMin[j] = DBL_MAX;
					for(int l=0; l<3; ++l) if (wMin[j] > weight[j][l]) wMin[j] = weight[j][l];
					if (wMin[j] > wMinMax){
						wMinMax = wMin[j];
						maxId = j;
					}
				}
				for(int j=0; j<3; ++j){
					if (weight[maxId][j] <= -0.001){
						DSTR << "グラフィクスの3頂点\t"; 
						for(unsigned k=0; k<3; ++k){
							DSTR << grFemMesh->vertices[gFace.indices[k]] << "\t";
						}
						DSTR << "の外側に物理の頂点:\t" << gmd.vertices[pv] << "があります" << std::endl;
					}
					//assert(grFemMesh->texCoords.size() == 0);	// ロードしたファイルにUVマッピングの情報がない
					texCoord += weight[maxId][j] * grFemMesh->texCoords[gFace.indices[j<maxId?j:j+1]];
					if(gNormal){
						normal += weight[maxId][j] * grFemMesh->normals[gFaceNormal.indices[j<maxId?j:j+1]];
					}
				}
			}
			gmd.texCoords.resize(gmd.vertices.size());
			if (grFemMesh->normals.size()) gmd.normals.resize(gmd.vertices.size());
			//	重複頂点の場合はコピーを作りながら代入
			if (vtxUsed[pv]){
				if (gmd.texCoords[pv] != texCoord || 
					(grFemMesh->normals.size() && gmd.normals[pv] != normal)){	
					//	頂点のコピーの作成
					gmd.vertices.push_back(gmd.vertices[pv]);
					gmd.texCoords.push_back(texCoord);
					if (gmd.normals.size()) gmd.normals.push_back(normal);
					gmd.faces[pf].indices[i] = (int)gmd.vertices.size()-1;
					vertexIdMap.push_back(phFemMesh->surfaceVertices[pv]);
				}
			}else{	//	そうでなければ、直接代入
				gmd.texCoords[pv] = texCoord;
				if (gmd.normals.size()) gmd.normals[pv] = normal; 
				vertexIdMap[pv] = phFemMesh->surfaceVertices[pv];
				vtxUsed[pv] = true;
			}
		}
	}
	// 頂点カラーの設定
	//*デバックモードだとmesh形状によって落ちる。どこかでメモリを破壊してる可能性あり
#if _DEBUG
#else
	gmd.colors.resize(grFemMesh->colors.size() ? vertexIdMap.size() : 0);
	for(unsigned pv=0; pv<gmd.colors.size(); ++pv){
		gmd.colors[pv] = grFemMesh->colors[vertexIdMap[pv]];
	}
#endif
	//	GRMeshを作成
	GRMesh* rv = grFemMesh->GetNameManager()->CreateObject(GRMeshIf::GetIfInfoStatic(), &gmd)->Cast();
	//	マテリアルの追加
	for (unsigned i=0; i<grFemMesh->NChildObject(); ++i){
		rv->AddChildObject(grFemMesh->GetChildObject(i));
	}
	//	テクスチャモードをコピー
	rv->tex3d = grFemMesh->tex3d;
	grFemMesh = rv;
}

void FWFemMeshNew::Sync(){		
	//	同期処理
	FWObject::Sync();
	if(syncSource == FWObjectDesc::PHYSICS){
		if(GetPHFemMesh()->GetPHFemVibration()){
			SyncVibrationInfo();
		}
		//if(phFemThermo){
		//		SyncThermoInfo();
		//}
	}
}

void FWFemMeshNew::SyncVibrationInfo(){
	if(!grFemMesh) return;
	grFemMesh->EnableAlwaysCreateBuffer();
	// 頂点位置の同期
	Vec3f* grVertices = grFemMesh->GetVertices();
	for(int i = 0; i < (int)vertexIdMap.size(); i++){
		int pId = vertexIdMap[i];
		grVertices[i] = (Vec3f)phFemMesh->vertices[pId].pos;
	}
	// 変位で色変化
	if(grFemMesh->NColors() < 1) return;
	Vec4f* vc = grFemMesh->GetColors();
	double base = 1e-8;
	double offset = 1e-8;
	for(int i = 0; i < (int)vertexIdMap.size(); i++){
		int pId = vertexIdMap[i];
		float value = phFemMesh->GetVertexDisplacementL(pId).norm();
		//DSTR << value << std::endl;
		//DSTR << value/base + offset << std::endl;
		vc[i] = CompThermoColor(value/base + offset);
	}
}

Vec4f FWFemMeshNew::CompThermoColor(float value){
	float cos = -0.5 * std::cos( 4 * M_PI * value) + 0.5;
	Vec4f color;
	if(value > 1.0){
		// red
		color = Vec4f(1.0, 0.0, 0.0, 1.0);
	}else if(value > 3.0 / 4.0){
		// red + green
		color = Vec4f(1.0, cos, 0.0, 1.0);
	}else if(value > 2.0 / 4.0){
		// green
		color = Vec4f(cos, 1.0, 0.0, 1.0);
	}else if(value > 1.0 / 4.0){
		// green + blue
		color = Vec4f(0.0, 1.0, cos, 1.0);
	}else if(value > 0.0){
		// blue
		color = Vec4f(0.0, 0.0, 1.0, 1.0);
	}
	return color;
}


}

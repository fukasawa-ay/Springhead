/*
 *  Copyright (c) 2003 - 2011, Fumihiro Kato, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#include <SciLab/SprSciLab.h>
#include <Physics/PHFemMeshThermo.h>
#include <Base/Affine.h>
//#include <Framework/FWObject.h>
//#include <Framework/sprFWObject.h>

using namespace PTM;

namespace Spr{;

#define UseMatAll
//#define DEBUG
//#define DumK



PHFemMeshThermoDesc::PHFemMeshThermoDesc(){
	Init();
}
void PHFemMeshThermoDesc::Init(){
	thConduct = 0.574;
	rho = 970;
	heatTrans = 25;
	specificHeat = 0.196;		//1960
}

///////////////////////////////////////////////////////////////////
//	PHFemMeshThermo


PHFemMeshThermo::PHFemMeshThermo(const PHFemMeshThermoDesc& desc, SceneIf* s){
	deformed = true;			//変数の初期化、形状が変わったかどうか
	SetDesc(&desc);
	if (s){ SetScene(s); }
	StepCount =0;				// ステップ数カウンタ
	StepCount_ =0;				// ステップ数カウンタ
	//phFloor =  DCAST(FWObjectIf, GetSdk()->GetScene()->FindObject("fwPan"));
	//GetFramePosition();
}

void PHFemMeshThermo::CalcVtxDisFromOrigin(){
	//double origin = GetScene()->GetChildObject(
	//this->GetScene()->Get
	///不要　GRMeshThermo自身の原点を取得する⇒main()で初期化やって、後で移植
	/// PHFemMeshIf* pan		=	DCAST(PHFemMeshIf*,GetScene()->FindObject("Pan"));
	//>	nSurfaceの内、x,z座標から距離を求めてsqrt(2乗和)、それをFemVertexに格納する
	///同心円系の計算に利用する　distance from origin

	//> 以下で取得する位置は、世界座標系!？ローカル座標系の位置の取り方を知りたい!!!
	for(int i=0; i<NSurfaceVertices(); i++){
		if(vertices[surfaceVertices[i]].pos.y < 0){
			double len = sqrt(vertices[surfaceVertices[i]].pos.x * vertices[surfaceVertices[i]].pos.x + vertices[surfaceVertices[i]].pos.z *vertices[surfaceVertices[i]].pos.z);
			vertices[surfaceVertices[i]].disFromOrigin = len;
			for(unsigned j=0; j < vertices[surfaceVertices[i]].faces.size();j++){ 
				faces[vertices[surfaceVertices[i]].faces[j]].mayIHheated = true;				//> IH加熱の対象候補面(pos.y<0が一つ目の条件)
			}
		}else{
			//>	物体の下底面でない場合には、0を入れて、計算から除外する
			vertices[surfaceVertices[i]].disFromOrigin = 0.0;
		}
		//DSTR << i << "th verticies pos: " << vertices[surfaceVertices[i]].pos << std::endl;
		//DSTR << i << "th distans from origin: " << len << std::endl;
		//DSTR << std::endl;
	}
	int kadoon =0;
}

void PHFemMeshThermo::SetThermalBoundaryCondition(){
	
}

void PHFemMeshThermo::CreateMatKAll(){

}

void PHFemMeshThermo::CreateMatCAll(){

}


void PHFemMeshThermo::ScilabTest(){
	if (!ScilabStart()) std::cout << "Error : ScilabStart \n";

	//	行列の読み書き
	Matrix2f A;
	A.Ex() = Vec2f(1,2);
	A.Ey() = Vec2f(3,4);
	std::cout << "A:  " << A ;
	std::cout << "A00:" << A[0][0] << std::endl;
	std::cout << "A01:" << A[0][1] << std::endl;
	std::cout << "A10:" << A[1][0] << std::endl;
	std::cout << "A11:" << A[1][1] << std::endl;
	
	ScilabSetMatrix("A", A);
	ScilabJob("b=[4;5]");
	std::cout << "A=";
	ScilabJob("disp(A);");
	std::cout << "b=";
	ScilabJob("disp(b);");
	std::cout << "x=A\\b" << std::endl;
	ScilabJob("A,b,x=A\\b;");
	
	ScilabGetMatrix(A, "A");
	SCMatrix b = ScilabMatrix("b");
	SCMatrix x = ScilabMatrix("x");
	std::cout << "x:" << x << std::endl;

	ScilabJob("y = A;");
	SCMatrix y = ScilabMatrix("y");
	std::cout << "y=" << y;
	y = 2*A;
	std::cout << "y = 2*A is done by C++ code" << std::endl;
	std::cout << "y=";
	ScilabJob("disp(y);");
	std::cout << A;
	std::cout << y;
	ScilabJob("clear;");

	//	グラフ描画
	ScilabJob("t = 0:0.01:2*3.141592653;");
	ScilabJob("x = sin(t);");
	ScilabJob("y = cos(t);");
	ScilabJob("plot2d(x, y);");
	for(int i=0; i<100000; ++i){
		ScilabJob("");
	}
//	ScilabEnd();
}

void PHFemMeshThermo::UsingFixedTempBoundaryCondition(unsigned id,double temp){
	//温度固定境界条件
	SetVertexTemp(id,temp);
	//for(unsigned i =0;i < vertices.size()/3; i++){
	//	SetVertexTemp(i,temp);
	//}
}

void PHFemMeshThermo::UsingHeatTransferBoundaryCondition(unsigned id,double temp,double heatTransRatio){
	//熱伝達境界条件
	//節点の周囲流体温度の設定(K,C,Fなどの行列ベクトルの作成後に実行必要あり)
//	if(vertices[id].Tc != temp){					//更新する節点のTcが変化した時だけ、TcやFベクトルを更新する
		SetLocalFluidTemp(id,temp);
		vertices[id].heatTransRatio = heatTransRatio;
		//熱伝達境界条件が使われるように、する。				///	＋＝してベクトルを作っているので、下のコードでは、余計に足してしまっていて、正しい行列を作れない。
		//for(unsigned i =0;i < vertices[id].tets.size();i++){
		//	CreateVecfLocal(tets[vertices[id].tets[i]]);		//	Tcを含むベクトルを更新する
		//}

		///	熱伝達率を含む項(K2,f3)のみ再計算
		InitCreateVecf_();				///	変更する必要のある項のみ、入れ物を初期化
		InitCreateMatk_();
		for(unsigned i =0; i < edges.size();i++){
			edges[i].k = 0.0;
		}
		for(unsigned i=0; i< this->tets.size();i++){
			//DSTR << "this->tets.size(): " << this->tets.size() <<std::endl;			//12
			CreateVecfLocal(i);				///	VecFの再作成
			CreateMatkLocal(i);				///	MatK2の再作成 →if(deformed==true){matk1を生成}		matK1はmatk1の変数に入れておいて、matk2だけ、作って、加算
			//DSTR <<"tets["<< i << "]: " << std::endl;
			//DSTR << "this->tets[i].matk1: " <<std::endl;
			//DSTR << this->tets[i].matk1  <<std::endl;
			//DSTR << "this->tets[i].matk2: " <<std::endl;
			//DSTR << this->tets[i].matk2  <<std::endl;
			//
			//DSTR << "dMatCAll: " << dMatCAll << std::endl;
			int hogehogehoge =0;
		}
//	}
		///	節点の属する面のalphaUpdatedをtrueにする
		for(unsigned i=0;i<vertices[id].faces.size();i++){
			faces[vertices[id].faces[i]].alphaUpdated = true;
			alphaUpdated = true;
		}
}

void PHFemMeshThermo::UsingHeatTransferBoundaryCondition(unsigned id,double temp){
	//熱伝達境界条件
	//節点の周囲流体温度の設定(K,C,Fなどの行列ベクトルの作成後に実行必要あり)
//	if(vertices[id].Tc != temp){					//更新する節点のTcが変化した時だけ、TcやFベクトルを更新する
		SetLocalFluidTemp(id,temp);
		//熱伝達境界条件が使われるように、する。
		//for(unsigned i =0;i < vertices[id].tets.size();i++){
		//	CreateVecfLocal(tets[vertices[id].tets[i]]);		//	Tcを含むベクトルを更新する
		//}
		InitCreateVecf_();
		for(unsigned i=0; i < this->tets.size();i++){
			CreateVecfLocal(i);				///	VeecFの再作成
													///	MatK2の再作成→matK1はmatk1の変数に入れておいて、matk2だけ、作って、加算
		}
//	}
}

void PHFemMeshThermo::SetRohSpheat(double r,double Spheat){
	//> 密度、比熱 of メッシュのグローバル変数(=メッシュ固有の値)を更新
	rho = r;
	specificHeat = Spheat;
}

std::vector<Vec2d> PHFemMeshThermo::CalcIntersectionPoint2(unsigned id0,unsigned id1,double r,double R){
	double constA = 0.0;
	double constB = 0.0;
	///	rと交点
	double constX1 = 0.0;
	double constX1_ = 0.0;
	double constY1 = 0.0;
	///	Rと交点
	double constX2 = 0.0;
	double constX2_ = 0.0;
	double constY2 = 0.0;

	//> 引数の代替処理	関数化したときに、変換する↓
	unsigned vtxId0 = id0;
	unsigned vtxId1 = id1;
	/// 原点に近い順に並び替え
	if(vertices[vtxId0].disFromOrigin > vertices[vtxId1].disFromOrigin){
		unsigned tempId = vtxId0;
		vtxId0 = vtxId1;
		vtxId1 = tempId;
	}
	/// vtxId0 < vtxId1 が保証されている

	//> 2点のdisFromOriginをr,Rと比較してどちらと交わるかを判別する。
	if( (r <= vertices[vtxId0].disFromOrigin && vertices[vtxId0].disFromOrigin <= R) ^ (r <= vertices[vtxId1].disFromOrigin && vertices[vtxId1].disFromOrigin <= R)){
		//> 円環との交点を求める
		// x-z平面で考えている
		/// constA,B:vtxId0.vtxId1を通る直線の傾きと切片　/// aconsta,constbは正負構わない
		constA = ( vertices[vtxId0].pos.z - vertices[vtxId1].pos.z) / ( vertices[vtxId0].pos.x - vertices[vtxId1].pos.x);
		constB = vertices[vtxId0].pos.z - constA * vertices[vtxId0].pos.x;

		///	交点の座標を計算
		if(vertices[vtxId0].disFromOrigin < r){		/// 半径rの円と交わるとき
			//CalcYfromXatcross(vtxId0,vtxId1,r);	//関数化しない
			//> 以下、関数化,vtxId0,1,r:引数、constYを返す
			constX1 = (- constA * constB + sqrt(r * r *(constA * constA + 1.0) - constB * constB));
			constX1_ = (- constA * constB - sqrt(r * r *(constA * constA + 1.0) - constB * constB));
			// どちらかが頂点の間にある　大小がわからないので、orで、点1のx座標、2のx座標と、その入れ替えと、作る
			//> 線分の両端の点の間にあるとき
			if( (vertices[vtxId0].pos.x <= constX1 && constX1 <= vertices[vtxId1].pos.x) || (vertices[vtxId1].pos.x <= constX1 && constX1 <= vertices[vtxId0].pos.x) ){
				constY1 = sqrt(r * r - constX1 * constX1 );
			}else{
				constY1 = sqrt(r * r - constX1_ * constX1_ );
				constX1 = constX1_;		///		点のx座標はconstX_が正しい事がわかった。
			}
		}else if(vertices[vtxId0].disFromOrigin < R){		/// 半径Rの円と交わるとき
			constX1 = (- constA * constB + sqrt(R * R *(constA * constA + 1.0) - constB * constB));
			constX1_ = (- constA * constB - sqrt(R * R *(constA * constA + 1.0) - constB * constB));
			// どちらかが頂点の間にある　大小がわからないので、orで、点1のx座標、2のx座標と、その入れ替えと、作る
			//> 線分の両端の点の間にあるとき
			if( (vertices[vtxId0].pos.x <= constX1 && constX1 <= vertices[vtxId1].pos.x) || (vertices[vtxId1].pos.x <= constX1 && constX1 <= vertices[vtxId0].pos.x) ){
				constY1 = sqrt(R * R - constX1 * constX1 );
			}else{
				constY1 = sqrt(R * R - constX1_ * constX1_ );
				constX1 = constX1_;		///		点のx座標はconstX_が正しい事がわかった。
			}
		
		}
		//> どちらとも交わるとき
		else if(vertices[vtxId0].disFromOrigin < r && R < vertices[vtxId1].disFromOrigin){
			//> 定数が2つ欲しい
			constX1 = (- constA * constB + sqrt(r * r *(constA * constA + 1.0) - constB * constB));
			constX1_ = (- constA * constB - sqrt(r * r *(constA * constA + 1.0) - constB * constB));
			// どちらかが頂点の間にある　大小がわからないので、orで、点1のx座標、2のx座標と、その入れ替えと、作る
			//> 線分の両端の点の間にあるとき
			if( (vertices[vtxId0].pos.x <= constX1 && constX1 <= vertices[vtxId1].pos.x) || (vertices[vtxId1].pos.x <= constX1 && constX1 <= vertices[vtxId0].pos.x) ){
				constY1 = sqrt(r * r - constX1 * constX1 );
			}else{
				constY1 = sqrt(r * r - constX1_ * constX1_ );
				constX1 = constX1_;		///		点のx座標はconstX_が正しい事がわかった。
			}
			constX2 = (- constA * constB + sqrt(R * R *(constA * constA + 1.0) - constB * constB));
			constX2_ = (- constA * constB - sqrt(R * R *(constA * constA + 1.0) - constB * constB));
			//> 線分の両端の点の間にあるとき
			if( (vertices[vtxId0].pos.x <= constX2 && constX2 <= vertices[vtxId1].pos.x) || (vertices[vtxId1].pos.x <= constX2 && constX2 <= vertices[vtxId0].pos.x) ){
				constY2 = sqrt(R * R - constX2 * constX2 );
			}else{
				constY2 = sqrt(R * R - constX2_ * constX2_ );
				constX2 = constX2_;		///		点のx座標はconstX_が正しい事がわかった。
			}
		}
	}
	std::vector<Vec2d> intersection;
	intersection.push_back(Vec2d(constX1,constY1));
	if(constX2 && constY2){
		intersection.push_back(Vec2d(constX2,constY2));
	}
		//Vec4d interSection;
		//interSection[0] = constX1;
		//interSection[1] = constY1;
		//interSection[2] = constX2;
		//interSection[3] = constY2;
		DSTR << __FILE__  << "(" <<  __LINE__ << "):"<< "intersection: " << intersection[0] << std::endl;
		//":" <<  __TIME__ << 
		//return interSection;
		return intersection;
}

Vec2d PHFemMeshThermo::CalcIntersectionPoint(unsigned id0,unsigned id1,double r,double R){
	double constA = 0.0;
	double constB = 0.0;
	double constX = 0.0;
	double constX_ = 0.0;
	double constY = 0.0;

	//> 引数の代替処理	関数化したときに、変換する↓
	unsigned vtxId0 = id0;
	unsigned vtxId1 = id1;
	/// 原点に近い順に並び替え
	if(vertices[vtxId0].disFromOrigin > vertices[vtxId1].disFromOrigin){
		unsigned tempId = vtxId0;
		vtxId0 = vtxId1;
		vtxId1 = tempId;
	}
	/// vtxId0 < vtxId1 が保証されている

	//> 2点のdisFromOriginをr,Rと比較してどちらと交わるかを判別する。
	if( (r <= vertices[vtxId0].disFromOrigin && vertices[vtxId0].disFromOrigin <= R) ^ (r <= vertices[vtxId1].disFromOrigin && vertices[vtxId1].disFromOrigin <= R)){
		//> 円環との交点を求める
		// x-z平面で考えている
		/// constA,B:vtxId0.vtxId1を通る直線の傾きと切片　/// aconsta,constbは正負構わない
		constA = ( vertices[vtxId0].pos.z - vertices[vtxId1].pos.z) / ( vertices[vtxId0].pos.x - vertices[vtxId1].pos.x);
		constB = vertices[vtxId0].pos.z - constA * vertices[vtxId0].pos.x;

		///	交点の座標を計算
		if(vertices[vtxId0].disFromOrigin < r){		/// 半径rの円と交わるとき
			//CalcYfromXatcross(vtxId0,vtxId1,r);	//関数化しない
			//> 以下、関数化,vtxId0,1,r:引数、constYを返す
			constX = (- constA * constB + sqrt(r * r *(constA * constA + 1.0) - constB * constB));
			constX_ = (- constA * constB - sqrt(r * r *(constA * constA + 1.0) - constB * constB));
			// どちらかが頂点の間にある　大小がわからないので、orで、点1のx座標、2のx座標と、その入れ替えと、作る
			//> 線分の両端の点の間にあるとき
			if( (vertices[vtxId0].pos.x <= constX && constX <= vertices[vtxId1].pos.x) || (vertices[vtxId1].pos.x <= constX && constX <= vertices[vtxId0].pos.x) ){
				constY = sqrt(r * r - constX * constX );
			}else{
				constY = sqrt(r * r - constX_ * constX_ );
				constX = constX_;		///		点のx座標はconstX_が正しい事がわかった。
			}
		}else if(r < vertices[vtxId0].disFromOrigin < R){		/// 半径Rの円と交わるとき
			constX = (- constA * constB + sqrt(r * R *(constA * constA + 1.0) - constB * constB));
			constX_ = (- constA * constB - sqrt(r * R *(constA * constA + 1.0) - constB * constB));
			// どちらかが頂点の間にある　大小がわからないので、orで、点1のx座標、2のx座標と、その入れ替えと、作る
			//> 線分の両端の点の間にあるとき
			if( (vertices[vtxId0].pos.x <= constX && constX <= vertices[vtxId1].pos.x) || (vertices[vtxId1].pos.x <= constX && constX <= vertices[vtxId0].pos.x) ){
				constY = sqrt(R * R - constX * constX );
			}else{
				constY = sqrt(R * R - constX_ * constX_ );
				constX = constX_;		///		点のx座標はconstX_が正しい事がわかった。
			}
		}
		//> どちらとも交わるとき
		else if(vertices[vtxId0].disFromOrigin < r && R < vertices[vtxId1].disFromOrigin){
			//> 定数が2つ欲しい
		}
	}
		Vec2d interSection;
		interSection[0] = constX;
		interSection[1] = constY;
		DSTR << __FILE__  << "(" <<  __LINE__ << "):"<< "interSection: " << interSection << std::endl;
		//":" <<  __TIME__ << 
		return interSection;
}

void PHFemMeshThermo::CalcIHdqdt5(double radius,double Radius,double dqdtAll){
	//> radius value check
	if(Radius <= radius){
		DSTR << "inner radius size is larger than outer Radius " << std::endl;
		DSTR << "check and set another value" << std::endl;
		assert(0);
	}
	///	内半径と外半径の間の節点に熱流束境界条件を設定
	//> 四面体面の三角形と円環領域の重複部分の形状・面積を求める当たり判定を計算する。
	//>	切り取り形状に応じた形状関数を求め、熱流束ベクトルの成分に代入し、計算する

	//> 1.円環領域と重なるface三角形の形状を算出する。領域に含まれる頂点、face三角形の辺との交点を求めてvecteorに格納する
	//>	2.vectorには、辺0,1,2の順に領域内の頂点や交点が入っているが、これを元に三角形分割を行う。三角形分割ができたら、各三角形を求める。三角形の総和を、このfaceの加熱領域とする。
	//>	3.vectorの点における形状関数を求めて、擬似体積（重なっている面積×形状関数の値）を使って、四面体内の各点における形状関数の面積分を求める。求めた値は、熱流束ベクトルの成分として要素剛性行列の成分に代入する。
	//>	4.毎ステップ、同じ熱流束の値をベクトル成分に加える

	//> raius,RadiusについてmayIHheatedの確度を上げてから、円環領域と重なっている形状を求める
	for(unsigned i=0;i < nSurfaceFace; i++){
		if(faces[i].mayIHheated){			// faceの節点のy座標が負の場合→IH加熱の対象節点 円環の範囲内に入っているとは限らない
			for(unsigned j=0;j<3;j++){
				/// 円環領域内にface頂点が含まれ無い
				if(radius <= vertices[faces[i].vertices[j]].disFromOrigin && vertices[faces[i].vertices[j]].disFromOrigin <= Radius){
					faces[i].mayIHheated = true;
					break;		//>	見つかったら、判定はtrueのままで良い。最内側のforを抜ける
				}
				else{
					faces[i].mayIHheated = false;
				}
				//> 円環領域の内・外側にfaceの辺の頂点がある
				if(vertices[faces[i].vertices[j%3]].disFromOrigin < radius && Radius < vertices[faces[i].vertices[(j+1)%3]].disFromOrigin 
					|| vertices[faces[i].vertices[(j+1)%3]].disFromOrigin < radius && Radius < vertices[faces[i].vertices[j%3]].disFromOrigin){
						faces[i].mayIHheated = true;
						break;		//>	同上
				}else{
					faces[i].mayIHheated = false;
				}
			}
		}
	}
	//> debug
	//>	mayIHheatedのフラグが立っているfaceにその面積の形状関数を与えてみる。	重なる面積をきちんと計算と、少しでも引っかかっていれば、加熱面に入れてしまう計算、試す
	//> CalcIHdqdt3 or 4
	unsigned numIHheated = 0; 
	for(unsigned i=0; i < nSurfaceFace;i++){
		if(faces[i].mayIHheated){
			DSTR << i << " ; "  << std::endl;
			numIHheated +=1;
		}
	}
	DSTR << "numIHheated / nSurfaceFace: " << numIHheated << " / " << nSurfaceFace << std::endl;		///:	鉄板:264/980
	
	//> 交点を求め、faces構造体のvectorに領域内の頂点や交点を格納
	for(unsigned i=0;i < nSurfaceFace; i++){
		if(faces[i].mayIHheated){		////	may から 確実になった
			//>	頂点を入れるvector or 配列		//>	円環領域と重なる形状を計算するために、重なる領域内にあるface頂点、辺との交点をfaceの辺0~2の順にvectorに格納していく。格納時に重複が無いようにする
			std::vector<Vec2d> intersectVtx;
			//Vec2d tempXZ[4] = {Vec2d(0.0, 0.0),Vec2d(0.0, 0.0),Vec2d(0.0, 0.0),Vec2d(0.0, 0.0)};
			std::vector<Vec2d> tempXZ;
			// 不要
			//for(unsigned j =0;j<4; j++){
			//	DSTR << "tempXZ[" << j <<"]: " << tempXZ[j] << std::endl;
			//}
			//Vec2d innertVtx[12];		//>	vectorから配列に変更する→静的メモリ確保,けど、vectorの方がコードは楽?メモリの開放を忘れずに
			//unsigned judge[2] = {0,0};		///	judge[0],[1]の順に原点に近い点の判定結果
			if(faces[i].area==0) faces[i].area = CalcTriangleArea(faces[i].vertices[0],faces[i].vertices[1],faces[i].vertices[2]);		// 面積計算が済んでなければ計算する
			
			//>	face内の頂点のdisFromOriginの値でソート
			unsigned nearestvtxnum		=	0;				///	原点に一番近い頂点のface頂点番号
			for(unsigned j=0;j<3;j++){
				double hikaku = DBL_MAX;
				if(hikaku > vertices[faces[i].vertices[j]].disFromOrigin){	hikaku = vertices[faces[i].vertices[j]].disFromOrigin;	nearestvtxnum = j;}
			}

			///	頂点の組みからなる辺について、内側の頂点から領域内の頂点又は、辺と交わる交点をvectorに格納していく
			//Vec2d vtxXZ[4];
			//for(unsigned)
			//={0.0, 0.0};
			/// 領域内にスタート頂点があるとき
			if(radius < vertices[faces[i].vertices[nearestvtxnum]].disFromOrigin && vertices[faces[i].vertices[(nearestvtxnum+1)%3]].disFromOrigin < Radius){	///	頂点が領域内にあるとき
				intersectVtx.push_back( Vec2d(vertices[faces[i].vertices[nearestvtxnum]].pos.x,vertices[faces[i].vertices[nearestvtxnum]].pos.z) );
			}
			/// 内円とだけ交点をもつとき
			else if(vertices[faces[i].vertices[nearestvtxnum]].disFromOrigin < radius && vertices[faces[i].vertices[(nearestvtxnum+1)%3]].disFromOrigin < Radius){
				//Vec2d vtxXZ = CalcIntersectionPoint(faces[i].vertices[nearestvtxnum],faces[i].vertices[(nearestvtxnum+1)%3],radius,Radius);
				tempXZ.push_back( CalcIntersectionPoint(faces[i].vertices[nearestvtxnum],faces[i].vertices[(nearestvtxnum+1)%3],radius,Radius) );
				DSTR << "case1 tempXZ[0]" << tempXZ[0] << std::endl;
			}
			//>	外円とだけ交点をもつとき
			else if(radius < vertices[faces[i].vertices[nearestvtxnum]].disFromOrigin && vertices[faces[i].vertices[(nearestvtxnum+1)%3]].disFromOrigin < Radius){
				//Vec2d vtxXZ = CalcIntersectionPoint(faces[i].vertices[nearestvtxnum],faces[i].vertices[(nearestvtxnum+1)%3],radius,Radius);
				tempXZ.push_back( CalcIntersectionPoint(faces[i].vertices[nearestvtxnum],faces[i].vertices[(nearestvtxnum+1)%3],radius,Radius) );
				DSTR << "case2 tempXZ[0]" << tempXZ[0] << std::endl;
			}
			//>		内円と外円と交点を持つ場合
			else if(vertices[faces[i].vertices[nearestvtxnum]].disFromOrigin < radius  && Radius < vertices[faces[i].vertices[(nearestvtxnum + 1)%3]].disFromOrigin ){	//>	頂点が領域0と1にあるとき
				//> 交点を求め、交点の値をvectorに代入する
				//Vec4d vtxXZ = CalcIntersectionPoint2(faces[i].vertices[nearestvtxnum],faces[i].vertices[(nearestvtxnum+1)%3],radius,Radius);
				tempXZ = CalcIntersectionPoint2(faces[i].vertices[nearestvtxnum],faces[i].vertices[(nearestvtxnum+1)%3],radius,Radius);
				DSTR << "case3 tempXZ[0]" << tempXZ[0] << std::endl;
				if(tempXZ.size() >2){DSTR << "case3 tempXZ[1]" << tempXZ[1] << std::endl;}
				//> 交点は、内円と、外円と2つ存在する
				// 2つの交点に分割
				//Vec2d vtxXZ0;
				//vtxXZ0[0] = vtxXZ[0];
				//vtxXZ0[1] = vtxXZ[1];
				//Vec2d vtxXZ1;
				//vtxXZ1[2] = vtxXZ[2];
				//vtxXZ1[3] = vtxXZ[3];
			}
			for(unsigned j=0; j< tempXZ.size();j++){
				DSTR << "tempXZ[" << j << "]" << tempXZ[j] << std::endl;
			}
			
		}
	}


	//>	Step	1.の実装
	//>	radius:内半径、Radius:外半径,　dqdtAll:単位時間あたりに入ってくる全熱量

	//%%	手順 
	//> isHeated(->nearIHHeater)のfacesの内、3つの節点全部について、特定半径の中に入っているものがあるかどうかをチェックする。
	//>	入っているものを、見つけたら、面積計算をしたいが、ここで、何個の節点が入っているかによって場合分けを行う。
	//>	３つ:faceの面積をfaceSに代入する	1~2つ:面積を近似計算するために、3節点の内、どの節点と節点で構成された辺が内半径rまたは、外半径Rと交わっているか判定
	//>	交わっている場合には、その交点を求める。求めた交点と内半径以上外半径以内の領域にあるface内の節点を用いて、三角形を作り、その面積を求める。

//> 以下コピペ元のソース、実装後に消す
	//>	1.1領域と重複する面積をface毎に求める
	double faceS = 0.0;
	for(unsigned i=0;i < nSurfaceFace; i++){
		if(faces[i].mayIHheated){			// faceの節点のy座標が負の場合→IH加熱の対象節点 円環の範囲内に入っているとは限らない
			unsigned nObinnerVtx = 0;

			unsigned innervtx[4] = {0,0,0,0};
			//>	頂点を入れるvector or 配列		//>	円環領域と重なる形状を計算するために、重なる領域内にあるface頂点、辺との交点をfaceの辺0~2の順にvectorに格納していく。格納時に重複が無いようにする
			std::vector<Vec2d> intersectVtx;
			//Vec2d innertVtx[12];		//>	vectorから配列に変更する→静的メモリ確保,けど、vectorの方がコードは楽?メモリの開放を忘れずに
			unsigned judge[2] = {0,0};		///	judge[0],[1]の順に原点に近い点の判定結果
			if(faces[i].area==0) faces[i].area = CalcTriangleArea(faces[i].vertices[0],faces[i].vertices[1],faces[i].vertices[2]);		// 面積計算が済んでなければ計算する
			
			//> 領域内に入っている点が見つかり次第、faceの頂点を含め、face.vectorに入れる。faceの頂点を入れる場合には、重複がないかを調べる。

			/// faceの各辺の両端の点について条件判定 ////	jは0から順に書いているだけで、faceの辺番号ではない
			for(unsigned j=0; j < 3; j++){
				///	face内の点の組(辺)について、その辺の組が
				/// vtx[(0+j)%3]について,原点に近い順に並び替え	//> == :同じ距離の時、何もしない。
				unsigned vtx0 = faces[i].vertices[(j+0)%3];
				unsigned vtx1 = faces[i].vertices[(j+1)%3];
				if(vertices[vtx0].disFromOrigin > vertices[vtx1].disFromOrigin){
					vtx0 = faces[i].vertices[(j+1)%3];
					vtx1 = faces[i].vertices[(j+0)%3];
				}
				///		   ＼      ＼	
				///  ・	  ・ )  ・   )	 ・
				///			/       /	 	
				///  ↑     ↑   　↑	 ↑	
				/// 原点　内半径　外半径 face頂点	
				/// judge[0]:vtx0, judge[1]:vtx1が領域内にあるかどうか (O(=Origin) < vtx0.disFromOrigin < vtx1.disFromOrigin )
				///	vtx0が r<, < R, else 領域ごとにjudge[]

				///	3点全部が領域内にあるとき、ただvectorに格納するだけでは、重複する頂点が出てきてしまう。
				///	ので、3点が全部領域内にあるときは、push_backの前に、除外する必要がある。
				///	同じ座標の点がvectorに入っていないか確認してから格納する

				//> 関数化する
				if(vertices[vtx0].disFromOrigin < radius){judge[0] =0;}
				else if(Radius < vertices[vtx0].disFromOrigin){judge[0] =2;}
				else{
					///	faces三角形頂点の格納
					judge[0] = 1;
					//> 領域内の頂点を格納
					Vec2d tempcoord = Vec2d(vertices[vtx0].pos.x,vertices[vtx0].pos.z);		
					//>	同じ座標値の点がなければ、格納。findっていう関数があったような気がする.	map ? list?
					unsigned tempjudge =0;
					for(unsigned k =0; k < intersectVtx.size(); k++){
						if(intersectVtx[k] == tempcoord){
							tempjudge += 1;
						}
					}
					if(tempjudge == 0){
						faces[i].innerIH.push_back(tempcoord);
						DSTR << "tempcoord: " << tempcoord <<std::endl;
					}
					for(unsigned k=0;k < intersectVtx.size();k++){
						DSTR << k << "th: " << intersectVtx[k] << std::endl;
					}
					//DSTR << "faces[i].innerIH: " << faces[i].innerIH.end() << std::endl;
				}
				
				///	vtx1が r<, < R, else 領域ごとにjudge[]
				if(vertices[vtx1].disFromOrigin < radius){judge[1] =0;}
				else if(Radius < vertices[vtx1].disFromOrigin){judge[1] =2;}
				else{
					///	faces三角形頂点の格納
					judge[1] = 1;
					//> 領域内の頂点を格納
					Vec2d tempcoord = Vec2d(vertices[vtx1].pos.x,vertices[vtx1].pos.z);
					//>	同じ座標値の点がなければ、格納。findっていう関数があったような気がする.	map ? list?
					unsigned tempjudge =0;
					for(unsigned k =0; k < intersectVtx.size(); k++){
						if(intersectVtx[k] == tempcoord){
							tempjudge += 1;
						}
					}
					if(tempjudge == 0){
						faces[i].innerIH.push_back(tempcoord);
						DSTR << "tempcoord: " << tempcoord <<std::endl;
					}
					for(unsigned k=0;k < intersectVtx.size();k++){
						DSTR << k << "th: " << intersectVtx[k] << std::endl;
					}
					//DSTR << "faces[i].innerIH: " << faces[i].innerIH.end() << std::endl;					//DSTR << "faces[i].innerIH: " << faces[i].innerIH[i].end() << std::endl;
				}

				//> 交点の格納
				/// faceの辺の両端の頂点でjudgeの値が異なる	→	円との交点を求める
				/// 内半径(r),外半径(R)との交点を求める	[0]!=[1]:交点を持つ、どちらかが、r,Rとの交点を持つ
				if(judge[0] == 0 && judge [1]==1 )
				{
					//> radiusとの交点を求める
					//> 求めた交点を代入する
					Vec2d vtxXZ = Vec2d(0.0,0.0);		//> 交点の座標を代入
					//> 交点のvectorに格納
					faces[i].innerIH.push_back(vtxXZ);
					//intersectVtx.push_back(vtxXZ);
				}
				else if(judge[0] == 1 && judge [1]==2 )
				{
					//> Rとの交点を求める
					//> 求めた交点をvectorに格納する
				}
				else if(judge[0] == 1 && judge [1]==2 )
				{
					//> r,Rとの交点を求める
					//> 求めた交点をvectorに格納する
				}
			}
		}
	}
}



void PHFemMeshThermo::CalcIHdqdt4(double radius,double Radius,double dqdtAll){
	///	内半径と外半径の間の節点に熱流束境界条件を設定
	//> 四面体面の三角形と円環領域の重複部分の形状・面積を求める当たり判定を計算する。
	//>	切り取り形状に応じた形状関数を求め、熱流束ベクトルの成分に代入し、計算する

	//> 1.円環領域と重なるface三角形の形状を算出する。領域に含まれる頂点、face三角形の辺との交点を求めてvecteorに格納する
	//>	2.vectorには、辺0,1,2の順に領域内の頂点や交点が入っているが、これを元に三角形分割を行う。三角形分割ができたら、各三角形を求める。三角形の総和を、このfaceの加熱領域とする。
	//>	3.vectorの点における形状関数を求めて、擬似体積（重なっている面積×形状関数の値）を使って、四面体内の各点における形状関数の面積分を求める。求めた値は、熱流束ベクトルの成分として要素剛性行列の成分に代入する。
	//>	4.毎ステップ、同じ熱流束の値をベクトル成分に加える

	//>	Step	1.の実装
	//>	radius:内半径、Radius:外半径,　dqdtAll:単位時間あたりに入ってくる全熱量

	//%%	手順 
	//> isHeated(->nearIHHeater)のfacesの内、3つの節点全部について、特定半径の中に入っているものがあるかどうかをチェックする。
	//>	入っているものを、見つけたら、面積計算をしたいが、ここで、何個の節点が入っているかによって場合分けを行う。
	//>	３つ:faceの面積をfaceSに代入する	1~2つ:面積を近似計算するために、3節点の内、どの節点と節点で構成された辺が内半径rまたは、外半径Rと交わっているか判定
	//>	交わっている場合には、その交点を求める。求めた交点と内半径以上外半径以内の領域にあるface内の節点を用いて、三角形を作り、その面積を求める。

	//>	1.1領域と重複する面積をface毎に求める
	double faceS = 0.0;
	for(unsigned i=0;i < nSurfaceFace; i++){
		if(faces[i].mayIHheated){			// faceの節点のy座標が負の場合→IH加熱の対象節点 円環の範囲内に入っているとは限らない
			unsigned nObinnerVtx = 0;

			unsigned innervtx[4] = {0,0,0,0};
			//>	頂点を入れるvector or 配列		//>	円環領域と重なる形状を計算するために、重なる領域内にあるface頂点、辺との交点をfaceの辺0~2の順にvectorに格納していく。格納時に重複が無いようにする
			std::vector<Vec2d> intersectVtx;
			//Vec2d innertVtx[12];		//>	vectorから配列に変更する→静的メモリ確保,けど、vectorの方がコードは楽?メモリの開放を忘れずに
			unsigned judge[2] = {0,0};		///	judge[0],[1]の順に原点に近い点の判定結果
			if(faces[i].area==0) faces[i].area = CalcTriangleArea(faces[i].vertices[0],faces[i].vertices[1],faces[i].vertices[2]);		// 面積計算が済んでなければ計算する
			
			//> 領域内に入っている点が見つかり次第、faceの頂点を含め、face.vectorに入れる。faceの頂点を入れる場合には、重複がないかを調べる。

			/// faceの各辺の両端の点について条件判定 ////	jは0から順に書いているだけで、faceの辺番号ではない
			for(unsigned j=0; j < 3; j++){
				///	face内の点の組(辺)について、その辺の組が
				/// vtx[(0+j)%3]について,原点に近い順に並び替え	//> == :同じ距離の時、何もしない。
				unsigned vtx0 = faces[i].vertices[(j+0)%3];
				unsigned vtx1 = faces[i].vertices[(j+1)%3];
				if(vertices[vtx0].disFromOrigin > vertices[vtx1].disFromOrigin){
					vtx0 = faces[i].vertices[(j+1)%3];
					vtx1 = faces[i].vertices[(j+0)%3];
				}
				///		   ＼      ＼	
				///  ・	  ・ )  ・   )	 ・
				///			/       /	 	
				///  ↑     ↑   　↑	 ↑	
				/// 原点　内半径　外半径 face頂点	
				/// judge[0]:vtx0, judge[1]:vtx1が領域内にあるかどうか (O(=Origin) < vtx0.disFromOrigin < vtx1.disFromOrigin )
				///	vtx0が r<, < R, else 領域ごとにjudge[]

				///	3点全部が領域内にあるとき、ただvectorに格納するだけでは、重複する頂点が出てきてしまう。
				///	ので、3点が全部領域内にあるときは、push_backの前に、除外する必要がある。
				///	同じ座標の点がvectorに入っていないか確認してから格納する

				//> 関数化する
				if(vertices[vtx0].disFromOrigin < radius){judge[0] =0;}
				else if(Radius < vertices[vtx0].disFromOrigin){judge[0] =2;}
				else{
					///	faces三角形頂点の格納
					judge[0] = 1;
					//> 領域内の頂点を格納
					Vec2d tempcoord = Vec2d(vertices[vtx0].pos.x,vertices[vtx0].pos.z);		
					//>	同じ座標値の点がなければ、格納。findっていう関数があったような気がする.	map ? list?
					unsigned tempjudge =0;
					for(unsigned k =0; k < intersectVtx.size(); k++){
						if(intersectVtx[k] == tempcoord){
							tempjudge += 1;
						}
					}
					if(tempjudge == 0){
						faces[i].innerIH.push_back(tempcoord);
						DSTR << "tempcoord: " << tempcoord <<std::endl;
					}
					for(unsigned k=0;k < intersectVtx.size();k++){
						DSTR << k << "th: " << intersectVtx[k] << std::endl;
					}
					//DSTR << "faces[i].innerIH: " << faces[i].innerIH.end() << std::endl;
				}
				
				///	vtx1が r<, < R, else 領域ごとにjudge[]
				if(vertices[vtx1].disFromOrigin < radius){judge[1] =0;}
				else if(Radius < vertices[vtx1].disFromOrigin){judge[1] =2;}
				else{
					///	faces三角形頂点の格納
					judge[1] = 1;
					//> 領域内の頂点を格納
					Vec2d tempcoord = Vec2d(vertices[vtx1].pos.x,vertices[vtx1].pos.z);
					//>	同じ座標値の点がなければ、格納。findっていう関数があったような気がする.	map ? list?
					unsigned tempjudge =0;
					for(unsigned k =0; k < intersectVtx.size(); k++){
						if(intersectVtx[k] == tempcoord){
							tempjudge += 1;
						}
					}
					if(tempjudge == 0){
						faces[i].innerIH.push_back(tempcoord);
						DSTR << "tempcoord: " << tempcoord <<std::endl;
					}
					for(unsigned k=0;k < intersectVtx.size();k++){
						DSTR << k << "th: " << intersectVtx[k] << std::endl;
					}
					//DSTR << "faces[i].innerIH: " << faces[i].innerIH.end() << std::endl;					//DSTR << "faces[i].innerIH: " << faces[i].innerIH[i].end() << std::endl;
				}

				//> 交点の格納
				/// faceの辺の両端の頂点でjudgeの値が異なる	→	円との交点を求める
				/// 内半径(r),外半径(R)との交点を求める	[0]!=[1]:交点を持つ、どちらかが、r,Rとの交点を持つ
				if(judge[0] == 0 && judge [1]==1 )
				{
					//> radiusとの交点を求める
					//> 求めた交点を代入する
					Vec2d vtxXZ = Vec2d(0.0,0.0);		//> 交点の座標を代入
					//> 交点のvectorに格納
					faces[i].innerIH.push_back(vtxXZ);
					//intersectVtx.push_back(vtxXZ);
				}
				else if(judge[0] == 1 && judge [1]==2 )
				{
					//> Rとの交点を求める
					//> 求めた交点をvectorに格納する
				}
				else if(judge[0] == 1 && judge [1]==2 )
				{
					//> r,Rとの交点を求める
					//> 求めた交点をvectorに格納する
				}
			}
			


			/// 以下、コピペ前のコードのため、多分、不要。交点を求めるアルゴリズムだけ、上に転用する
//
//			//> 置き換えと0,1を入れ替えるだけ(=ポリモーフィズム)で残りの実装も作る
//				// 下記処理を関数化する?。点のIDを入れれば、同じ処理をする関数
//				//> 0-1で交点を作っているとき true false は論演算の排他的論理和XORでtrue時、交点を計算する 00=0,01=10=1,11=0; 片方の点が範囲内で、もう一方が範囲外の場合、trueになる
//			
//				//> 円環の範囲内に入っていたら、mayIHheatedをtrueに、あるいは、falseにする(一応)
//
//				/// faces[i]の頂点が何個、領域内に入っているかを見つける
//			for(unsigned j=0;j<3;j++){
//				if( radius <= vertices[faces[i].vertices[j]].disFromOrigin && vertices[faces[i].vertices[j]].disFromOrigin <= Radius){
//					nObinnerVtx += 1;
//					innervtx[j] = 1;
//				}
//			}
//
//			//> nObinnerVtxの値で条件分岐
//			
//			///	faceの辺が、円弧と交わる交点を求める
//			if(nObinnerVtx == 1){
//				Vec2d vtxXY[3];			/// faceの辺と半径r,Rとの交点のx,z座標　最大３点
//				for(unsigned j=0;j<3;j++){		///	faceを構成する3頂点について
//					unsigned k = 0;
//					unsigned m = 0;
//					k = (j+1)%3;		///	j=0;k=1, j=1;k=2, j=2;k=0 
//					m = (j+2)%3;
//					//vtxXY[j] = CalcIntersectionPoint(faces[i].vertices[j],faces[i].vertices[k],r,R);
//					//DSTR << "vtxXY[" << j << "]: " << vtxXY[j] << std::endl; 
//					if(innervtx[j] ==1){			/// faces[i]のj番目の節点が円環領域内に入っている
//					/// j番目の頂点とエッジを構成する点(他の２点)を使って、半径r,Rと交わる点を算出する						
//					//> j 0,1,2
//						if(j == 0){	/// 頂点jと辺を作る頂点を使って、辺と交わる交点を求める
//							//k = (j+1)%3;		///	j=0;k=1, j=1;k=2, j=2;k=0 
//						vtxXY[j] = CalcIntersectionPoint(faces[i].vertices[j],faces[i].vertices[k],radius,Radius);
//						vtxXY[k] = CalcIntersectionPoint(faces[i].vertices[j],faces[i].vertices[m],radius,Radius);
//						}
//						else if(j == 1){
//						vtxXY[j] = CalcIntersectionPoint(faces[i].vertices[j],faces[0].vertices[1],radius,Radius);
//						vtxXY[j] = CalcIntersectionPoint(faces[i].vertices[j],faces[2].vertices[2],radius,Radius);
//						}
//						else if(j == 2){
//						vtxXY[j] = CalcIntersectionPoint(faces[i].vertices[j],faces[0].vertices[1],radius,Radius);
//						vtxXY[j] = CalcIntersectionPoint(faces[i].vertices[j],faces[1].vertices[2],radius,Radius);
//						}
//						else{
//							assert(0);
//							DSTR << "Error in PHFemMeshThermo::CalcIHdqdt2(hogehoge) 領域判定にミス" << std::endl;
//						}
//					}
//				}
//			}
//				//for(unsigned j=0;j<3;j++){
//				//	/// face内の節点を順番にチェックする
//				//	unsigned k =0;
//				//	k = (j+1)%3;		///	j=0;k=1, j=1;k=2, j=2;k=0 
//				//	if( r <= vertices[faces[i].vertices[j]].disFromOrigin && vertices[faces[i].vertices[j]].disFromOrigin <= R){
//				//	}
//				//	if( r <= vertices[faces[i].vertices[j]].disFromOrigin && vertices[faces[i].vertices[j]].disFromOrigin <= R){
//				//		//> j,kをfaces[i].vertices[j],faces[i].vertices[k]として代入
//				//		CalcIntersectionPoint(faces[i].vertices[0],faces[i].vertices[1],r,R);
//				//	}
//				//}
//
//				//> r -> radius
//				//> R -> Radius
//			
//				/// 座標(consX ,constY)が円と三角形の辺との交点
//			
//				//unsigned vtxId0 = faces[i].vertices[0];
//				//unsigned vtxId1 = faces[i].vertices[1];
//		
////			free(innervtx);		//> メモリの解放	//配列は動的メモリではないのでfreeで開放できない
//		
//		
//			////> 1-2で交点を作っているとき	//
//			//else if( (r <= vertices[faces[i].vertices[1]].disFromOrigin && vertices[faces[i].vertices[1]].disFromOrigin <= R) ^ (vertices[faces[i].vertices[2]].disFromOrigin && vertices[faces[i].vertices[2]].disFromOrigin <= R)){
//			//	//> 円環との交点を求める
//			//	int katoon00 =0;
//			//}
//			////> 0-2で交点を作っているとき
//			//else if( (r <= vertices[faces[i].vertices[0]].disFromOrigin && vertices[faces[i].vertices[0]].disFromOrigin <= R) ^ (vertices[faces[i].vertices[2]].disFromOrigin && vertices[faces[i].vertices[2]].disFromOrigin <= R)){
//			//	//> 円環との交点を求める
//			//	int katoon000 =0;
//			//}
//
//			//for(unsigned j=0;j<3;j++){
//			//	if( (r <= vertices[faces[i].vertices[j]].disFromOrigin && vertices[faces[i].vertices[j]].disFromOrigin <= R) * (r <= vertices[faces[i].vertices[j+1]].disFromOrigin && vertices[faces[i].vertices[j+1]].disFromOrigin <= R)){
//			//		if(r <= vertices[faces[i].vertices[j]].disFromOrigin && vertices[faces[i].vertices[j]].disFromOrigin <= R)
//			//		nObinnerVtx += 1;
//			//	}
//			//}
//			//if( nObinnerVtx == 1)			faces[i].fluxarea = 1.5/3.0 * faces[i].area;//faces[i].fluxarea = 1.0/3.0 * faces[i].area;
//			//else if(nObinnerVtx == 2)		faces[i].fluxarea = 2.8/3.0 * faces[i].area;//faces[i].fluxarea = 2.0/3.0 * faces[i].area;
//			//else if(nObinnerVtx == 3)		faces[i].fluxarea = faces[i].area;
//			//else if(nObinnerVtx == 0)		faces[i].fluxarea = 0;
//
//			//if(faces[i].fluxarea >= 0){	
//			//	faceS += faces[i].fluxarea;
//			//}else{	assert(0);	}		//	faces[i].fluxareaに0未満の数字が入っているのに加算しようとしている
//			//DSTR << "faces[" << i << "].fluxarea: " << faces[i].fluxarea << std::endl;	
	}

	//for(unsigned i=0;i < nSurfaceFace; i++){
	//	DSTR << "faces[" << i << "].fluxarea: " << faces[i].fluxarea << std::endl;
	//}

		if(faceS > 0){
			//> dqdt を単位面積あたりに直す([1/m^2])
			double dqdtds = dqdtAll / faceS;
	//		DSTR << "dqdtds:  " << dqdtds << std::endl;
			//>	以下、熱流束をfacesに格納する
			//>	熱流束の面積計算はfluxareaを用いて行う
			for(unsigned i=0;i < nSurfaceFace; i++){
				if(faces[i].mayIHheated){
					faces[i].heatflux = dqdtds * faces[i].fluxarea;		//	熱流束の量をheatfluxの面積から計算
	//				DSTR << "faces[" << i <<"].heatflux: " << faces[i].heatflux <<std::endl;
				}
			}
		}
	
		//　以上、値は入っているようだ

		int katoon =0;
		//↑をつかって、CreateMatk2tをコピーした関数で、Vecf2?を作る基に

		//>	熱量は、dqdtdsを用いる

		//> r <= <= Rの中心から放射状に加熱

		//	節点でdqdtの値を更新する

		//　以下は、ベクトルを作る関数の仕事
		//	節点の属する表面の面で、計算する
		//  vertices[].heatFluxValueを基に計算を進める
		//	ガウスザイデル計算できるように処理など、準備する
	}
}


void PHFemMeshThermo::CalcIHdqdt3(double r,double R,double dqdtAll){
	///	内半径と外半径の間の節点に熱流束境界条件を設定

	//> 加熱する四面体面の面積の総和を求める
	double faceS = 0.0;
	for(unsigned i=0;i < nSurfaceFace; i++){
		if(faces[i].mayIHheated){			// faceの節点のy座標が負の場合→IH加熱の対象節点
			unsigned nObinnerVtx = 0;
			if(faces[i].area==0) faces[i].area = CalcTriangleArea(faces[i].vertices[0],faces[i].vertices[1],faces[i].vertices[2]);
			for(unsigned j=0;j<3;j++){
				if( r <= vertices[faces[i].vertices[j]].disFromOrigin && vertices[faces[i].vertices[j]].disFromOrigin <= R){
					nObinnerVtx += 1;
				}
			}
			if( nObinnerVtx == 1)			faces[i].fluxarea = 1.5/3.0 * faces[i].area;//faces[i].fluxarea = 1.0/3.0 * faces[i].area;
			else if(nObinnerVtx == 2)		faces[i].fluxarea = 2.8/3.0 * faces[i].area;//faces[i].fluxarea = 2.0/3.0 * faces[i].area;
			else if(nObinnerVtx == 3)		faces[i].fluxarea = faces[i].area;
			else if(nObinnerVtx == 0)		faces[i].fluxarea = 0;

			if(faces[i].fluxarea >= 0){	
				faceS += faces[i].fluxarea;
			}else{		assert(0);	}		//	faces[i].fluxareaに0未満の数字が入っているのに加算しようとしている
			//DSTR << "faces[" << i << "].fluxarea: " << faces[i].fluxarea << std::endl;
		}
	}

	//for(unsigned i=0;i < nSurfaceFace; i++){
	//	DSTR << "faces[" << i << "].fluxarea: " << faces[i].fluxarea << std::endl;
	//}

	if(faceS > 0){
		//> dqdt を単位面積あたりに直す([1/m^2])
		double dqdtds = dqdtAll / faceS;
//		DSTR << "dqdtds:  " << dqdtds << std::endl;
		//>	以下、熱流束をfacesに格納する
		//>	熱流束の面積計算はfluxareaを用いて行う
		for(unsigned i=0;i < nSurfaceFace; i++){
			if(faces[i].mayIHheated){
				faces[i].heatflux = dqdtds * faces[i].fluxarea;		//	熱流束の量をheatfluxの面積から計算
//				DSTR << "faces[" << i <<"].heatflux: " << faces[i].heatflux <<std::endl;
			}
		}
	}
	
	//　以上、値は入っているようだ

	int katoon =0;
	//↑をつかって、CreateMatk2tをコピーした関数で、Vecf2?を作る基に

	//>	熱量は、dqdtdsを用いる

	//> r <= <= Rの中心から放射状に加熱

	//	節点でdqdtの値を更新する

	//　以下は、ベクトルを作る関数の仕事
	//	節点の属する表面の面で、計算する
	//  vertices[].heatFluxValueを基に計算を進める
	//	ガウスザイデル計算できるように処理など、準備する

}


void PHFemMeshThermo::CalcIHdqdt2(double r,double R,double dqdtAll){
	///	内半径と外半径の間の節点に熱流束境界条件を設定
	//> 円環で区切られる四面体面の領域を三角形で近似する

	//> 加熱する四面体面の面積の総和を求める

	//%%	手順 
	//> isHeated(->nearIHHeater)のfacesの内、3つの節点全部について、特定半径の中に入っているものがあるかどうかをチェックする。
	//>	入っているものを、見つけたら、面積計算をしたいが、ここで、何個の節点が入っているかによって場合分けを行う。
	//>	３つ:faceの面積をfaceSに代入する	1~2つ:面積を近似計算するために、3節点の内、どの節点と節点で構成された辺が内半径rまたは、外半径Rと交わっているか判定
	//>	交わっている場合には、その交点を求める。求めた交点と内半径以上外半径以内の領域にあるface内の節点を用いて、三角形を作り、その面積を求める。

	double faceS = 0.0;
	for(unsigned i=0;i < nSurfaceFace; i++){
		if(faces[i].mayIHheated){			// faceの節点のy座標が負の場合→IH加熱の対象節点 円環の範囲内に入っているとは限らない
			unsigned nObinnerVtx = 0;
			unsigned inner[3] = {0,0,0};
			unsigned judge[2] = {0,0};		///	judge[0],[1]の順に原点に近い点の判定結果
			if(faces[i].area==0) faces[i].area = CalcTriangleArea(faces[i].vertices[0],faces[i].vertices[1],faces[i].vertices[2]);		// 面積計算が済んでなければ計算する
				//> 置き換えと0,1を入れ替えるだけ(=ポリモーフィズム)で残りの実装も作る
				// 下記処理を関数化する?。点のIDを入れれば、同じ処理をする関数
				//> 0-1で交点を作っているとき true false は論演算の排他的論理和XORでtrue時、交点を計算する 00=0,01=10=1,11=0; 片方の点が範囲内で、もう一方が範囲外の場合、trueになる
			
				//> 円環の範囲内に入っていたら、mayIHheatedをtrueに、あるいは、falseにする(一応)

				/// faces[i]の頂点が何個、領域内に入っているかを見つける
				for(unsigned j=0;j<3;j++){
					if( r <= vertices[faces[i].vertices[j]].disFromOrigin && vertices[faces[i].vertices[j]].disFromOrigin <= R){
						nObinnerVtx += 1;
						inner[j] = 1;
					}
				}

				//> nObinnerVtxの値で条件分岐
			
				///	
				if(nObinnerVtx == 1){
					Vec2d vtxXY[3];			/// faceの辺と半径r,Rとの交点のx,z座標　最大３点
					for(unsigned j=0;j<3;j++){		///	faceを構成する3頂点について
						unsigned k = 0;
						unsigned m = 0;
						k = (j+1)%3;		///	j=0;k=1, j=1;k=2, j=2;k=0 
						m = (j+2)%3;
						//vtxXY[j] = CalcIntersectionPoint(faces[i].vertices[j],faces[i].vertices[k],r,R);
						//DSTR << "vtxXY[" << j << "]: " << vtxXY[j] << std::endl; 
						if(inner[j] ==1){			/// faces[i]のj番目の節点が円環領域内に入っている
						/// j番目の頂点とエッジを構成する点(他の２点)を使って、半径r,Rと交わる点を算出する						
						//> j 0,1,2
							if(j == 0){	/// 頂点jと辺を作る頂点を使って、辺と交わる交点を求める
								//k = (j+1)%3;		///	j=0;k=1, j=1;k=2, j=2;k=0 
							vtxXY[j] = CalcIntersectionPoint(faces[i].vertices[j],faces[i].vertices[k],r,R);
							vtxXY[k] = CalcIntersectionPoint(faces[i].vertices[j],faces[i].vertices[m],r,R);
							}
							else if(j == 1){
							vtxXY[j] = CalcIntersectionPoint(faces[i].vertices[j],faces[0].vertices[1],r,R);
							vtxXY[j] = CalcIntersectionPoint(faces[i].vertices[j],faces[2].vertices[2],r,R);
							}
							else if(j == 2){
							vtxXY[j] = CalcIntersectionPoint(faces[i].vertices[j],faces[0].vertices[1],r,R);
							vtxXY[j] = CalcIntersectionPoint(faces[i].vertices[j],faces[1].vertices[2],r,R);
							}
							else{
								assert(0);
								DSTR << "Error in PHFemMeshThermo::CalcIHdqdt2(hogehoge) 領域判定にミス" << std::endl;
							}
						 }
					}
				}

				//for(unsigned j=0;j<3;j++){
				//	/// face内の節点を順番にチェックする
				//	unsigned k =0;
				//	k = (j+1)%3;		///	j=0;k=1, j=1;k=2, j=2;k=0 
				//	if( r <= vertices[faces[i].vertices[j]].disFromOrigin && vertices[faces[i].vertices[j]].disFromOrigin <= R){
				//	}
				//	if( r <= vertices[faces[i].vertices[j]].disFromOrigin && vertices[faces[i].vertices[j]].disFromOrigin <= R){
				//		//> j,kをfaces[i].vertices[j],faces[i].vertices[k]として代入
				//		CalcIntersectionPoint(faces[i].vertices[0],faces[i].vertices[1],r,R);
				//	}
				//}

				//> r -> radius
				//> R -> Radius
			
				/// 座標(consX ,constY)が円と三角形の辺との交点
			
				//unsigned vtxId0 = faces[i].vertices[0];
				//unsigned vtxId1 = faces[i].vertices[1];
			}
			////> 1-2で交点を作っているとき	//
			//else if( (r <= vertices[faces[i].vertices[1]].disFromOrigin && vertices[faces[i].vertices[1]].disFromOrigin <= R) ^ (vertices[faces[i].vertices[2]].disFromOrigin && vertices[faces[i].vertices[2]].disFromOrigin <= R)){
			//	//> 円環との交点を求める
			//	int katoon00 =0;
			//}
			////> 0-2で交点を作っているとき
			//else if( (r <= vertices[faces[i].vertices[0]].disFromOrigin && vertices[faces[i].vertices[0]].disFromOrigin <= R) ^ (vertices[faces[i].vertices[2]].disFromOrigin && vertices[faces[i].vertices[2]].disFromOrigin <= R)){
			//	//> 円環との交点を求める
			//	int katoon000 =0;
			//}

			//for(unsigned j=0;j<3;j++){
			//	if( (r <= vertices[faces[i].vertices[j]].disFromOrigin && vertices[faces[i].vertices[j]].disFromOrigin <= R) * (r <= vertices[faces[i].vertices[j+1]].disFromOrigin && vertices[faces[i].vertices[j+1]].disFromOrigin <= R)){
			//		if(r <= vertices[faces[i].vertices[j]].disFromOrigin && vertices[faces[i].vertices[j]].disFromOrigin <= R)
			//		nObinnerVtx += 1;
			//	}
			//}
			//if( nObinnerVtx == 1)			faces[i].fluxarea = 1.5/3.0 * faces[i].area;//faces[i].fluxarea = 1.0/3.0 * faces[i].area;
			//else if(nObinnerVtx == 2)		faces[i].fluxarea = 2.8/3.0 * faces[i].area;//faces[i].fluxarea = 2.0/3.0 * faces[i].area;
			//else if(nObinnerVtx == 3)		faces[i].fluxarea = faces[i].area;
			//else if(nObinnerVtx == 0)		faces[i].fluxarea = 0;

			//if(faces[i].fluxarea >= 0){	
			//	faceS += faces[i].fluxarea;
			//}else{	assert(0);	}		//	faces[i].fluxareaに0未満の数字が入っているのに加算しようとしている
			//DSTR << "faces[" << i << "].fluxarea: " << faces[i].fluxarea << std::endl;
		}

	//for(unsigned i=0;i < nSurfaceFace; i++){
	//	DSTR << "faces[" << i << "].fluxarea: " << faces[i].fluxarea << std::endl;
	//}

	if(faceS > 0){
		//> dqdt を単位面積あたりに直す([1/m^2])
		double dqdtds = dqdtAll / faceS;
//		DSTR << "dqdtds:  " << dqdtds << std::endl;
		//>	以下、熱流束をfacesに格納する
		//>	熱流束の面積計算はfluxareaを用いて行う
		for(unsigned i=0;i < nSurfaceFace; i++){
			if(faces[i].mayIHheated){
				faces[i].heatflux = dqdtds * faces[i].fluxarea;		//	熱流束の量をheatfluxの面積から計算
//				DSTR << "faces[" << i <<"].heatflux: " << faces[i].heatflux <<std::endl;
			}
		}
	}
	
	//　以上、値は入っているようだ

	int katoon =0;
	//↑をつかって、CreateMatk2tをコピーした関数で、Vecf2?を作る基に

	//>	熱量は、dqdtdsを用いる

	//> r <= <= Rの中心から放射状に加熱

	//	節点でdqdtの値を更新する

	//　以下は、ベクトルを作る関数の仕事
	//	節点の属する表面の面で、計算する
	//  vertices[].heatFluxValueを基に計算を進める
	//	ガウスザイデル計算できるように処理など、準備する

}



void PHFemMeshThermo::CalcIHdqdt(double r,double R,double dqdtAll){
	///	内半径と外半径の間の節点に熱流束境界条件を設定

	//> 加熱する四面体面の面積の総和を求める
	double faceS = 0.0;
	for(unsigned i=0;i < nSurfaceFace; i++){
		if(faces[i].mayIHheated){			// faceの節点のy座標が負の場合→IH加熱の対象節点
			unsigned nObinnerVtx = 0;
			if(faces[i].area==0) faces[i].area = CalcTriangleArea(faces[i].vertices[0],faces[i].vertices[1],faces[i].vertices[2]);
			for(unsigned j=0;j<3;j++){
				if( r <= vertices[faces[i].vertices[j]].disFromOrigin && vertices[faces[i].vertices[j]].disFromOrigin <= R){
					nObinnerVtx += 1;
				}
			}
			if( nObinnerVtx == 1)			faces[i].fluxarea = 1.5/3.0 * faces[i].area;//faces[i].fluxarea = 1.0/3.0 * faces[i].area;
			else if(nObinnerVtx == 2)		faces[i].fluxarea = 2.8/3.0 * faces[i].area;//faces[i].fluxarea = 2.0/3.0 * faces[i].area;
			else if(nObinnerVtx == 3)		faces[i].fluxarea = faces[i].area;
			else if(nObinnerVtx == 0)		faces[i].fluxarea = 0;

			if(faces[i].fluxarea >= 0){	
				faceS += faces[i].fluxarea;
			}else{		assert(0);	}		//	faces[i].fluxareaに0未満の数字が入っているのに加算しようとしている
			//DSTR << "faces[" << i << "].fluxarea: " << faces[i].fluxarea << std::endl;
		}
	}

	//for(unsigned i=0;i < nSurfaceFace; i++){
	//	DSTR << "faces[" << i << "].fluxarea: " << faces[i].fluxarea << std::endl;
	//}

	if(faceS > 0){
		//> dqdt を単位面積あたりに直す([1/m^2])
		double dqdtds = dqdtAll / faceS;
//		DSTR << "dqdtds:  " << dqdtds << std::endl;
		//>	以下、熱流束をfacesに格納する
		//>	熱流束の面積計算はfluxareaを用いて行う
		for(unsigned i=0;i < nSurfaceFace; i++){
			if(faces[i].mayIHheated){
				faces[i].heatflux = dqdtds * faces[i].fluxarea;		//	熱流束の量をheatfluxの面積から計算
//				DSTR << "faces[" << i <<"].heatflux: " << faces[i].heatflux <<std::endl;
			}
		}
	}
	
	//　以上、値は入っているようだ

	int katoon =0;
	//↑をつかって、CreateMatk2tをコピーした関数で、Vecf2?を作る基に

	//>	熱量は、dqdtdsを用いる

	//> r <= <= Rの中心から放射状に加熱

	//	節点でdqdtの値を更新する

	//　以下は、ベクトルを作る関数の仕事
	//	節点の属する表面の面で、計算する
	//  vertices[].heatFluxValueを基に計算を進める
	//	ガウスザイデル計算できるように処理など、準備する

}
/// face毎に作ってしまうのが良いのか、verticesごとにやるのがいいのか。どっちがいいか分からないので、ひとまず、vertices毎に作ってしまおう

void PHFemMeshThermo::SetVertexHeatFlux(int id,double heatFlux){
	vertices[id].heatFluxValue = heatFlux;
}

void PHFemMeshThermo::SetVtxHeatFluxAll(double heatFlux){
	for(unsigned i=0; i < vertices.size() ;i++){
		SetVertexHeatFlux(i,heatFlux);
	}
}

void PHFemMeshThermo::CalcHeatTransUsingGaussSeidel(unsigned NofCyc,double dt){
	//dt = 0.0000000000001 * dt;		//デバッグ用に、dtをものすごく小さくしても、節点0がマイナスになるのか、調べた
	
	//dtはPHFemEngine.cppで取得する動力学シミュレーションのステップ時間
	bool DoCalc =true;											//初回だけ定数ベクトルbの計算を行うbool		//NofCycが0の時にすればいいのかも
	std::ofstream ofs("log.txt");
	for(unsigned i=0; i < NofCyc; i++){							//ガウスザイデルの計算ループ
		if(DoCalc){		
			if(deformed || alphaUpdated){												//D_iiの作成　形状や熱伝達率が更新された際に1度だけ行えばよい
				for(unsigned j =0; j < vertices.size() ; j++){
					//for(unsigned k =0;k < vertices.size(); k++){
					//	DSTR << "dMatCAll "<< k << " : " << dMatCAll[0][k] << std::endl;
					//}
					_dMatAll.resize(1,vertices.size());
					_dMatAll[0][j] = 1.0/ ( 1.0/2.0 * dMatKAll[0][j] + 1.0/dt * dMatCAll[0][j] );		//1 / D__ii	を求める
					//1.0/dt = 500 d
					//DSTR << "dMatKAll : "  << dMatKAll << std::endl;
					//DSTR << "dMatCAll : "  << dMatCAll << std::endl;
					//DSTR << "1.0/dt : " << 1.0/dt <<std::endl;
					//DSTR <<  1.0/dt *dMatCAll[0][j] << std::endl;		//0.001のオーダー
					//DSTR << 1.0/2.0 * dMatKAll[0][j] << std::endl;		//0.0003前後のオーダー
					//値が入っているかをチェック
					//DSTR << "_dMatAll[0][" << j << "] : " << _dMatAll[0][j]  << std::endl;
					int debughogeshi =0;
				}
				deformed = false;
				alphaUpdated =false;
				//	for DEBUG
				//DSTR << "_dMatAll : " << std::endl;
				//for(unsigned j =0; j < vertices.size() ;j++){
				//	DSTR << j << " : " << _dMatAll[0][j] << std::endl;
				//}
				//int hogeshi=0;
			}
			//	 1      1        1  
			//	--- ( - - [K] + ---[C] ){T(t)} + {F} 
			//	D_jj    2       �冲
			//

			for(unsigned j =0; j < vertices.size() ; j++){		//初回ループだけ	係数ベクトルbVecAllの成分を計算
				bVecAll[j][0] = 0.0;							//bVecAll[j][0]の初期化
				//節点が属すedges毎に　対角成分(j,j)と非対角成分(j,?)毎に計算
				//対角成分は、vertices[j].k or .c に入っている値を、非対角成分はedges[hoge].vertices[0] or vertices[1] .k or .cに入っている値を用いる
				//�@)非対角成分について
				for(unsigned k =0;k < vertices[j].edges.size() ; k++){
					unsigned edgeId = vertices[j].edges[k];
					//リファクタリング	以下の条件分岐についてj>edges[edgeId].vertices[0] とそうでない時とで分けたほうが漏れが出る心配はない？
					if( j != edges[edgeId].vertices[0]){					//節点番号jとedges.vertices[0]が異なる節点番号の時:非対角成分
						unsigned vtxid0 = edges[edgeId].vertices[0];
						bVecAll[j][0] += (-1.0/2.0 * edges[edgeId].k + 1.0/dt * edges[edgeId].c ) * TVecAll[vtxid0];
					}
					else if( j != edges[edgeId].vertices[1] ){			//節点番号jとedges.vertices[1]が異なる節点番号の時:非対角成分
						unsigned vtxid1 = edges[edgeId].vertices[1];
						bVecAll[j][0] += (-1.0/2.0 * edges[edgeId].k + 1.0/dt * edges[edgeId].c ) * TVecAll[vtxid1];

					}
					else{
						//上記のどちらでもない場合、エラー
						DSTR << "edges.vertex has 3 vertexies or any other problem" <<std::endl;
					}
					//	for Debug
					//DSTR << "edges[" << edgeId << "].vertices[0] : " << edges[edgeId].vertices[0] << std::endl;
					//DSTR << "edges[" << edgeId << "].vertices[1] : " << edges[edgeId].vertices[1] << std::endl;
					//int hogeshi =0;
				}
				//�A)対角成分について
				bVecAll[j][0] += (-1.0/2.0 * dMatKAll[0][j] + 1.0/dt * dMatCAll[0][j] ) * TVecAll[j];
				ofs << "bVecAll[" << j <<"][0] : " << bVecAll[j][0] << std::endl;			// DSTR
				//{F}を加算
				bVecAll[j][0] += vecFAll[j][0];		//Fを加算
				//DSTR << " vecFAll[" << j << "][0] : "  << vecFAll[j][0] << std::endl;
				//DSTR << std::endl;
				//D_iiで割る ⇒この場所は、ここで良いの？どこまで掛け算するの？
				bVecAll[j][0] = bVecAll[j][0] * _dMatAll[0][j];
				ofs << "bVecAll[" << j <<"][0] * _dMatAll : " << bVecAll[j][0] << std::endl;
				//	DSTR <<  "bVecAll[" << j <<"][0] * _dMatAll : " << bVecAll[j][0] << std::endl;
				ofs << "TVecAll[" << j <<"] : " << TVecAll[j] << std::endl;
				//	DSTR << "TVecAll[" << j <<"] : " << TVecAll[j] << std::endl;
			}
			DoCalc = false;			//初回のループだけで利用
			//値が入っているか、正常そうかをチェック
			//DSTR << "bVecAll[j][0] : " << std::endl;
			//for(unsigned j =0;j <vertices.size() ; j++){
			//	DSTR << j << " : "<< bVecAll[j][0] << std::endl;
			//}
			int debughogeshi =0;
		}		//if(DoCalc){...}

#ifdef DEBUG
		//	念のため、計算前の初期温度を0にしている。
		if(i == 0){
				for(unsigned j=0;j <vertices.size() ;j++){
					TVecAll[j] = 0.0;
				}
		}
#endif
		//	 1      
		//	--- [F]{T(t+dt)}
		//	D_jj 		
		//[F] = 1/2 [K] +1/dt [C] から対角成分を除し(-1)をかけたもの
		//エッジに入っている成分に-1をかけるのではなく、最後に-1をかける。
		//
		for(unsigned j =0; j < vertices.size() ; j++){
			//T(t+dt) = の式
			//	まずtempkjを作る
			double tempkj = 0.0;			//ガウスザイデルの途中計算で出てくるFの成分計算に使用する一時変数
			for(unsigned k =0;k < vertices[j].edges.size() ; k++){
				unsigned edgeId = vertices[j].edges[k]; 
				if( j != edges[edgeId].vertices[0]){					//節点番号jとedges.vertices[0]が異なる節点番号の時:非対角成分		//OK
					unsigned vtxid0 = edges[edgeId].vertices[0];
					//DSTR << "TVecAll["<< vtxid0<<"] : " << TVecAll[vtxid0] <<std::endl;
					//TVecAll[j] +=_dMatAll[j][0] * -(1.0/2.0 * edges[edgeId].k + 1.0/dt * edges[edgeId].c ) * TVecAll[vtxid0] + bVecAll[j][0]; 
					//DSTR << "j : " << j << ", vtxid0 : " << vtxid0 <<", edges[edgeId].vertices[0] : " << edges[edgeId].vertices[0] <<  std::endl;
					tempkj += (1.0/2.0 * edges[edgeId].k + 1.0/dt * edges[edgeId].c ) * TVecAll[vtxid0];
				}
				else if( j != edges[edgeId].vertices[1] ){			//節点番号jとedges.vertices[1]が異なる節点番号の時:非対角成分
					unsigned vtxid1 = edges[edgeId].vertices[1];
					//DSTR << "TVecAll["<< vtxid1<<"] : " << TVecAll[vtxid1] <<std::endl;
					tempkj += (1.0/2.0 * edges[edgeId].k + 1.0/dt * edges[edgeId].c ) * TVecAll[vtxid1];
				}
				else{
					//上記のどちらでもない場合、エラー
					DSTR << "edges.vertex has 3 vertexies or any other problem" <<std::endl;
				}
				//	for Debug
				//DSTR << "TVecAll:"
				//DSTR << "edges[" << edgeId << "].vertices[0] : " << edges[edgeId].vertices[0] << std::endl;
				//DSTR << "edges[" << edgeId << "].vertices[1] : " << edges[edgeId].vertices[1] << std::endl;
				//int hogeshi =0;
			}
			//	TVecAllの計算
			TVecAll[j] =	_dMatAll[0][j] * ( -1.0 * tempkj) + bVecAll[j][0];			//	-b = D^(-1) [ (-1/2 * K + 1/dt * C ){T(t+dt)} + {F} ]なので、bVecAllはただの加算でよい
			//TVecAll[j] =	_dMatAll[0][j] * ( -1.0 * tempkj) + bVecAll[j][0];   // -b = D^(-1) [ (-1/2 * K + 1/dt * C ){T(t+dt)} + {F} ]なので、bVecAllはただの加算でよい
//			TVecAll[j] =	_dMatAll[0][j] * ( -1.0 * tempkj) + bVecAll[j][0];			//この計算式だと、まともそうな値が出るが・・・理論的にはどうなのか、分からない。。。
			////	for DEBUG
			//int hofgeshi =0;
			//if(TVecAll[j] != 0.0){
			//	DSTR << "!=0 TVecAll["<< j<<"] : " << TVecAll[j] <<std::endl;
			//}
			//DSTR << i << "回目の計算、" << j <<"行目のtempkj: " << tempkj << std::endl;
			//tempkj =0.0;
			ofs << j << std::endl;
			ofs << "tempkj: "<< tempkj << std::endl;
			ofs << "DMatAll[0][j] * ( -1.0 * tempkj) :" <<_dMatAll[0][j] * ( -1.0 * tempkj) << std::endl;
			ofs << "bVecAll[j][0] :  " <<  bVecAll[j][0] << std::endl;
			ofs << "  TVecAll[j] : " << TVecAll[j] << std::endl;
			ofs << std::endl;
		}
		////	for Debug
		//for(unsigned j=0;j < vertices.size();j++){
		//	//DSTR << "tempk" << j << " : " << tempkj << std::endl;
		//	int hogeshi__ =0;
		//	//TVecAll[j]の計算結果を代入する
		//	//定数ベクトルbを上で計算、毎行でbVecAllを減算すればよい。
		//	DSTR << i << "回目の計算の " << "TVecAll[" << j << "] : " << TVecAll[j] << std::endl;
		//}
		//for(unsigned j=0;j < vertices.size();j++){
		//	//DSTR << "tempk" << j << " : " << tempkj << std::endl;
		//	int hogeshi__ =0;
		//	//TVecAll[j]の計算結果を代入する
		//	//定数ベクトルbを上で計算、毎行でbVecAllを減算すればよい。
		//	DSTR << i << "回目の計算の " << "bVecAll[" << j << "][0] : " << bVecAll[j][0] << std::endl;
		//}

		//DSTR << i <<  "th Cyc" << std::endl; 
		//DSTR << i << "回目の計算、TVecAll : " <<std::endl;
		//DSTR << TVecAll << std::endl;
		ofs << i <<  "th Cyc" << std::endl;
		ofs << i << "回目の計算、TVecAll : " <<std::endl;
		ofs << TVecAll << std::endl;
		ofs << "bVecAll: " <<std::endl;
		ofs << bVecAll << std::endl;
		ofs << "_dMatAll: " <<std::endl; 
		ofs << _dMatAll <<std::endl;
		int piyopiyoyo =0;
		double tempTemp=0.0;
		for(unsigned j=0;j <vertices.size() ; j++){
			tempTemp += TVecAll[j];
		}
		//	DSTR
		ofs << i <<"回目の計算時の　全節点の温度の和 : " << tempTemp << std::endl;
		ofs << std::endl;
	}
//	deformed = true;
}

void PHFemMeshThermo::CalcHeatTransUsingGaussSeidel(unsigned NofCyc,double dt,double eps){
	//dt = 0.0000000000001 * dt;		//デバッグ用に、dtをものすごく小さくしても、節点0がマイナスになるのか、調べた
	double _eps = 1-eps;			// 1-epsの計算に利用
	//dtはPHFemEngine.cppで取得する動力学シミュレーションのステップ時間
	bool DoCalc =true;											//初回だけ定数ベクトルbの計算を行うbool		//NofCycが0の時にすればいいのかも
	std::ofstream ofs("log.txt");
	for(unsigned i=0; i < NofCyc; i++){							//ガウスザイデルの計算ループ
		if(DoCalc){												
			if(deformed || alphaUpdated ){												//D_iiの作成　形状が更新された際に1度だけ行えばよい
				for(unsigned j =0; j < vertices.size() ; j++){
					//for(unsigned k =0;k < vertices.size(); k++){
					//	DSTR << "dMatCAll "<< k << " : " << dMatCAll[0][k] << std::endl;
					//}
					_dMatAll.resize(1,vertices.size());
					_dMatAll[0][j] = 1.0/ ( eps * dMatKAll[0][j] + 1.0/dt * dMatCAll[0][j] );		//1 / D__ii	を求める
					//1.0/dt = 500 d
					//DSTR << "dMatKAll : "  << dMatKAll << std::endl;
					//DSTR << "dMatCAll : "  << dMatCAll << std::endl;
					//DSTR << "1.0/dt : " << 1.0/dt <<std::endl;
					//DSTR <<  1.0/dt *dMatCAll[0][j] << std::endl;		//0.001のオーダー
					//DSTR << 1.0/2.0 * dMatKAll[0][j] << std::endl;		//0.0003前後のオーダー
					//値が入っているかをチェック
					//DSTR << "_dMatAll[0][" << j << "] : " << _dMatAll[0][j]  << std::endl;
					int debughogeshi =0;
				}
				deformed = false;
				//	for DEBUG
				//DSTR << "_dMatAll : " << std::endl;
				//for(unsigned j =0; j < vertices.size() ;j++){
				//	DSTR << j << " : " << _dMatAll[0][j] << std::endl;
				//}
				//int hogeshi=0;
			}
			//	 1      1        1  
			//	--- ( - - [K] + ---[C] ){T(t)} + {F} 
			//	D_jj    2       �冲
			//

			for(unsigned j =0; j < vertices.size() ; j++){		//初回ループだけ	係数ベクトルbVecAllの成分を計算
				bVecAll[j][0] = 0.0;							//bVecAll[j][0]の初期化
				//節点が属すedges毎に　対角成分(j,j)と非対角成分(j,?)毎に計算
				//対角成分は、vertices[j].k or .c に入っている値を、非対角成分はedges[hoge].vertices[0] or vertices[1] .k or .cに入っている値を用いる
				//�@)非対角成分について
				for(unsigned k =0;k < vertices[j].edges.size() ; k++){
					unsigned edgeId = vertices[j].edges[k];
					//リファクタリング	以下の条件分岐についてj>edges[edgeId].vertices[0] とそうでない時とで分けたほうが漏れが出る心配はない？
					if( j != edges[edgeId].vertices[0]){					//節点番号jとedges.vertices[0]が異なる節点番号の時:非対角成分
						unsigned vtxid0 = edges[edgeId].vertices[0];
						bVecAll[j][0] += (-_eps * edges[edgeId].k + 1.0/dt * edges[edgeId].c ) * TVecAll[vtxid0];
					}
					else if( j != edges[edgeId].vertices[1] ){			//節点番号jとedges.vertices[1]が異なる節点番号の時:非対角成分
						unsigned vtxid1 = edges[edgeId].vertices[1];
						bVecAll[j][0] += (-_eps * edges[edgeId].k + 1.0/dt * edges[edgeId].c ) * TVecAll[vtxid1];

					}
					else{
						//上記のどちらでもない場合、エラー
						DSTR << "edges.vertex has 3 vertexies or any other problem" <<std::endl;
					}
					//	for Debug
					//DSTR << "edges[" << edgeId << "].vertices[0] : " << edges[edgeId].vertices[0] << std::endl;
					//DSTR << "edges[" << edgeId << "].vertices[1] : " << edges[edgeId].vertices[1] << std::endl;
					//int hogeshi =0;
				}
				//�A)対角成分について
				bVecAll[j][0] += (-_eps * dMatKAll[0][j] + 1.0/dt * dMatCAll[0][j] ) * TVecAll[j];
				ofs << "bVecAll[" << j <<"][0] : " << bVecAll[j][0] << std::endl;			// DSTR
				ofs << "dMatKAll[0][" << j <<"] : " << dMatKAll[0][j] << std::endl;			// DSTR
				ofs << "dMatCAll[0][" << j <<"] : " << dMatCAll[0][j] << std::endl;			// DSTR
				//{F}を加算
				bVecAll[j][0] += vecFAll[j][0];		//Fを加算
				//DSTR << " vecFAll[" << j << "][0] : "  << vecFAll[j][0] << std::endl;
				//DSTR << std::endl;
				//D_iiで割る ⇒この場所は、ここで良いの？どこまで掛け算するの？
				bVecAll[j][0] = bVecAll[j][0] * _dMatAll[0][j];
				ofs << "bVecAll[" << j <<"][0] * _dMatAll : " << bVecAll[j][0] << std::endl;
				//	DSTR <<  "bVecAll[" << j <<"][0] * _dMatAll : " << bVecAll[j][0] << std::endl;
				ofs << "TVecAll[" << j <<"] : " << TVecAll[j] << std::endl;
				//	DSTR << "TVecAll[" << j <<"] : " << TVecAll[j] << std::endl;
			}
			DoCalc = false;			//初回のループだけで利用
			//値が入っているか、正常そうかをチェック
			//DSTR << "bVecAll[j][0] : " << std::endl;
			//for(unsigned j =0;j <vertices.size() ; j++){
			//	DSTR << j << " : "<< bVecAll[j][0] << std::endl;
			//}
			int debughogeshi =0;
		}		//if(DoCalc){...}

#ifdef DEBUG
		//	念のため、計算前の初期温度を0にしている。
		if(i == 0){
				for(unsigned j=0;j <vertices.size() ;j++){
					TVecAll[j] = 0.0;
				}
		}
#endif
		//	 1      
		//	--- [F]{T(t+dt)}
		//	D_jj 		
		//[F] = eps(ilon) [K] +1/dt [C] から対角成分を除し(-1)をかけたもの
		//エッジに入っている成分に-1をかけるのではなく、最後に-1をかける。
		//
		for(unsigned j =0; j < vertices.size() ; j++){
			//T(t+dt) = の式
			//	まずtempkjを作る
			double tempkj = 0.0;			//ガウスザイデルの途中計算で出てくるFの成分計算に使用する一時変数
			for(unsigned k =0;k < vertices[j].edges.size() ; k++){
				unsigned edgeId = vertices[j].edges[k]; 
				if( j != edges[edgeId].vertices[0]){					//節点番号jとedges.vertices[0]が異なる節点番号の時:非対角成分		//OK
					unsigned vtxid0 = edges[edgeId].vertices[0];
					//DSTR << "TVecAll["<< vtxid0<<"] : " << TVecAll[vtxid0] <<std::endl;
					//TVecAll[j] +=_dMatAll[j][0] * -(1.0/2.0 * edges[edgeId].k + 1.0/dt * edges[edgeId].c ) * TVecAll[vtxid0] + bVecAll[j][0]; 
					//DSTR << "j : " << j << ", vtxid0 : " << vtxid0 <<", edges[edgeId].vertices[0] : " << edges[edgeId].vertices[0] <<  std::endl;
					tempkj += (eps * edges[edgeId].k + 1.0/dt * edges[edgeId].c ) * TVecAll[vtxid0];
				}
				else if( j != edges[edgeId].vertices[1] ){			//節点番号jとedges.vertices[1]が異なる節点番号の時:非対角成分
					unsigned vtxid1 = edges[edgeId].vertices[1];
					//DSTR << "TVecAll["<< vtxid1<<"] : " << TVecAll[vtxid1] <<std::endl;
					tempkj += (eps * edges[edgeId].k + 1.0/dt * edges[edgeId].c ) * TVecAll[vtxid1];
				}
				else{
					//上記のどちらでもない場合、エラー
					DSTR << "edges.vertex has 3 vertexies or any other problem" <<std::endl;
				}
				//	for Debug
				//DSTR << "TVecAll:"
				//DSTR << "edges[" << edgeId << "].vertices[0] : " << edges[edgeId].vertices[0] << std::endl;
				//DSTR << "edges[" << edgeId << "].vertices[1] : " << edges[edgeId].vertices[1] << std::endl;
				//int hogeshi =0;
			}
			//	TVecAllの計算
			TVecAll[j] =	_dMatAll[0][j] * ( -1.0 * tempkj) + bVecAll[j][0];			//	-b = D^(-1) [ (-1/2 * K + 1/dt * C ){T(t+dt)} + {F} ]なので、bVecAllはただの加算でよい
			//TVecAll[j] =	_dMatAll[0][j] * ( -1.0 * tempkj) + bVecAll[j][0];   // -b = D^(-1) [ (-1/2 * K + 1/dt * C ){T(t+dt)} + {F} ]なので、bVecAllはただの加算でよい
//			TVecAll[j] =	_dMatAll[0][j] * ( -1.0 * tempkj) + bVecAll[j][0];			//この計算式だと、まともそうな値が出るが・・・理論的にはどうなのか、分からない。。。
			////	for DEBUG
			//int hofgeshi =0;
			//if(TVecAll[j] != 0.0){
			//	DSTR << "!=0 TVecAll["<< j<<"] : " << TVecAll[j] <<std::endl;
			//}
			//DSTR << i << "回目の計算、" << j <<"行目のtempkj: " << tempkj << std::endl;
			//tempkj =0.0;
			ofs << j << std::endl;
			ofs << "tempkj: "<< tempkj << std::endl;
			ofs << "DMatAll[0][j] * ( -1.0 * tempkj) :" <<_dMatAll[0][j] * ( -1.0 * tempkj) << std::endl;
			ofs << "bVecAll[j][0] :  " <<  bVecAll[j][0] << std::endl;
			ofs << "  TVecAll[j] : " << TVecAll[j] << std::endl;
			ofs << std::endl;
		}
		////	for Debug
		//for(unsigned j=0;j < vertices.size();j++){
		//	//DSTR << "tempk" << j << " : " << tempkj << std::endl;
		//	int hogeshi__ =0;
		//	//TVecAll[j]の計算結果を代入する
		//	//定数ベクトルbを上で計算、毎行でbVecAllを減算すればよい。
		//	DSTR << i << "回目の計算の " << "TVecAll[" << j << "] : " << TVecAll[j] << std::endl;
		//}
		//for(unsigned j=0;j < vertices.size();j++){
		//	//DSTR << "tempk" << j << " : " << tempkj << std::endl;
		//	int hogeshi__ =0;
		//	//TVecAll[j]の計算結果を代入する
		//	//定数ベクトルbを上で計算、毎行でbVecAllを減算すればよい。
		//	DSTR << i << "回目の計算の " << "bVecAll[" << j << "][0] : " << bVecAll[j][0] << std::endl;
		//}

		//DSTR << i <<  "th Cyc" << std::endl; 
		//DSTR << i << "回目の計算、TVecAll : " <<std::endl;
		//DSTR << TVecAll << std::endl;
		ofs << i <<  "th Cyc" << std::endl;
		ofs << i << "回目の計算、TVecAll : " <<std::endl;
		ofs << TVecAll << std::endl;
		ofs << "bVecAll: " <<std::endl;
		ofs << bVecAll << std::endl;
		ofs << "_dMatAll: " <<std::endl; 
		ofs << _dMatAll <<std::endl;
		int piyopiyoyo =0;
		double tempTemp=0.0;
		for(unsigned j=0;j <vertices.size() ; j++){
			tempTemp += TVecAll[j];
		}
		//	DSTR
		ofs << i <<"回目の計算時の　全節点の温度の和 : " << tempTemp << std::endl;
		ofs << std::endl;
	}
//	deformed = true;
}

void PHFemMeshThermo::UpdateVertexTempAll(unsigned size){
	for(unsigned i=0;i < size;i++){
		vertices[i].temp = TVecAll[i];
	}
}
void PHFemMeshThermo::UpdateVertexTemp(unsigned vtxid){
		vertices[vtxid].temp = TVecAll[vtxid];
}

void PHFemMeshThermo::TexChange(unsigned id,double tz){

}

void PHFemMeshThermo::HeatTransFromPanToFoodShell(){
	//if(pan){
	//	Affinef afPan = pan->GetGRFrame()->GetWorldTransform();
	//	Affinef afMesh = tmesh.obj->GetGRFrame()->GetWorldTransform();
	//	Affinef afMeshToPan = afPan.inv() * afMesh;	
	//}

	//shape pair solid pair
	

	//	2物体の接触面から、加熱する節点を決める。
	//	Shape pair のSolid pare辺りに記述がある

	//	最外殻の節点を世界座標に変換し、そのフライパン又は、鉄板を基にした座標系に変換した座標値が、ある座標以下なら
	//	最外殻の節点に熱伝達する
	//PHSolidIf* phs ;
//	Affinef afPan = phs
	//Affinef afPan = pan->GetGRFrame()->GetWorldTransform();
	//Affinef afMesh = tmesh.obj->GetGRFrame()->GetWorldTransform();
	//Affinef afMeshToPan = afPan.inv() * afMesh;	
//	for(unsigned i=-0; i < surfaceVertices.size();i++){
//		if(vertices[surfaceVertices[i]].pos){};
//		//vertices[surfaceVertices[i]].Tc;
//	}

	//	接触面からの距離が一定距離以内なら

	//	熱伝達境界条件で、熱伝達

	//	熱伝達境界条件で熱伝達後、フライパンの熱は吸熱というか、減るが、それは有限要素法をどのように結合してやればいいのか？
	
	//	節点周囲のTcと熱伝達率αによって、熱が伝わるので、ここで、フライパンから熱を伝えたい節点のTcと熱伝達率を設定する

	//	熱伝達率は、相手との関係によって異なるので、節点が何と接しているかによって変更する


}


void PHFemMeshThermo::Step(double dt){

	//std::ofstream templog("templog.txt");

	std::ofstream ofs_("log_.txt");
//	ScilabTest();									//	Scilabを使うテスト
	//境界条件を設定:温度の設定
//	UsingFixedTempBoundaryCondition(0,200.0);

	//	実験用のコード
	//	a) 時間離散化時に台形公式利用、前進・後退差分で振動する加熱方式
	//if(StepCount == 0){
	//	unsigned texid_ =7;
	//	UsingFixedTempBoundaryCondition(texid_,200.0);
	//}

	//	b) 断熱過程の実験
	//	熱伝達率を0にする。温度固定境界条件で加熱。

	//	UsingFixedTempBoundaryCondition(3,50.0);
	//for(unsigned i=0 ;i<1;i++){
	//	UsingFixedTempBoundaryCondition(i,200.0);
	//}

	//%%%%		熱伝達境界条件		%%%%//
	//	食材メッシュの表面の節点に、周囲の流体温度を与える
	//	周囲の流体温度は、フライパンの表面温度や、食材のUsingFixedTempBoundaryCondition(0,200.0);液体内の温度の分布から、その場所での周囲流体温度を判別する。
	//	位置座標から判別するコードをここに記述
	//UsingHeatTransferBoundaryCondition(unsigned id,double temp);
	//エネルギー保存則より、周囲流体温度の低下や、流体への供給熱量は制限されるべき

	//for(unsigned i =0; i < 2; i++){
	//	UsingHeatTransferBoundaryCondition(surfaceVertices[i],200.0);
	//}
//	UsingHeatTransferBoundaryCondition(7,200.0);
//	UsingHeatTransferBoundaryCondition(0,200.0);
	//for(unsigned i =0; i < surfaceVertices.size(); i++){
	//	UsingHeatTransferBoundaryCondition(surfaceVertices[i],150.0);
	//}
	//DSTR << "vecFAll : " <<std::endl;
	//DSTR << vecFAll << std::endl;

	//	test　shapepairを取ってくる
	//GetScene()->
	
	//dt = dt *0.01;		誤差1度程度になる
	//dt = dt;				収束した時の、計算誤差？（マイナスになっている節点温度がそれなりに大きくなる。）
	
	///>	ガウスザイデル法の設定
	//	CalcHeatTransUsingGaussSeidel(20,dt);			//ガウスザイデル法で熱伝導計算を解く　クランクニコルソン法のみを使いたい場合

//	dNdt = 10.0 * dt;
		CalcHeatTransUsingGaussSeidel(5,dt,1.0);			//ガウスザイデル法で熱伝導計算を解く 第三引数は、前進・クランクニコルソン・後退積分のいずれかを数値で選択

		//温度を表示してみる
		//DSTR << "vertices[3].temp : " << vertices[3].temp << std::endl;

		//温度のベクトルから節点へ温度の反映
		UpdateVertexTempAll(vertices.size());
	//for(unsigned i =0;i<vertices.size();i++){
	//	DSTR << "vertices[" << i << "].temp : " << vertices[i].temp << std::endl;
	//}
	int templogkatoon =0;

	for(unsigned i =0;i<vertices.size();i++){
		if(vertices[i].temp !=0){
			ofs_ << "vertices[" << i << "].temp : " << vertices[i].temp << std::endl;
		}
	}
	int hogehoge=0;

	//	節点の温度の推移履歴の保存
	if(StepCount ==0){
		templog <<"ステップ数"<<",";
		templog <<"熱シミュレーション時間"<<",";
		for(unsigned i=0; i<vertices.size();i++){
			templog << "節点" << i << "の温度" << ",";
		}
		templog <<"," << std::endl;
		}
	templog << StepCount << ", ";
	templog << StepCount * dt << ", ";
	for(unsigned i=0; i<vertices.size();i++){
		templog << vertices[i].temp << ",";
	}
	templog <<"," << std::endl;
	//templog;
	//節点温度を画面に表示する⇒3次元テクスチャを使ったテクスチャ切り替えに値を渡す⇒関数化

	//DSTR << "TVecAll : " <<std::endl;
	//DSTR << TVecAll << std::endl;
	//DSTR << "bVecAll : " <<std::endl;
	//DSTR << bVecAll << std::endl;


	//	for	DEBUG	節点3とエッジ対を作る節点を表示
	//for(unsigned i =0; i < vertices[3].edges.size();i++){
	//	DSTR << "edges[vertices[3].edges[" << i << "]].vertices[0] : " << edges[vertices[3].edges[i]].vertices[0] << std::endl;
	//	DSTR << "edges[vertices[3].edges[" << i << "]].vertices[1] : " << edges[vertices[3].edges[i]].vertices[1] << std::endl;
	//}
	//for(unsigned i =0; i < vertices[3].edges.size();i++){
	//	DSTR << "vertices[3].edges[" << i << "] : " << vertices[3].edges[i] << std::endl;
	//}
	int hogeshi = 0;
	//	for DEBUG
	//（形状が変わったら、マトリクスやベクトルを作り直す）
	//温度変化・最新の時間の{T}縦ベクトルに記載されている節点温度を基に化学変化シミュレーションを行う
		//SetChemicalSimulation();
		//化学変化シミュレーションに必要な温度などのパラメータを渡す
	//温度変化や化学シミュレーションの結果はグラフィクス表示を行う
	StepCount += 1;
	if(StepCount >= 1000*1000*1000){
		StepCount = 0;
		StepCount_ += 1;
	}
	int temphogeshi =0;

}

void PHFemMeshThermo::CreateMatrix(){
}


void PHFemMeshThermo::InitTcAll(){
	for(unsigned i =0; i <vertices.size();i++){
		vertices[i].Tc = 0.0;
	}
}

void PHFemMeshThermo::InitCreateMatC(){
	/// MatCについて	//	使用する行列の初期化
	//dMatCAll：対角行列の成分の入った行列のサイズを定義:配列として利用	幅:vertices.size(),高さ:1
	dMatCAll.resize(1,vertices.size()); //(h,w)
	dMatCAll.clear();								///	値の初期化
	//matcの初期化は、matcを作る関数でやっているので、省略
}

void PHFemMeshThermo::InitCreateMatk_(){
	///	MatKについて
	//matkの初期化
	for(unsigned i =0; i < 4 ; i++){
		for(unsigned j =0; j < 4 ; j++){
			matk[i][j] = 0.0;
		}
	}
	dMatKAll.clear();		///	初期化
#ifdef UseMatAll
	//matKAll.resize(vertices.size(),vertices.size());	///	matKAllのサイズの代入
	matKAll.clear();									///	matKAllの初期化
	//DSTR << "matKAll: " << matKAll <<std::endl;
#endif UseMatAll

#ifdef DumK
	//matKAllの初期化
	matKAll.resize(vertices.size(),vertices.size());
	matkAll.clear();					///	初期化、下の初期化コードは不要
#endif


}
void PHFemMeshThermo::InitCreateMatk(){
	///	MatKについて
	//matkの初期化
	for(unsigned i =0; i < 4 ; i++){
		for(unsigned j =0; j < 4 ; j++){
			matk[i][j] = 0.0;
		}
	}
	//for(unsigned i=0;i<3;i++){
	//tets.matk[i]
	//}
	//DMatAll：対角行列の成分の入った行列のサイズを定義:配列として利用	幅:vertices.size(),高さ:1
	dMatKAll.resize(1,vertices.size()); //(h,w)
	dMatKAll.clear();		///	初期化
	////値の初期化
	//for(unsigned i=0;i<vertices.size();i++){
	//	dMatKAll[0][i] = 0.0;
	//}
#ifdef UseMatAll
	
	matKAll.resize(vertices.size(),vertices.size());	///	matKAllのサイズの代入
	matKAll.clear();									///	matKAllの初期化
	//for(unsigned i=0;i<vertices.size();i++){
	//	for(unsigned j=0;j<vertices.size();j++){
	//		matKAll[i][j] = 0.0;
	//	}
	//}
#endif UseMatAll

#ifdef DumK
	//matKAllの初期化
	matKAll.resize(vertices.size(),vertices.size());
	matkAll.clear();					///	初期化、下の初期化コードは不要
	//for(unsigned i=0;i<vertices.size();i++){
	//	for(unsigned j=0;j<vertices.size();j++){
	//		matKAll[i][j] = 0.0;
	//	}
	//}
#endif
}

void PHFemMeshThermo::InitCreateVecf_(){
	///	Vecfについて
	//Vecfの初期化
	for(unsigned i =0; i < 4 ; i++){
			vecf[i] = 0.0;
	}
	vecFAll.clear();						///	初期化
}

void PHFemMeshThermo::InitCreateVecf(){
	///	Vecfについて
	//Vecfの初期化
	for(unsigned i =0; i < 4 ; i++){
			vecf[i] = 0.0;
	}
	vecFAll.resize(vertices.size(),1);		///	全体剛性ベクトルFのサイズを規定
	vecFAll.clear();						///	初期化
}

void PHFemMeshThermo::AfterSetDesc() {	
	////時間刻み幅	dtの設定
	//PHFemMeshThermo::dt = 0.01;

	//シーンから、シーンの時間ステップを取得する⇒以下のコードでは、GetTimeStepしようとすると、NULLが返ってくるので、PHEngineで行う
	//DSTR << DCAST(PHSceneIf, GetScene()) << std::endl;
	//DSTR << DCAST(PHSceneIf, GetScene())->GetTimeStep() << std::endl;
	//int hogeshimitake =0;

	

	//%%%	初期化類		%%%//

	//各種メンバ変数の初期化⇒コンストラクタでできたほうがいいかもしれない。
	///	Edges
	for(unsigned i =0; i < edges.size();i++){
		edges[i].c = 0.0;
		edges[i].k = 0.0;
	}

	///	faces
	for(unsigned i=0;i<faces.size();i++){
		faces[i].alphaUpdated = true;
		faces[i].area = 0.0;
		faces[i].heatTransRatio = 0.0;
		faces[i].deformed = true;				//初期状態は、変形後とする
		faces[i].fluxarea =0.0;
		faces[i].heatflux =0.0;
	}

	///	vertex

	///	tets

	//行列の成分数などを初期化
	bVecAll.resize(vertices.size(),1);

	//行列を作る
		//行列を作るために必要な節点や四面体の情報は、PHFemMeshThermoの構造体に入っている。
			//PHFemMeshThermoのオブジェクトを作る際に、ディスクリプタに値を設定して作る
		
	//節点温度の初期設定(行列を作る前に行う)
	SetVerticesTempAll(0.0);			///	初期温度の設定

	//周囲流体温度の初期化(0.0度にする)
	InitTcAll();

	//節点の初期温度を設定する⇒{T}縦ベクトルに代入
		//{T}縦ベクトルを作成する。以降のK,C,F行列・ベクトルの節点番号は、この縦ベクトルの節点の並び順に合わせる?
		
	//dmnN 次元の温度の縦（列）ベクトル
	CreateTempMatrix();

	//熱伝導率、密度、比熱、熱伝達率　のパラメーターを設定・代入
		//PHFemMEshThermoのメンバ変数の値を代入 CADThermoより、0.574;//玉ねぎの値//熱伝導率[W/(ｍ・K)]　Cp = 1.96 * (Ndt);//玉ねぎの比熱[kJ/(kg・K) 1.96kJ/(kg K),（玉ねぎの密度）食品加熱の科学p64より970kg/m^3
		//熱伝達率の単位系　W/(m^2 K)⇒これはSI単位系なのか？　25は論文(MEAT COOKING SIMULATION BY FINITE ELEMENTS)のオーブン加熱時の実測値
		//SetInitThermoConductionParam(0.574,970,1.96,25);
	SetInitThermoConductionParam(0.574,970,0.1960,25 * 0.01);		//> thConduct:熱伝導率 ,roh:密度,	specificHeat:比熱 J/ (K・kg):1960 ,　heatTrans:熱伝達率 W/(m^2・K)
		//これら、変数値は後から計算の途中で変更できるようなSetParam()関数を作っておいたほうがいいかな？

	//> 熱流束の初期化
	SetVtxHeatFluxAll(0.0);

	//>	熱放射率の設定
	SetThermalEmissivityToVerticesAll(0.0);				///	暫定値0.0で初期化	：熱放射はしないｗ

	//>	行列の作成　行列の作成に必要な変数はこの行以前に設定が必要
		//計算に用いるマトリクス、ベクトルを作成（メッシュごとの要素剛性行列/ベクトル⇒全体剛性行列/ベクトル）
		//{T}縦ベクトルの節点の並び順に並ぶように、係数行列を加算する。係数行列には、面積や体積、熱伝達率などのパラメータの積をしてしまったものを入れる。


	//> IHのために必要な処理
	CalcVtxDisFromOrigin();
	//>	IHからの単位時間当たりの加熱熱量
	//単位時間当たりの総加熱熱量	231.9; //>	J/sec
	CalcIHdqdt(0.04,0.095,231.9 * 0.005 * 1e6);		/// 単位 m,m,J/sec		//> 0.002:dtの分;Stepで用いるdt倍したいが...	// 0.05,0.11は適当値
//	CalcIHdqdt2(0.04,0.095,231.9 * 0.005 * 1e6);
	CalcIHdqdt4(0.04,0.095,231.9 * 0.005 * 1e6);
	CalcIHdqdt5(0.04,0.095,231.9 * 0.005 * 1e6);
	//	この後で、熱流束ベクトルを計算する関数を呼び出す

	//CreateLocalMatrixAndSet();			//> 以下の処理を、この関数に集約


	InitCreateMatC();					///	CreateMatCの初期化
	InitCreateVecf();					///	CreateVecfの初期化
	InitCreateMatk();					///	CreateMatKの初期化
	///	熱伝達率を各節点に格納
	SetHeatTransRatioToAllVertex();
	for(unsigned i=0; i < this->tets.size(); i++){
		//各行列を作って、ガウスザイデルで計算するための係数の基本を作る。Timestepの入っている項は、このソース(SetDesc())では、実現できないことが分かった(NULLが返ってくる)
		CreateMatkLocal(i);				///	Matk1 Matk2(更新が必要な場合がある)を作る
		//CreateMatKall();		//CreateMatkLocal();に実装したので、後程分ける。
		CreateMatcLocal(i);
		CreateVecfLocal(i);
	}
	int hogeshidebug =0;
	//	節点温度推移の書き出し
	templog.open("templog.csv");

	// カウントの初期化
	Ndt =0;

	
}



//void PHFemMeshThermo::CreateLocalMatrixAndSet(){
//	//K,C,Fの行列を作る関数を呼び出して、作らせる
//	for(unsigned i = 0; i< tets.size() ; i++){
//		//tetsを引数にいれると、その行列・ベクトルを作ってくれて、できた行列、ベクトルを基に係数を設定しくれる
//		//こうすれば、各要素剛性行列でfor文を回さなくてもよくなる
//		//CreateMatkLocal(tets);
//		//CreateMatcLocal(tets);
//		//CreateVecfLocal(tets);
//
//		//tetsを入れて作らせる
////		SetkcfParam(tets);
//
//	}
//
//}
void PHFemMeshThermo::SetkcfParam(Tet tets){
}

void PHFemMeshThermo::CreateMatc(unsigned id){
	//最後に入れる行列を初期化
	for(unsigned i =0; i < 4 ;i++){
		for(unsigned j =0; j < 4 ;j++){
			matc[i][j] = 0.0;
		}
	}
	//matc に21でできた行列を入れる
	matc = Create44Mat21();
	//	for debug
		//DSTR << "matc " << matc << " ⇒ ";
	matc = rho * specificHeat * CalcTetrahedraVolume(tets[id]) / 20.0 * matc;
	//	debug	//係数の積をとる
		//DSTR << matc << std::endl;
		//int hogemat =0 ;
}

void PHFemMeshThermo::CreateMatcLocal(unsigned id){
	//すべての要素について係数行列を作る
		//c
	CreateMatc(id);
	int mathoge=0;
	//	(ガウスザイデルを使った計算時)要素毎に作った行列の成分より、エッジに係数を格納する
	//	or	(ガウスザイデルを使わない計算時)要素ごとの計算が終わるたびに、要素剛性行列の成分だけをエッジや点に作る変数に格納しておく	#ifedefでモード作って、どちらもできるようにしておいても良いけどw
	for(unsigned j=1; j < 4; j++){
		int vtxid0 = tets[id].vertices[j];
		//	下三角行列部分についてのみ実行
		//	j==1:k=0, j==2:k=0,1, j==3:k=0,1,2
		for(unsigned k = 0; k < j; k++){
			int vtxid1 = tets[id].vertices[k];
				for(unsigned l =0; l < vertices[vtxid0].edges.size(); l++){
					for(unsigned m =0; m < vertices[vtxid1].edges.size(); m++){
						if(vertices[vtxid0].edges[l] == vertices[vtxid1].edges[m]){
							edges[vertices[vtxid0].edges[l]].c += matc[j][k];		//同じものが二つあるはずだから半分にする。上三角化下三角だけ走査するには、どういうfor文ｓにすれば良いのか？
							//DSTR << edges[vertices[vtxid0].edges[l]].k << std::endl;
						}
					}
				}
		}
	}
	//対角成分を対角成分の全体剛性行列から抜き出した1×nの行列に代入する
	//j=0~4まで代入(上のループでは、jは対角成分の範囲しかないので、値が入らない成分が出てしまう)
	for(unsigned j =0;j<4;j++){
		dMatCAll[0][tets[id].vertices[j]] += matc[j][j];
	}

	////	for debug
	//DSTR << "dMatCAll : " << std::endl;
	//for(unsigned j =0;j < vertices.size();j++){
	//	DSTR << j << "th : " << dMatCAll[0][j] << std::endl;
	//}
	// ネギについて非0成分になった。

	//	調べる
	//dMatKAllの成分のうち、0となる要素があったら、エラー表示をするコードを書く
	// try catch文にする
	//for(unsigned j = 0; j < vertices.size() ; j++){
	//	if(dMatCAll[0][j] ==0.0){
	//		DSTR << "dMatCAll[0][" << j << "] element is blank" << std::endl;
	//	}
	//}
	int piyodebug =0;
}


void PHFemMeshThermo::CreateVecfLocal(unsigned id){
	
	//すべての要素について係数行列を作る
	//f1を作る
	//>	熱流束境界条件	vecf2を作る			
	//CreateVecf2(id);				//>	tets[id].vecf[1] を初期化,代入		熱流束は相加平均で求める
	CreateVecf2surface(id);			//
	//>	熱伝達境界条件	f3を作る
	CreateVecf3(id);			//>	tets[id].vecf[2] を初期化,代入		熱伝達率は相加平均、周囲流体温度は節点の形状関数？ごとに求める
	//CreateVecf3_(id);			//>	tets[id].vecf[2] を初期化,代入		熱伝達率、周囲流体温度を相加平均で求める
	//f4を作る
	//f1,f2,f3,f4を加算する
	vecf = tets[id].vecf[1] + tets[id].vecf[2];		//>	+ tets[id].vecf[0] +  tets[id].vecf[3] の予定
	//	for debug
	//DSTR << "vecf : " << std::endl;
	//DSTR << vecf << std::endl;
	//DSTR << "vecf3 : " << std::endl;
	//DSTR << vecf3 << std::endl;
	//	(ガウスザイデルを使った計算時)要素毎に作った行列の成分より、エッジに係数を格納する
	//	or	(ガウスザイデルを使わない計算時)要素ごとの計算が終わるたびに、要素剛性行列の成分だけをエッジや点に作る変数に格納しておく	#ifedefでモード作って、どちらもできるようにしておいても良いけどw

	//要素の節点番号の場所に、その節点のfの値を入れる
	//j:要素の中の何番目か
	for(unsigned j =0;j < 4; j++){
		int vtxid0 = tets[id].vertices[j];
		vecFAll[vtxid0][0] = vecf[j];
	}
	//	for debug
	//vecFAllに値が入ったのかどうかを調べる 2011.09.21全部に値が入っていることを確認した
	//DSTR << "vecFAll : " << std::endl;
	//for(unsigned j =0; j < vertices.size() ; j++){
	//	DSTR << j << " ele is :  " << vecFAll[j][0] << std::endl;
	//}
	int hogeshi =0;

	////	調べる
	////vecFAllの成分のうち、0となる要素があったら、エラー表示をするコードを書く
	//// try catch文にする
	//for(unsigned j = 0; j < vertices.size() ; j++){
	//	if(vecFAll[j][0] ==0.0){
	//		DSTR << "vecFAll[" << j << "][0] element is blank" << std::endl;
	//	}
	//}


	//	注意
	//	f3を使用する場合:周囲流体温度Tcが0の節点の要素は0になるため、温度の設定が必要
	//int hogef =0;
}

void PHFemMeshThermo::CreateMatkLocal(unsigned id){
	//すべての四面体要素について係数行列を作る

	//	k1を作る	k1kでも、k1bでもどちらでも構わない	どりらが速いか調べる
	///	変形した時だけ生成する
	if(deformed){	CreateMatk1k(id);}			//	k理論を根拠に、加筆して、形状関数を導出	
//	if(deformed){	CreateMatk1b(id);}			//	書籍の理論を根拠に、公式を用いて形状関数を導出
	//DSTR << "tets[id].matk1: " << tets[id].matk1 << std::endl;

	//熱伝達境界条件に必要な、k2を作るか否か
	CreateMatk2t(id);					///	熱伝達境界条件
	//CreateMatk2(id,tets[id]);			///	この関数は使用しない
	//DSTR << "tets[id].matk2: " << tets[id].matk2 << std::endl;
	int hogehogehoge=0;

	//k1,k2,k3を加算する(使っている数値だけ)
	matk = tets[id].matk[0] + tets[id].matk[1];	
	//DSTR << "matk: " << matk << std::endl;

	//	(ガウスザイデルを使った計算時)要素毎に作った行列の成分より、エッジに係数を格納する
	//	or	(ガウスザイデルを使わない計算時)要素ごとの計算が終わるたびに、要素剛性行列の成分だけをエッジや点に作る変数に格納しておく	#ifedefでモード作って、どちらもできるようにしておいても良いけどw
	//	Edges のこの要素で計算したK行列の成分をkに係数として格納する
		
	//matkの対角成分以外で、下三角の部分の値を、edgeのkに代入する
	//
	//DSTR << i <<"th tetrahedra element'edges[vertices[vtxid0].edges[l]].k (All edges.k' value): " << std::endl;
	for(unsigned j=1; j < 4; j++){
		int vtxid0 = tets[id].vertices[j];
		//	下三角行列部分についてのみ実行
		//	j==1:k=0, j==2:k=0,1, j==3:k=0,1,2
		for(unsigned k = 0; k < j; k++){
			int vtxid1 = tets[id].vertices[k];
			for(unsigned l =0; l < vertices[vtxid0].edges.size(); l++){
				for(unsigned m =0; m < vertices[vtxid1].edges.size(); m++){
					if(vertices[vtxid0].edges[l] == vertices[vtxid1].edges[m]){
						edges[vertices[vtxid0].edges[l]].k += matk[j][k];		//同じものが二つあるはずだから半分にする。上三角化下三角だけ走査するには、どういうfor文ｓにすれば良いのか？
						//DSTR << edges[vertices[vtxid0].edges[l]].k << std::endl;
#ifdef DumK
						edges[vertices[vtxid0].edges[l]].k = 0.0;
#endif DumK
					}
				}
			}
		}
	}

#ifdef UseMatAll
	//SciLabで使うために、全体剛性行列を作る
	//matkから作る
	for(unsigned j=0; j<4 ; j++){
		for(unsigned k=0; k<4 ;k++){
			matKAll[tets[id].vertices[j]][tets[id].vertices[k]] += matk[j][k];
		}
	}

	////edgesに入った係数から作る
	//for(unsigned j=1; j < 4; j++){
	//	int vtxid0 = tets[i].vertices[j];
	//	//	下三角行列部分についてのみ実行
	//	//	j==1:k=0, j==2:k=0,1, j==3:k=0,1,2
	//	for(unsigned k = 0; k < j; k++){
	//		int vtxid1 = tets[i].vertices[k];
	//			for(unsigned l =0; l < vertices[vtxid0].edges.size(); l++){
	//				for(unsigned m =0; m < vertices[vtxid1].edges.size(); m++){
	//					if(vertices[vtxid0].edges[l] == vertices[vtxid1].edges[m]){
	//						edges[vertices[vtxid0].edges[l]].k += matk[j][k];		//同じものが二つあるはずだから半分にする。上三角化下三角だけ走査するには、どういうfor文ｓにすれば良いのか？
	//						//DSTR << edges[vertices[vtxid0].edges[l]].k << std::endl;
	//					}
	//				}
	//			}
	//	}
	//}

#endif UseMatAll

#ifdef DumK
	//SciLabで使うために、全体剛性行列を作る
	//matkから作る
	for(unsigned j=0; j<4 ; j++){
		for(unsigned k=0; k<4 ;k++){
			matKAll[tets.vertices[j]][tets.vertices[k]] = 0.0;;
		}
	}
#endif

	//対角成分を対角成分の全体剛性行列から抜き出した1×nの行列に代入する
	//j=0~4まで代入(上のループでは、jは対角成分の範囲しかないので、値が入らない成分が出てしまう)
	for(unsigned j =0;j<4;j++){
		dMatKAll[0][tets[id].vertices[j]] += matk[j][j];
		//DSTR << "matk[" << j << "][" << j << "] : " << matk[j][j] << std::endl;
		//DSTR << "dMatKAll[0][" << tets[i].vertices[j] << "] : " << dMatKAll[0][tets[i].vertices[j]] << std::endl;
		int hoge4 =0;
	}
	//DSTR << std::endl;	//改行

	//std::ofstream matKAll("matKAll.csv");
	//for(){
	//	matKAll
	//	}


#ifdef DumK
	for(unsigned j=0;j<4;j++){
		dMatKAll[0][tets.vertices[j]] = 0.0;
		int hogeshi =0;
	} 
#endif DumK

	
	//	for debug
	//要素25の0~3番目の節点が何か表示する
	//if(i == 25){
	//		for(unsigned n=0;n < 4;n++){
	//			DSTR << n << " : " << tets[25].vertices[n] << std::endl;	//要素25の0番目の節点が63である。
	//		}
	//}
	//節点番号が63の点がどの要素に入っているのかを調べる⇒25,57だった
	//for(unsigned j=0;j < vertices[63].tets.size();j++){
	//	DSTR << vertices[63].tets[j] <<std::endl;
	//}
		
	//	調べる
	//dMatKAllの成分のうち、0となる要素があったら、エラーか？　入っていない成分があっても、問題ない気もする
	// try catch文にする
//	for(unsigned j = 0; j < vertices.size() ; j++){
//		if(dMatKAll[0][j] ==0.0){
//			DSTR << "Creating dMatKAll error!? : dMatKAll[0][" << j << "] == 0.0 " << std::endl;
////			DSTR << "If " << j <<" 's blank eroors didn't banished until display simulation scene, I recommened Source Code Check!" <<std::endl;  
//		}
//	}

	//DSTR << "matKAll : " << matKAll <<std::endl;
	//for(unsigned i=0;i<vertices.size();i++){
	//	for(unsigned j=0;j <  vertices.size();j++){
	//		if(matKAll[i][j] !=0){
	//			DSTR << "matKAll[" << i <<"][" << j << "]: " << matKAll[i][j] <<std::endl;
	//		}
	//	}
	//}
	//DSTR << "dMatKAll : " <<dMatKAll << std::endl;
#ifdef UseMatAll
	for(unsigned j =0;j<vertices.size();j++){
		if(matKAll[j][j] != dMatKAll[0][j]){
			DSTR << j <<" 成分の要素はおかしい！調査が必要である。 " <<std::endl;
		}
	}
#endif UseMatAll
	int hoge5 =0;

}

void PHFemMeshThermo::CreateMatk1b(unsigned id){
	//yagawa1983を基にノートに式展開した計算式
	unsigned i=0;
	unsigned j=0;
	unsigned k=0;
	unsigned l=0;

	//a_1~a_4, ... , c_4	を作成
	//係数(符号)×行列式の入れ物
	double a[4];
	double b[4];
	double c[4];

	//要素を構成する節点の座標の入れ物
	double x[4];
	double y[4];
	double z[4];
	//要素内の4節点のx,y,z座標を格納
	for(unsigned m=0; m < 4;m++){
		x[m] = vertices[tets[id].vertices[m]].pos.x;
		y[m] = vertices[tets[id].vertices[m]].pos.y;
		z[m] = vertices[tets[id].vertices[m]].pos.z;
	}

	//mata~matc
	//a[i]
	for(unsigned i =0;i<4;i++){
		double fugou =0.0;				// (-1)^i の符号の定義

		//	fugou の符号判定
		if(i == 0 || i == 2){		//0,2の時、(-1)^1,3 = -1
			fugou = -1.0;
		}
		else{					//1,3の時、(-1)^0,2 = 1
			fugou = 1.0;
		}
		
		//i,j,k,lの関係セット⇒配列の要素にしてもいいかも。i[4],if(i[0]=0){i[1](=j)=1, i[2](=k)=2, i[3](=l)=3}	if(i[0]=1){i[1](=j)=2, i[2](=k)=3, i[3](=l)=0}
		if(i==0){		j=1;	k=2;	l=3;	}
		else if(i==1){	j=2;	k=3;	l=0;	}
		else if(i==2){	j=3;	k=0;	l=1;	}
		else if(i==3){	j=0;	k=1;	l=2;	}

		// a_iの作成
		for(unsigned m =0;m<3;m++){						//	1の成分への代入はついで
			mata[m][0] = 1.0;
		}
		mata[0][1] = y[j];
		mata[1][1] = y[k];
		mata[2][1] = y[l];

		mata[0][2] = z[j];
		mata[1][2] = z[k];
		mata[2][2] = z[l];

		a[i] = fugou * mata.det();

		//DSTR << "mata : " <<std::endl;
		//DSTR << mata << std::endl;

		//DSTR << "mata.det() : " <<std::endl;
		//DSTR << mata.det() << std::endl;

		//DSTR << "a[" << i << "] : " <<std::endl;
		//DSTR << a[i] << std::endl;


		// b_iの作成
		matb[0][0]=x[j];
		matb[1][0]=x[k];
		matb[2][0]=x[l];

		for(unsigned m =0;m<3;m++){						//	1の成分への代入はついで
			matb[m][1] = 1.0;
		}

		matb[0][2]=z[j];
		matb[1][2]=z[k];
		matb[2][2]=z[l];

		b[i] = fugou * matb.det();

		//DSTR << "matb : " <<std::endl;
		//DSTR << matb << std::endl;
		//DSTR << "matb.det() : " <<std::endl;
		//DSTR << matb.det() << std::endl;

		//DSTR << "b[" << i << "] : " <<std::endl;
		//DSTR << b[i] << std::endl;

		// c_iの作成
		matcc[0][0]=x[j];
		matcc[1][0]=x[k];
		matcc[2][0]=x[l];

		matcc[0][1]=y[j];
		matcc[1][1]=y[k];
		matcc[2][1]=y[l];

		for(unsigned m =0;m<3;m++){						//	1の成分への代入はついで
			matcc[m][2] = 1.0;
		}

		//DSTR << "matcc : " <<std::endl;
		//DSTR << matcc << std::endl;
		//DSTR << "matcc.det() : " <<std::endl;
		//DSTR << matcc.det() << std::endl;

		c[i] = fugou * matcc.det();
		
		//	for debug　要素ごとのa_i~c_iの算出
		//DSTR << "a["<< i << "] : " << a[i] << std::endl;
		//DSTR << "b["<< i << "] : " << b[i] << std::endl;
		//DSTR << "c["<< i << "] : " << c[i] << std::endl;
		//DSTR << std::endl;
		int debughogeshi =0;
	}
	
	//	matk1の成分にa_i ~ c_iの多項式を代入	きれいすぎるが、下のコードで良い！	対角成分も非対角成分も、全部、下のコード
	//	改善案		下三角と対角成分だけ、計算し、上三角は下三角を代入でもよい。
	for(unsigned i =0;i<4;i++){
		for(unsigned j =0;j<4;j++){
			tets[id].matk[0][i][j] = a[i] * a[j] +b[i] * b[j] + c[i] * c[j];
		}
	}

	////	上記よりコストの少ないコード?
	//matk[0][0] = a[0] * a[0] +b[0] * b[0] + c[0] * c[0];
	//matk[1][1] = a[1] * a[1] +b[1] * b[1] + c[1] * c[1];
	//matk[2][2] = a[2] * a[2] +b[2] * b[2] + c[2] * c[2];
	//matk[3][3] = a[3] * a[3] +b[3] * b[3] + c[3] * c[3];

	//matk[0][1] = a[0] * a[1] + b[0] * b[1] + c[0] * c[1];
	//matk[1][0] = matk[0][1];

	//matk[0][2] = a[0] * a[2] + b[0] * b[2] + c[0] * c[2];
	//matk[2][0] = matk[0][2];

	//matk[0][3] = a[0] * a[3] + b[0] * b[3] + c[0] * c[3];
	//matk[3][0] = matk[0][3];

	//	for DEBUG
	//DSTR << "matk1 : " << std::endl;
	//DSTR << matk1 << std::endl;
	//int debughogeshi2 =0;
	
	//係数の積
	tets[id].matk[0]= thConduct / (36 *  CalcTetrahedraVolume(tets[id])) * tets[id].matk[0];		//理論が間違っていたので、修正

	//	for DEBUG
	//DSTR << "係数積後の matk1 : " << std::endl;
	//DSTR << matk1 << std::endl;
	int debughogeshi3 =0;

	DSTR << "Inner Function matk1b _ tets["<< id << "].matk[0] "<< tets[id].matk[0] << std::endl;  
	//a~cの多項式をK1に代入
	//matk1(4x4)に代入

}

void PHFemMeshThermo::CreateMatk1k(unsigned id){
	//この計算を呼び出すときに、各四面体ごとに計算するため、四面体の0番から順にこの計算を行う
	//四面体を構成する4節点を節点の配列(Tetsには、節点の配列が作ってある)に入っている順番を使って、面の計算を行ったり、行列の計算を行ったりする。
	//そのため、この関数の引数に、四面体要素の番号を取る

	//最後に入れる行列を初期化
	tets[id].matk[0].clear();
	//for(unsigned i =0; i < 4 ;i++){
	//	for(unsigned j =0; j < 4 ;j++){
	//		//tets[id].matk1[i][j] = 0.0;
	//		tets[id].matk[0][i][j] = 0.0;
	//	}
	//}

	//	A行列　=	a11 a12 a13
	//				a21 a22 a23
	//				a31 a32 a33
	//を生成
	PTM::TMatrixRow<4,4,double> matk1A;
	FemVertex p[4];
	for(unsigned i = 0; i< 4 ; i++){
		p[i]= vertices[tets[id].vertices[i]];
	}
	
	matk1A[0][0] = (p[2].pos.y - p[0].pos.y) * (p[3].pos.z - p[0].pos.z) - (p[2].pos.z - p[0].pos.z) * (p[3].pos.y - p[0].pos.y);
	matk1A[0][1] = (p[1].pos.z - p[0].pos.z) * (p[3].pos.y - p[0].pos.y) - (p[1].pos.y - p[0].pos.y) * (p[3].pos.z - p[0].pos.z);
	matk1A[0][2] = (p[1].pos.y - p[0].pos.y) * (p[2].pos.z - p[0].pos.z) - (p[1].pos.z - p[0].pos.z) * (p[2].pos.y - p[0].pos.y);

	matk1A[1][0] = (p[2].pos.z - p[0].pos.z) * (p[3].pos.x - p[0].pos.x) - (p[2].pos.x - p[0].pos.x) * (p[3].pos.z - p[0].pos.z);
	matk1A[1][1] = (p[1].pos.x - p[0].pos.x) * (p[3].pos.z - p[0].pos.z) - (p[1].pos.z - p[0].pos.z) * (p[3].pos.x - p[0].pos.x);
	matk1A[1][2] = (p[1].pos.z - p[0].pos.z) * (p[2].pos.x - p[0].pos.x) - (p[1].pos.x - p[0].pos.x) * (p[2].pos.z - p[0].pos.z);

	matk1A[2][0] = (p[2].pos.x - p[0].pos.x) * (p[3].pos.y - p[0].pos.y) - (p[2].pos.y - p[0].pos.y) * (p[3].pos.x - p[0].pos.x);
	matk1A[2][1] = (p[1].pos.y - p[0].pos.y) * (p[3].pos.x - p[0].pos.x) - (p[1].pos.x - p[0].pos.x) * (p[3].pos.y - p[0].pos.y);
	matk1A[2][2] = (p[1].pos.x - p[0].pos.x) * (p[2].pos.y - p[0].pos.y) - (p[1].pos.y - p[0].pos.y) * (p[2].pos.x - p[0].pos.x);

	//	for	debug
	//DSTR << matk1A[0][0] << std::endl;
	//DSTR << "matk1A : " << matk1A << std::endl; 
	//int hogeshi =0;

	//a11 ~ a33 を行列に入れて、[N~T] [N] を計算させる
	
	PTM::TMatrixRow<1,4,double> Nx;
	PTM::TMatrixRow<1,4,double> Ny;
	PTM::TMatrixRow<1,4,double> Nz;

	Nx[0][0] = -matk1A[0][0] - matk1A[0][1] -matk1A[0][2];
	Nx[0][1] = matk1A[0][0];
	Nx[0][2] = matk1A[0][1];
	Nx[0][3] = matk1A[0][2];

	Ny[0][0] = -matk1A[1][0] - matk1A[1][1] -matk1A[1][2];
	Ny[0][1] = matk1A[1][0];
	Ny[0][2] = matk1A[1][1];
	Ny[0][3] = matk1A[1][2];

	Nz[0][0] = -matk1A[2][0] - matk1A[2][1] -matk1A[2][2];
	Nz[0][1] = matk1A[2][0];
	Nz[0][2] = matk1A[2][1];
	Nz[0][3] = matk1A[2][2];

	//	Km の算出
	//tets[id].matk1 = Nx.trans() * Nx + Ny.trans() * Ny + Nz.trans() * Nz;
	tets[id].matk[0] = Nx.trans() * Nx + Ny.trans() * Ny + Nz.trans() * Nz;

	////	for debug
	//DSTR << "Nx : " << Nx << std::endl;
	//DSTR << "Nx^T : " << Nx.trans() << std::endl;
	//DSTR << "Nx^T * Nx : " << Nx.trans() * Nx << std::endl;
	//DSTR << "Ny^T * Ny : " << Ny.trans() * Ny << std::endl;
	//DSTR << "Nz^T * Nz : " << Nz.trans() * Nz << std::endl;
	//DSTR << "matk1 : " << matk1 << std::endl;
	//int hogehoge =0;

	//	for	DEBUG
	//DSTR << "matk1 : " << matk1 << std::endl;

	//K1
//	matk1 = thConduct / (36 * CalcTetrahedraVolume(tets) ) * matk1;
	
	//tets[id].matk1 = thConduct / (36 * CalcTetrahedraVolume(tets[id]) ) * tets[id].matk1;
	tets[id].matk[0] = thConduct / (36 * CalcTetrahedraVolume(tets[id]) ) * tets[id].matk[0];
	//DSTR << "Inner Function _tets[id].matk1 : " << tets[id].matk1 << std::endl;

}

void PHFemMeshThermo::CreateVecf2surface(unsigned id){
	tets[id].vecf[1].clear();			//	初期化
	//l=0の時f21,1の時:f22, 2の時:f23, 3の時:f24	を生成
	for(unsigned l= 0 ; l < 4; l++){
		vecf2array[l] = Create41Vec1();
		vecf2array[l][l] = 0.0;			//	l行を0に
	}

	for(unsigned l= 0 ; l < 4; l++){
		///	四面体の各面(l = 0 〜 3) についてメッシュ表面かどうかをチェックする。表面なら、行列を作ってvecf2arrayに入れる
		//faces[tets.faces[i]].sorted;		/// 1,24,58みたいな節点番号が入っている
		///	行列型の入れ物を用意

		//faces[tets.faces[l]].vertices;
		if(tets[id].faces[l] < (int)nSurfaceFace && faces[tets[id].faces[l]].fluxarea > 0 ){			///	外殻の面 且つ 熱伝達率が更新されたら matk2を更新する必要がある
			///	四面体の三角形の面積を計算		///	この関数の外で面積分の面積計算を実装する。移動する
			if(faces[tets[id].faces[l]].area ==0 || faces[tets[id].faces[l]].deformed ){		///	面積が計算されていない時（はじめ） or deformed(変形した時・初期状態)がtrueの時		///	条件の追加	面積が0か ||(OR) αが更新されたか
				faces[tets[id].faces[l]].area = CalcTriangleArea(faces[tets[id].faces[l]].vertices[0], faces[tets[id].faces[l]].vertices[1], faces[tets[id].faces[l]].vertices[2]);
				faces[tets[id].faces[l]].deformed = false;
			}
			///	計算結果を行列に代入
			///	areaの計算に使っていない点が入っている行を除いたベクトルの積をとる
			///	積分計算を根本から考える
			unsigned vtx = tets[id].vertices[0] + tets[id].vertices[1] + tets[id].vertices[2] + tets[id].vertices[3];			
			///	area計算に使われていない節点ID：ID
			unsigned ID = vtx -( faces[tets[id].faces[l]].vertices[0] + faces[tets[id].faces[l]].vertices[1] + faces[tets[id].faces[l]].vertices[2] );
			for(unsigned j=0;j<4;j++){
				if(tets[id].vertices[j] == ID){					///	形状関数が１、（すなわち）このfaceに対面する頂点　と一致したら　その時のfaceで面積分する
					///	j番目の行列の成分を0にしたmatk2arrayで計算する
					///	外殻にないメッシュ面の面積は0で初期化しておく
					///	以下の[]は上までの[l]と異なる。
					///	IDが何番目かによって、形状関数の係数が異なるので、
					//tets[id].matk2 += faces[tets[id].faces[l]].heatTransRatio * (1.0/12.0) * faces[tets[id].faces[l]].area * matk2array[j];
					tets[id].vecf[1] += faces[tets[id].faces[l]].heatflux * (1.0/3.0) * faces[tets[id].faces[l]].area * vecf2array[j];
					//DSTR << "tets[id].matk2にfaces[tets[id].faces[l]].heatTransRatio * (1.0/12.0) * faces[tets[id].faces[l]].area * matk2array[" << j << "]"<< "を加算: " <<faces[tets[id].faces[l]].heatTransRatio * (1.0/12.0) * faces[tets[id].faces[l]].area * matk2array[j] << std::endl;
					//DSTR << "tets[id].matk2 +=  " << tets[id].matk2 << std::endl;
				}
				//else{
				//	///	IDと一致しない場合には、matk2array[j]には全成分0を入れる
				//	///	としたいところだが、
				//	//matk2array[j] =0.0 * matk2array[j];
				//	//DSTR << "matk2array[" << j << "]: " << matk2array[j] << std::endl;
				//}
			}
		}
		///	SurfaceFaceじゃなかったら、matk2arrayには0を入れる
		//else{
		//	//matk2array[l];
		//}
	}


}

void PHFemMeshThermo::CreateVecf2(unsigned id){
	//	初期化
	for(unsigned i =0; i < 4 ;i++){
		//最後に入れる行列を初期化
		tets[id].vecf[1][i] =0.0;				//>	f3 = vecf[1] 
	}	
	//l=0の時f31,1:f32, 2:f33, 3:f34	を生成
	for(unsigned l= 0 ; l < 4; l++){
		//matk2array[l] = matk2temp;
		vecf2array[l] = Create41Vec1();
		//	l行を0に
		vecf2array[l][l] = 0.0;

		//array[n][m][l]	= narray[n],m行l列
		//	f_3	(vecf3array[0], vecf3array[1],..)
		// =	| 0 | + | 1 |+...
		//		| 1 |   | 0 |
		//		| 1 |   | 1 |
		//		| 1 |   | 1 |

		//	for debug
		//DSTR << "vecf3array[" << l << "] : " << std::endl;
		//DSTR << vecf3array[l] << std::endl;

		//係数の積をとる
		//この節点で構成される四面体の面積の積をとる
		//四面体の節点1,2,3(0以外)で作る三角形の面積
		//l==0番目の時、 123	を代入する
		//l==1			0 23
		//l==2			01 3
		//l==3			012
		//をCalcTriangleAreaに入れることができるようにアルゴリズムを考える。

		//>	CreateMatk2tのようなアルゴリズムに変更予定
		//k21
		if(l==0){
			//>	三角形面を構成する3頂点の熱流束の相加平均
			double tempHF = (vertices[tets[id].vertices[1]].heatFluxValue + vertices[tets[id].vertices[2]].heatFluxValue + vertices[tets[id].vertices[3]].heatFluxValue ) / 3.0;		//HTR:HeatTransRatio
			vecf2array[l] = tempHF * (1.0/3.0) * CalcTriangleArea( tets[id].vertices[1],tets[id].vertices[2],tets[id].vertices[3] ) * vecf2array[l];
			//DSTR << "vecf2array[" << l << "] : " << vecf2array[l] << std::endl;
			//Vecの節点毎にその節点での周囲流体温度Tcとの積を行う
			
			////>	不要？
			//for(unsigned m=0; m<4; m++){
			//	vecf2array[l][m] = vertices[tets[id].vertices[m]].Tc * vecf2array[l][m];
			//}
		}
		//	k22
		else if(l==1){
			double tempHF = (vertices[tets[id].vertices[0]].heatFluxValue + vertices[tets[id].vertices[2]].heatFluxValue + vertices[tets[id].vertices[3]].heatFluxValue ) / 3.0;		//HTR:HeatTransRatio
			vecf2array[l] = tempHF * (1.0/3.0) * CalcTriangleArea( tets[id].vertices[0],tets[id].vertices[2],tets[id].vertices[3] ) * vecf2array[l];
			//vecf3array[l] = heatTrans * (1.0/3.0) * CalcTriangleArea( tets[id].vertices[0],tets[id].vertices[2],tets[id].vertices[3] ) * vecf3array[l];
			//DSTR << "vecf3array[" << l << "] : " << vecf3array[l] << std::endl;
			//Vecの節点毎にその節点での周囲流体温度Tcとの積を行う
			//for(unsigned m=0; m<4; m++){
			//	vecf2array[l][m] = vertices[tets[id].vertices[m]].Tc * vecf2array[l][m];
			//}
		}
		//	k23
		else if(l==2){
			double tempHF = (vertices[tets[id].vertices[0]].heatFluxValue + vertices[tets[id].vertices[1]].heatFluxValue + vertices[tets[id].vertices[3]].heatFluxValue ) / 3.0;		//HTR:HeatTransRatio
			vecf2array[l] = tempHF * (1.0/3.0) * CalcTriangleArea( tets[id].vertices[0],tets[id].vertices[1],tets[id].vertices[3] ) * vecf2array[l];
			//vecf3array[l] = heatTrans * (1.0/3.0) * CalcTriangleArea( tets[id].vertices[0],tets[id].vertices[1],tets[id].vertices[3] ) * vecf3array[l];
			//DSTR << "vecf3array[" << l << "] : " << vecf3array[l] << std::endl;
			//Vecの節点毎にその節点での周囲流体温度Tcとの積を行う
			//for(unsigned m=0; m<4; m++){
			//	vecf2array[l][m] = vertices[tets[id].vertices[m]].Tc * vecf2array[l][m];
			//}
		}
		//	k24
		else if(l==3){
			double tempHF = (vertices[tets[id].vertices[0]].heatFluxValue + vertices[tets[id].vertices[1]].heatFluxValue + vertices[tets[id].vertices[2]].heatFluxValue ) / 3.0;		//HTR:HeatTransRatio
			vecf2array[l] = tempHF * (1.0/3.0) * CalcTriangleArea( tets[id].vertices[0],tets[id].vertices[1],tets[id].vertices[2] ) * vecf2array[l];
			//vecf3array[l] = heatTrans * (1.0/3.0) * CalcTriangleArea( tets[id].vertices[0],tets[id].vertices[1],tets[id].vertices[2] ) * vecf3array[l];
			//DSTR << "vecf3array[" << l << "] : " << vecf3array[l] << std::endl;
			//Vecの節点毎にその節点での周囲流体温度Tcとの積を行う
			//for(unsigned m=0; m<4; m++){
			//	vecf2array[l][m] = vertices[tets[id].vertices[m]].Tc * vecf2array[l][m];
			//}
		}
		//for debug
		//DSTR << "vecf3array[" << l << "]の完成版は↓" << std::endl;
		//DSTR << vecf3array[l] << std::endl;
		//if(dMatCAll == NULL){
		//	//DSTR <<"i : "<< i << ", l : " << l << std::endl;
		//	DSTR << "dMatCAll == NULL" <<std::endl;
		//	DSTR <<"l : " << l << std::endl;
		//}
	}

	//f3 = f31 + f32 + f33 + f34
	for(unsigned i=0; i < 4; i++){
		//vecf3 += vecf3array[i];
		tets[id].vecf[1] += vecf2array[i];
		//	for debug
		//DSTR << "vecf3 に vecf3array = f3" << i+1 <<"まで加算した行列" << std::endl;
		//DSTR << vecf3 << std::endl;
	}
	
	//	f1,f2,f3,f4	を計算する際に、[0][0]成分から[3][0]成分までの非0成分について、先にTcをかけてしまう


	//for debug
	//DSTR << "節点（";
	//for(unsigned i =0; i < 4; i++){
	//	DSTR << tets[id].vertices[i] << "," ;
	//}
	//DSTR << ")で構成される四面体の" << std::endl;
	//DSTR << "vecf3 : " << std::endl;
	//DSTR << vecf3 << std::endl;
	//int hogeshishi =0;
}

void PHFemMeshThermo::CreateVecf3_(unsigned id){
	//	初期化
	for(unsigned i =0; i < 4 ;i++){
		//vecf3[i] = 0.0;		//最後に入れる行列を初期化
		tets[id].vecf[2][i] =0.0;
	}	
	//l=0の時f31,1:f32, 2:f33, 3:f34	を生成
	for(unsigned l= 0 ; l < 4; l++){
		//matk2array[l] = matk2temp;
		vecf3array[l] = Create41Vec1();
		//	l行を0に
		//for(int i=0;i<4;i++){
		//	vecf3array[l][l][i] = 0.0;
		//}
		vecf3array[l][l] = 0.0;
		//array[n][m][l]	= narray[n],m行l列
		//	f_3	(vecf3array[0], vecf3array[1],..)
		// =	| 0 | + | 1 |+...
		//		| 1 |   | 0 |
		//		| 1 |   | 1 |
		//		| 1 |   | 1 |
		//	for debug
		//DSTR << "vecf3array[" << l << "] : " << std::endl;
		//DSTR << vecf3array[l] << std::endl;

		//係数の積をとる
		//この節点で構成される四面体の面積の積をとる
		//四面体の節点1,2,3(0以外)で作る三角形の面積
		//l==0番目の時、 123	を代入する
		//l==1			0 23
		//l==2			01 3
		//l==3			012
		//をCalcTriangleAreaに入れることができるようにアルゴリズムを考える。
		//k21
		if(l==0){
			//三角形面を構成する3頂点の熱伝達率の相加平均
			double tempHTR = (vertices[tets[id].vertices[1]].heatTransRatio + vertices[tets[id].vertices[2]].heatTransRatio + vertices[tets[id].vertices[3]].heatTransRatio ) / 3.0;		//HTR:HeatTransRatio
			double avgTc = (vertices[tets[id].vertices[1]].Tc + vertices[tets[id].vertices[2]].Tc + vertices[tets[id].vertices[3]].Tc ) / 3.0;
			vecf3array[l] = tempHTR * avgTc * (1.0/3.0) * CalcTriangleArea( tets[id].vertices[1],tets[id].vertices[2],tets[id].vertices[3] ) * vecf3array[l];
			//DSTR << "vecf3array[" << l << "] : " << vecf3array[l] << std::endl;
			
			//>	↓は本当？
			//Vecの節点毎にその節点での周囲流体温度Tcとの積を行う
			//for(unsigned m=0; m<4; m++){
			//	vecf3array[l][m] = vertices[tets[id].vertices[m]].Tc * vecf3array[l][m];
			//}
		}
		//	k22
		else if(l==1){
			double tempHTR = (vertices[tets[id].vertices[0]].heatTransRatio + vertices[tets[id].vertices[2]].heatTransRatio + vertices[tets[id].vertices[3]].heatTransRatio ) / 3.0;		//HTR:HeatTransRatio
			double avgTc = (vertices[tets[id].vertices[0]].Tc + vertices[tets[id].vertices[2]].Tc + vertices[tets[id].vertices[3]].Tc ) / 3.0;		//HTR:HeatTransRatio
			vecf3array[l] = tempHTR * avgTc * (1.0/3.0) * CalcTriangleArea( tets[id].vertices[0],tets[id].vertices[2],tets[id].vertices[3] ) * vecf3array[l];
			//DSTR << "vecf3array[" << l << "] : " << vecf3array[l] << std::endl;
			//Vecの節点毎にその節点での周囲流体温度Tcとの積を行う
			//for(unsigned m=0; m<4; m++){
			//	vecf3array[l][m] = vertices[tets[id].vertices[m]].Tc * vecf3array[l][m];
			//}
		}
		//	k23
		else if(l==2){
			double tempHTR = (vertices[tets[id].vertices[0]].heatTransRatio + vertices[tets[id].vertices[1]].heatTransRatio + vertices[tets[id].vertices[3]].heatTransRatio ) / 3.0;		//HTR:HeatTransRatio
			double avgTc = (vertices[tets[id].vertices[0]].Tc + vertices[tets[id].vertices[1]].Tc + vertices[tets[id].vertices[3]].Tc ) / 3.0;		//HTR:HeatTransRatio
			vecf3array[l] = tempHTR * avgTc * (1.0/3.0) * CalcTriangleArea( tets[id].vertices[0],tets[id].vertices[1],tets[id].vertices[3] ) * vecf3array[l];
			//vecf3array[l] = heatTrans * (1.0/3.0) * CalcTriangleArea( tets[id].vertices[0],tets[id].vertices[1],tets[id].vertices[3] ) * vecf3array[l];
			//DSTR << "vecf3array[" << l << "] : " << vecf3array[l] << std::endl;
			//Vecの節点毎にその節点での周囲流体温度Tcとの積を行う
			//for(unsigned m=0; m<4; m++){
			//	vecf3array[l][m] = vertices[tets[id].vertices[m]].Tc * vecf3array[l][m];
			//}
		}
		//	k24
		else if(l==3){
			double tempHTR = (vertices[tets[id].vertices[0]].heatTransRatio + vertices[tets[id].vertices[1]].heatTransRatio + vertices[tets[id].vertices[2]].heatTransRatio ) / 3.0;		//HTR:HeatTransRatio
			double avgTc = (vertices[tets[id].vertices[0]].Tc + vertices[tets[id].vertices[1]].Tc + vertices[tets[id].vertices[2]].Tc ) / 3.0;		//HTR:HeatTransRatio
			vecf3array[l] = tempHTR * avgTc * (1.0/3.0) * CalcTriangleArea( tets[id].vertices[0],tets[id].vertices[1],tets[id].vertices[2] ) * vecf3array[l];
			//DSTR << "vecf3array[" << l << "] : " << vecf3array[l] << std::endl;
			//Vecの節点毎にその節点での周囲流体温度Tcとの積を行う
			//for(unsigned m=0; m<4; m++){
			//	vecf3array[l][m] = vertices[tets[id].vertices[m]].Tc * vecf3array[l][m];
			//}
		}
		//for debug
		//DSTR << "vecf3array[" << l << "]の完成版は↓" << std::endl;
		//DSTR << vecf3array[l] << std::endl;
		//if(dMatCAll == NULL){
		//	//DSTR <<"i : "<< i << ", l : " << l << std::endl;
		//	DSTR << "dMatCAll == NULL" <<std::endl;
		//	DSTR <<"l : " << l << std::endl;
		//}
	}

	//f3 = f31 + f32 + f33 + f34
	for(unsigned i=0; i < 4; i++){
		//vecf3 += vecf3array[i];
		tets[id].vecf[2] += vecf3array[i];
		//	for debug
		//DSTR << "vecf3 に vecf3array = f3" << i+1 <<"まで加算した行列" << std::endl;
		//DSTR << vecf3 << std::endl;
	}
	
	//	f1,f2,f3,f4	を計算する際に、[0][0]成分から[3][0]成分までの非0成分について、先にTcをかけてしまう


	//for debug
	//DSTR << "節点（";
	//for(unsigned i =0; i < 4; i++){
	//	DSTR << tets[id].vertices[i] << "," ;
	//}
	//DSTR << ")で構成される四面体の" << std::endl;
	//DSTR << "vecf3 : " << std::endl;
	//DSTR << vecf3 << std::endl;
	//int hogeshishi =0;
}


void PHFemMeshThermo::CreateVecf3(unsigned id){
	//	初期化
	for(unsigned i =0; i < 4 ;i++){
		//vecf3[i] = 0.0;		//最後に入れる行列を初期化
		tets[id].vecf[2][i] =0.0;
	}	
	//l=0の時f31,1:f32, 2:f33, 3:f34	を生成
	for(unsigned l= 0 ; l < 4; l++){
		//matk2array[l] = matk2temp;
		vecf3array[l] = Create41Vec1();
		//	l行を0に
		//for(int i=0;i<4;i++){
		//	vecf3array[l][l][i] = 0.0;
		//}
		vecf3array[l][l] = 0.0;
		//array[n][m][l]	= narray[n],m行l列
		//	f_3	(vecf3array[0], vecf3array[1],..)
		// =	| 0 | + | 1 |+...
		//		| 1 |   | 0 |
		//		| 1 |   | 1 |
		//		| 1 |   | 1 |
		//	for debug
		//DSTR << "vecf3array[" << l << "] : " << std::endl;
		//DSTR << vecf3array[l] << std::endl;

		//係数の積をとる
		//この節点で構成される四面体の面積の積をとる
		//四面体の節点1,2,3(0以外)で作る三角形の面積
		//l==0番目の時、 123	を代入する
		//l==1			0 23
		//l==2			01 3
		//l==3			012
		//をCalcTriangleAreaに入れることができるようにアルゴリズムを考える。
		//k21
		if(l==0){
			//三角形面を構成する3頂点の熱伝達率の相加平均
			double tempHTR = (vertices[tets[id].vertices[1]].heatTransRatio + vertices[tets[id].vertices[2]].heatTransRatio + vertices[tets[id].vertices[3]].heatTransRatio ) / 3.0;		//HTR:HeatTransRatio
			vecf3array[l] = tempHTR * (1.0/3.0) * CalcTriangleArea( tets[id].vertices[1],tets[id].vertices[2],tets[id].vertices[3] ) * vecf3array[l];
			//vecf3array[l] = heatTrans * (1.0/3.0) * CalcTriangleArea( tets.vertices[1],tets.vertices[2],tets.vertices[3] ) * vecf3array[l];
			//DSTR << "vecf3array[" << l << "] : " << vecf3array[l] << std::endl;
			//Vecの節点毎にその節点での周囲流体温度Tcとの積を行う
			for(unsigned m=0; m<4; m++){
				vecf3array[l][m] = vertices[tets[id].vertices[m]].Tc * vecf3array[l][m];
			}
		}
		//	k22
		else if(l==1){
			double tempHTR = (vertices[tets[id].vertices[0]].heatTransRatio + vertices[tets[id].vertices[2]].heatTransRatio + vertices[tets[id].vertices[3]].heatTransRatio ) / 3.0;		//HTR:HeatTransRatio
			vecf3array[l] = tempHTR * (1.0/3.0) * CalcTriangleArea( tets[id].vertices[0],tets[id].vertices[2],tets[id].vertices[3] ) * vecf3array[l];
			//vecf3array[l] = heatTrans * (1.0/3.0) * CalcTriangleArea( tets[id].vertices[0],tets[id].vertices[2],tets[id].vertices[3] ) * vecf3array[l];
			//DSTR << "vecf3array[" << l << "] : " << vecf3array[l] << std::endl;
			//Vecの節点毎にその節点での周囲流体温度Tcとの積を行う
			for(unsigned m=0; m<4; m++){
				vecf3array[l][m] = vertices[tets[id].vertices[m]].Tc * vecf3array[l][m];
			}
		}
		//	k23
		else if(l==2){
			double tempHTR = (vertices[tets[id].vertices[0]].heatTransRatio + vertices[tets[id].vertices[1]].heatTransRatio + vertices[tets[id].vertices[3]].heatTransRatio ) / 3.0;		//HTR:HeatTransRatio
			vecf3array[l] = tempHTR * (1.0/3.0) * CalcTriangleArea( tets[id].vertices[0],tets[id].vertices[1],tets[id].vertices[3] ) * vecf3array[l];
			//vecf3array[l] = heatTrans * (1.0/3.0) * CalcTriangleArea( tets[id].vertices[0],tets[id].vertices[1],tets[id].vertices[3] ) * vecf3array[l];
			//DSTR << "vecf3array[" << l << "] : " << vecf3array[l] << std::endl;
			//Vecの節点毎にその節点での周囲流体温度Tcとの積を行う
			for(unsigned m=0; m<4; m++){
				vecf3array[l][m] = vertices[tets[id].vertices[m]].Tc * vecf3array[l][m];
			}
		}
		//	k24
		else if(l==3){
			double tempHTR = (vertices[tets[id].vertices[0]].heatTransRatio + vertices[tets[id].vertices[1]].heatTransRatio + vertices[tets[id].vertices[2]].heatTransRatio ) / 3.0;		//HTR:HeatTransRatio
			vecf3array[l] = tempHTR * (1.0/3.0) * CalcTriangleArea( tets[id].vertices[0],tets[id].vertices[1],tets[id].vertices[2] ) * vecf3array[l];
			//vecf3array[l] = heatTrans * (1.0/3.0) * CalcTriangleArea( tets[id].vertices[0],tets[id].vertices[1],tets[id].vertices[2] ) * vecf3array[l];
			//DSTR << "vecf3array[" << l << "] : " << vecf3array[l] << std::endl;
			//Vecの節点毎にその節点での周囲流体温度Tcとの積を行う
			for(unsigned m=0; m<4; m++){
				vecf3array[l][m] = vertices[tets[id].vertices[m]].Tc * vecf3array[l][m];
			}
		}
		//for debug
		//DSTR << "vecf3array[" << l << "]の完成版は↓" << std::endl;
		//DSTR << vecf3array[l] << std::endl;
		//if(dMatCAll == NULL){
		//	//DSTR <<"i : "<< i << ", l : " << l << std::endl;
		//	DSTR << "dMatCAll == NULL" <<std::endl;
		//	DSTR <<"l : " << l << std::endl;
		//}
	}

	//f3 = f31 + f32 + f33 + f34
	for(unsigned i=0; i < 4; i++){
		//vecf3 += vecf3array[i];
		tets[id].vecf[2] += vecf3array[i];
		//	for debug
		//DSTR << "vecf3 に vecf3array = f3" << i+1 <<"まで加算した行列" << std::endl;
		//DSTR << vecf3 << std::endl;
	}
	
	//	f1,f2,f3,f4	を計算する際に、[0][0]成分から[3][0]成分までの非0成分について、先にTcをかけてしまう


	//for debug
	//DSTR << "節点（";
	//for(unsigned i =0; i < 4; i++){
	//	DSTR << tets[id].vertices[i] << "," ;
	//}
	//DSTR << ")で構成される四面体の" << std::endl;
	//DSTR << "vecf3 : " << std::endl;
	//DSTR << vecf3 << std::endl;
	//int hogeshishi =0;
}


double PHFemMeshThermo::CalcTetrahedraVolume(Tet tets){
	PTM::TMatrixRow<4,4,double> tempMat44;
	for(unsigned i =0; i < 4; i++){
		for(unsigned j =0; j < 4; j++){
			if(i == 0){
				tempMat44[i][j] = 1.0;
			}
			else if(i == 1){
				tempMat44[i][j] = vertices[tets.vertices[j]].pos.x;
			}
			else if(i == 2){
				tempMat44[i][j] = vertices[tets.vertices[j]].pos.y;
			}
			else if(i == 3){
				tempMat44[i][j] = vertices[tets.vertices[j]].pos.z;
			}
			
		}
	}
	//	for debug
	//DSTR << tempMat44 << std::endl;
	//for(unsigned i =0; i < 4 ;i++){
	//	DSTR << vertices[tets.vertices[i]].pos.x << " , " << vertices[tets.vertices[i]].pos.y << " , " << vertices[tets.vertices[i]].pos.z << std::endl; 
	//}
	//DSTR << tempMat44.det() << std::endl;
	//int hogever = 0;
	return tempMat44.det() / 6.0;
}

double PHFemMeshThermo::CalcTriangleArea(int id0, int id1, int id2){
	double area=0.0;								///	要改善	faces[id].areaに値を入れる 

	//行列式の成分を用いて面積を求める
	//triarea =
	//|  1     1     1   |
	//|x2-x1 y2-y1 z2-z1 |
	//|x3-x1 y3-y1 z3-z1 |
	//|
	PTM::TMatrixRow<3,3,double> triarea;		//三角形の面積　= tri + area
	for(unsigned i =0 ; i < 3 ; i++){
		triarea[0][i] = 1.0;
	}
	for(unsigned i =0 ; i < 3 ; i++){
		//					x2(節点2のx(pos第i成分)目)	-	x1(〃)
		// i==0の時	vertices[id1].pos[i]	=>	 pos[0] == pos.x
		triarea[1][i] = vertices[id1].pos[i] - vertices[id0].pos[i];
	}
	for(unsigned i =0 ; i < 3 ; i++){
		//					x3(節点3のx(pos第i成分)目)	-	x1(〃)
		triarea[2][i] = vertices[id2].pos[i] - vertices[id0].pos[i];
	}
	double m1,m2,m3 = 0.0;
	m1 = triarea[1][1] * triarea[2][2] - triarea[1][2] * triarea[2][1];
	m2 = triarea[2][0] * triarea[1][2] - triarea[1][0] * triarea[2][2];
	m3 = triarea[1][0] * triarea[2][1] - triarea[2][0] * triarea[1][1];

	area = sqrt(m1 * m1 + m2 * m2 + m3 * m3) / 2.0;

	//	for debug
	//DSTR << "三角形の面積は : " << area << std::endl; 

	//0番目の節点は40,1番目の節点は134,2番目の節点は79 の座標で計算してみた
	//三角形を求める行列 : 2.75949e-005 * 1 = 2.75949 × 10-5(byGoogle計算機) [m^2] = 2.75949 × 10-1 [cm^2]なので、ネギのメッシュのスケールなら大体あっているはず

	return area;
}

PTM::TMatrixCol<4,1,double> PHFemMeshThermo::Create41Vec1(){
	PTM::TMatrixCol<4,1,double> Mat1temp;
	for(int i =0; i <4 ; i++){
				Mat1temp[i][0] = 1.0;
	}
	return Mat1temp;
}

PTM::TMatrixRow<4,4,double> PHFemMeshThermo::Create44Mat21(){
	//|2 1 1 1 |
	//|1 2 1 1 |
	//|1 1 2 1 |
	//|1 1 1 2 |	を作る
	PTM::TMatrixRow<4,4,double> MatTemp;
	for(int i =0; i <4 ; i++){
		for(int j=0; j < 4 ; j++){
			if(i==j){
				MatTemp[i][j] = 2.0;
			}else{
				MatTemp[i][j] = 1.0;
			}
		}
	}
	return MatTemp;
}
void PHFemMeshThermo::CreateMatk3t(unsigned id){
	//l=0の時k21,1の時:k22, 2の時:k23, 3の時:k24	を生成
	for(unsigned l= 0 ; l < 4; l++){
		//matk2array[l] = matk2temp;
		matk3array[l] = Create44Mat21();
		//	1行i列を0に
		for(int i=0;i<4;i++){
			matk3array[l][l][i] = 0.0;
		}
		//	i行1列を0に
		for(int i=0;i<4;i++){
			matk3array[l][i][l] = 0.0;
		}
	}

	///	初期化
	tets[id].matk[2].clear();
	
	for(unsigned l= 0 ; l < 4; l++){
		///	四面体の各面(l = 0 〜 3) についてメッシュ表面かどうかをチェックする。表面なら、行列を作ってmatk2arrayに入れる
		//faces[tets.faces[i]].sorted;		/// 1,24,58みたいな節点番号が入っている
		///	行列型の入れ物を用意

		//faces[tets.faces[l]].vertices;
		if(tets[id].faces[l] < (int)nSurfaceFace && faces[tets[id].faces[l]].alphaUpdated ){			///	外殻の面 且つ 熱伝達率が更新されたら matk2を更新する必要がある
			//最後に入れる行列を初期化
			for(unsigned i =0; i < 4 ;i++){
				for(unsigned j =0; j < 4 ;j++){
					tets[id].matk[2][i][j] = 0.0;
				}
			}
			///	四面体の三角形の面積を計算		///	この関数の外で面積分の面積計算を実装する。移動する
			if(faces[tets[id].faces[l]].area ==0 || faces[tets[id].faces[l]].deformed ){		///	面積が計算されていない時（はじめ） or deformed(変形した時・初期状態)がtrueの時		///	条件の追加	面積が0か ||(OR) αが更新されたか
				faces[tets[id].faces[l]].area = CalcTriangleArea(faces[tets[id].faces[l]].vertices[0], faces[tets[id].faces[l]].vertices[1], faces[tets[id].faces[l]].vertices[2]);
				faces[tets[id].faces[l]].deformed = false;
			}
			///	計算結果を行列に代入
			///	areaの計算に使っていない点が入っている行と列を除いた行列の積をとる
			///	積分計算を根本から考える
			unsigned vtx = tets[id].vertices[0] + tets[id].vertices[1] + tets[id].vertices[2] + tets[id].vertices[3];
			//DSTR << "vtx: " << vtx <<std::endl;
				///	area計算に使われていない節点ID：ID
			unsigned ID = vtx -( faces[tets[id].faces[l]].vertices[0] + faces[tets[id].faces[l]].vertices[1] + faces[tets[id].faces[l]].vertices[2] );
			//DSTR << "メッシュ表面の面は次の3頂点からなる。" << std::endl;
			//DSTR << "faces[tets.faces[l]].vertices[0]: " << faces[tets.faces[l]].vertices[0] <<std::endl;
			//DSTR << "faces[tets.faces[l]].vertices[1]: " << faces[tets.faces[l]].vertices[1] <<std::endl;
			//DSTR << "faces[tets.faces[l]].vertices[2]: " << faces[tets.faces[l]].vertices[2] <<std::endl;
			//DSTR << "ID: " << ID <<"のときの節点と対面する面で面積分を計算する"<<std::endl;
			for(unsigned j=0;j<4;j++){
				if(tets[id].vertices[j] == ID){					///	形状関数が１、（すなわち）このfaceに対面する頂点　と一致したら　その時のfaceで面積分する
					///	j番目の行列の成分を0にしたmatk2arrayで計算する
					///	外殻にないメッシュ面の面積は0で初期化しておく
					faces[tets[id].faces[l]].thermalEmissivity = (vertices[faces[tets[id].faces[l]].vertices[0]].thermalEmissivity + vertices[faces[tets[id].faces[l]].vertices[1]].thermalEmissivity 
						+ vertices[faces[tets[id].faces[l]].vertices[2]].thermalEmissivity ) / 3.0;		///	当該faceの熱伝達率を構成節点での値の相加平均をとる
					///	熱輻射率を計算、熱輻射率も、節点の熱輻射率から計算？

					///	以下の[]は上までの[l]と異なる。
					///	IDが何番目かによって、形状関数の係数が異なるので、
					tets[id].matk[2] += faces[tets[id].faces[l]].thermalEmissivity * (1.0/12.0) * faces[tets[id].faces[l]].area * matk2array[j];
					//DSTR << "tets[id].matk2にfaces[tets[id].faces[l]].heatTransRatio * (1.0/12.0) * faces[tets[id].faces[l]].area * matk2array[" << j << "]"<< "を加算: " <<faces[tets[id].faces[l]].heatTransRatio * (1.0/12.0) * faces[tets[id].faces[l]].area * matk2array[j] << std::endl;
					//DSTR << "tets[id].matk2 +=  " << tets[id].matk2 << std::endl;
				}
			}
		}
	}
}
void PHFemMeshThermo::CreateMatk2t(unsigned id){

	//l=0の時k21,1の時:k22, 2の時:k23, 3の時:k24	を生成
	for(unsigned l= 0 ; l < 4; l++){
		//matk2array[l] = matk2temp;
		matk2array[l] = Create44Mat21();
		//	1行i列を0に
		for(int i=0;i<4;i++){
			matk2array[l][l][i] = 0.0;
		}
		//	i行1列を0に
		for(int i=0;i<4;i++){
			matk2array[l][i][l] = 0.0;
		}
	}

	///	初期化
	tets[id].matk[1].clear();
	//for(unsigned i =0; i < 4 ;i++){
	//	for(unsigned j =0; j < 4 ;j++){
	//		//tets[id].matk2[i][j] = 0.0;
	//		tets[id].matk[1][i][j] = 0.0;
	//	}
	//}

	//	Check
	//DSTR << "matk2array:" << std::endl;
	//for(unsigned i=0;i<4;i++){
	//	DSTR <<i <<": " << matk2array[i] << std::endl;
	//}
	//DSTR << "++i" <<std::endl;
	//for(unsigned i=0;i<4;++i){
	//	DSTR <<i <<": " << matk2array[i] << std::endl;
	//}


	for(unsigned l= 0 ; l < 4; l++){
		///	四面体の各面(l = 0 〜 3) についてメッシュ表面かどうかをチェックする。表面なら、行列を作ってmatk2arrayに入れる
		//faces[tets.faces[i]].sorted;		/// 1,24,58みたいな節点番号が入っている
		///	行列型の入れ物を用意

		//faces[tets.faces[l]].vertices;
		if(tets[id].faces[l] < (int)nSurfaceFace && faces[tets[id].faces[l]].alphaUpdated ){			///	外殻の面 且つ 熱伝達率が更新されたら matk2を更新する必要がある
			//最後に入れる行列を初期化
			for(unsigned i =0; i < 4 ;i++){
				for(unsigned j =0; j < 4 ;j++){
					//matk2[i][j] = 0.0;
					//tets[id].matk2[i][j] = 0.0;
					tets[id].matk[1][i][j] = 0.0;
				}
			}
			///	四面体の三角形の面積を計算		///	この関数の外で面積分の面積計算を実装する。移動する
			if(faces[tets[id].faces[l]].area ==0 || faces[tets[id].faces[l]].deformed ){		///	面積が計算されていない時（はじめ） or deformed(変形した時・初期状態)がtrueの時		///	条件の追加	面積が0か ||(OR) αが更新されたか
				faces[tets[id].faces[l]].area = CalcTriangleArea(faces[tets[id].faces[l]].vertices[0], faces[tets[id].faces[l]].vertices[1], faces[tets[id].faces[l]].vertices[2]);
				faces[tets[id].faces[l]].deformed = false;
			}
			///	計算結果を行列に代入
			///	areaの計算に使っていない点が入っている行と列を除いた行列の積をとる
			///	積分計算を根本から考える
			unsigned vtx = tets[id].vertices[0] + tets[id].vertices[1] + tets[id].vertices[2] + tets[id].vertices[3];
			//DSTR << "vtx: " << vtx <<std::endl;
			
			///	area計算に使われていない節点ID：ID
			unsigned ID = vtx -( faces[tets[id].faces[l]].vertices[0] + faces[tets[id].faces[l]].vertices[1] + faces[tets[id].faces[l]].vertices[2] );
			//DSTR << "メッシュ表面の面は次の3頂点からなる。" << std::endl;
			//DSTR << "faces[tets.faces[l]].vertices[0]: " << faces[tets.faces[l]].vertices[0] <<std::endl;
			//DSTR << "faces[tets.faces[l]].vertices[1]: " << faces[tets.faces[l]].vertices[1] <<std::endl;
			//DSTR << "faces[tets.faces[l]].vertices[2]: " << faces[tets.faces[l]].vertices[2] <<std::endl;
			//DSTR << "ID: " << ID <<"のときの節点と対面する面で面積分を計算する"<<std::endl;
			for(unsigned j=0;j<4;j++){
				if(tets[id].vertices[j] == ID){					///	形状関数が１、（すなわち）このfaceに対面する頂点　と一致したら　その時のfaceで面積分する
					///	j番目の行列の成分を0にしたmatk2arrayで計算する
					///	外殻にないメッシュ面の面積は0で初期化しておく
					faces[tets[id].faces[l]].heatTransRatio = (vertices[faces[tets[id].faces[l]].vertices[0]].heatTransRatio + vertices[faces[tets[id].faces[l]].vertices[1]].heatTransRatio 
						+ vertices[faces[tets[id].faces[l]].vertices[2]].heatTransRatio ) / 3.0;		///	当該faceの熱伝達率を構成節点での値の相加平均をとる
					///	以下の[]は上までの[l]と異なる。
					///	IDが何番目かによって、形状関数の係数が異なるので、
					//tets[id].matk2 += faces[tets[id].faces[l]].heatTransRatio * (1.0/12.0) * faces[tets[id].faces[l]].area * matk2array[j];
					tets[id].matk[1] += faces[tets[id].faces[l]].heatTransRatio * (1.0/12.0) * faces[tets[id].faces[l]].area * matk2array[j];
					//DSTR << "tets[id].matk2にfaces[tets[id].faces[l]].heatTransRatio * (1.0/12.0) * faces[tets[id].faces[l]].area * matk2array[" << j << "]"<< "を加算: " <<faces[tets[id].faces[l]].heatTransRatio * (1.0/12.0) * faces[tets[id].faces[l]].area * matk2array[j] << std::endl;
					//DSTR << "tets[id].matk2 +=  " << tets[id].matk2 << std::endl;
				}
				//else{
				//	///	IDと一致しない場合には、matk2array[j]には全成分0を入れる
				//	///	としたいところだが、
				//	//matk2array[j] =0.0 * matk2array[j];
				//	//DSTR << "matk2array[" << j << "]: " << matk2array[j] << std::endl;
				//}
			}
		}
		///	SurfaceFaceじゃなかったら、matk2arrayには0を入れる
		//else{
		//	//matk2array[l];
		//}
	}

	//DSTR << "matk2array:" << std::endl;
	//for(unsigned i=0;i<4;i++){
	//	DSTR <<i <<": " << matk2array[i] << std::endl;
	//}

	//k2 = k21 + k22 + k23 + k24
	//for(unsigned i=0; i < 4; i++){
	//	matk2 += matk2array[i];
	//	//	for debug
	//	//DSTR << "matk2 に matk2array = k2" << i+1 <<"まで加算した行列" << std::endl;
	//	//DSTR << matk2 << std::endl;
	//}
	
	//for debug
	//DSTR << "節点（";
	//for(unsigned i =0; i < 4; i++){
	//	DSTR << tets[id].vertices[i] << "," ;
	//}
	//DSTR << ")で構成される四面体の" << std::endl;
	//DSTR << "matk2 : " << std::endl;
	//DSTR << matk2 << std::endl;
	//int hogeshishi =0;
	
	//DSTR << "Inner Function _ matk2t tets[id].matk2: " <<tets[id].matk2 << std::endl;
}
void PHFemMeshThermo::CreateMatk2(unsigned id,Tet tets){
	//この計算を呼び出すときに、各四面体ごとに計算するため、四面体の0番から順にこの計算を行う
	//四面体を構成する4節点を節点の配列(Tetsには、節点の配列が作ってある)に入っている順番を使って、面の計算を行ったり、行列の計算を行ったりする。
	//そのため、この関数の引数に、四面体要素の番号を取る

	//最後に入れる行列を初期化
	for(unsigned i =0; i < 4 ;i++){
		for(unsigned j =0; j < 4 ;j++){
			//matk2[i][j] = 0.0;
			tets.matk[1][i][j] = 0.0;
		}
	}

	//2)	
	//l=0の時k21,1:k22, 2:k23, 3:k24	を生成
	for(unsigned l= 0 ; l < 4; l++){
		//matk2array[l] = matk2temp;
		matk2array[l] = Create44Mat21();
		//	1行i列を0に
		for(int i=0;i<4;i++){
			matk2array[l][l][i] = 0.0;
		}
		//	i行1列を0に
		for(int i=0;i<4;i++){
			matk2array[l][i][l] = 0.0;
		}
		//	k_21	
		// =	|0 0 0 0 |
		//		|0 2 1 1 |
		//		|0 1 2 1 |
		//		|0 1 1 2 |
		//	for debug
		//DSTR <<"matk2array[" << l << "] : " << std::endl;
		//DSTR << matk2array[l] << std::endl;

		//係数の積をとる
		//この節点で構成される四面体の面積の積をとる

		//節点を見てみよう♪
		//for(unsigned i =0; i < 4 ; i++){
		//	DSTR << "k2"<< l << "行列の "<< i <<"番目の節点は" << tets.vertices[i] << std::endl;
		//}
	
		//四面体の節点1,2,3(0以外)で作る三角形の面積
		//l==0番目の時、 123	を代入する
		//l==1			0 23
		//l==2			01 3
		//l==3			012
		//をCalcTriangleAreaに入れることができるようにアルゴリズムを考える。
		//k21
		//この面は外殻か？外殻なら、計算を実行
			///	vertices
			///	surfaceVerticesかどうかで判定
		//areaは計算されていないか？されているのなら、faces.areaの値を使う
		if(l==0){
			///	面の熱伝達係数を、
			double tempHTR = (vertices[tets.vertices[1]].heatTransRatio + vertices[tets.vertices[2]].heatTransRatio + vertices[tets.vertices[3]].heatTransRatio ) / 3.0;
			matk2array[l] = tempHTR * (1.0/12.0) * CalcTriangleArea( tets.vertices[1],tets.vertices[2],tets.vertices[3] ) * matk2array[l];
		}
		//	k22
		else if(l==1){
			double tempHTR = (vertices[tets.vertices[0]].heatTransRatio + vertices[tets.vertices[2]].heatTransRatio + vertices[tets.vertices[3]].heatTransRatio ) / 3.0;
			matk2array[l] = tempHTR * (1.0/12.0) * CalcTriangleArea( tets.vertices[0],tets.vertices[2],tets.vertices[3] ) * matk2array[l];
		}
		//	k23
		else if(l==2){
			double tempHTR = (vertices[tets.vertices[0]].heatTransRatio + vertices[tets.vertices[1]].heatTransRatio + vertices[tets.vertices[3]].heatTransRatio ) / 3.0;
			matk2array[l] = tempHTR * (1.0/12.0) * CalcTriangleArea( tets.vertices[0],tets.vertices[1],tets.vertices[3] ) * matk2array[l];
		}
		//	k24
		else if(l==3){
			double tempHTR = (vertices[tets.vertices[0]].heatTransRatio + vertices[tets.vertices[1]].heatTransRatio + vertices[tets.vertices[2]].heatTransRatio ) / 3.0;
			matk2array[l] = tempHTR * (1.0/12.0) * CalcTriangleArea( tets.vertices[0],tets.vertices[1],tets.vertices[2] ) * matk2array[l];
		}
		//for debug
		//DSTR << "matk2array[" << l << "]の完成版は↓" << std::endl;
		//DSTR << matk2array[l] << std::endl;
	}

	//k2 = k21 + k22 + k23 + k24
	for(unsigned i=0; i < 4; i++){
		tets.matk[1] += matk2array[i];
		//	for debug
		//DSTR << "matk2 に matk2array = k2" << i+1 <<"まで加算した行列" << std::endl;
		//DSTR << matk2 << std::endl;
	}
	
	//for debug
	//DSTR << "節点（";
	//for(unsigned i =0; i < 4; i++){
	//	DSTR << tets.vertices[i] << "," ;
	//}
	//DSTR << ")で構成される四面体の" << std::endl;
	//DSTR << "matk2 : " << std::endl;
	//DSTR << matk2 << std::endl;
	//int hogeshishi =0;
}

void PHFemMeshThermo::SetInitThermoConductionParam(double thConduct0,double roh0,double specificHeat0,double heatTrans0){
	thConduct = thConduct0;
	rho = roh0;
	specificHeat = specificHeat0;
	heatTrans = heatTrans0;
}
void PHFemMeshThermo::SetThermalEmissivityToVtx(unsigned id,double thermalEmissivity){
	vertices[id].thermalEmissivity = thermalEmissivity;
}
void PHFemMeshThermo::SetThermalEmissivityToVerticesAll(double thermalEmissivity){
	for(unsigned i =0; i < vertices.size(); i++){
		vertices[i].thermalEmissivity = thermalEmissivity;
	}
}
void PHFemMeshThermo::SetHeatTransRatioToAllVertex(){
	for(unsigned i =0; i < vertices.size() ; i++){
		vertices[i].heatTransRatio = heatTrans;
	}
}


void PHFemMeshThermo::SetTempAllToTVecAll(unsigned size){
	for(unsigned i =0; i < size;i++){
		TVecAll[i] = vertices[i].temp;
	}
}


void PHFemMeshThermo::CreateTempMatrix(){
	unsigned int dmnN = vertices.size();
	TVecAll.resize(dmnN);
	SetTempAllToTVecAll(dmnN);
	//for(std::vector<unsigned int>::size_type i=0; i < dmnN ; i++){
	//	TVecAll[i] = vertices[i].temp;
	//}

	//for Debug
	//for(unsigned int i =0; i < dmnN; i++){
	//	DSTR << i <<" : " << TVecAll[i] << std::endl;
	//}
	//for debug
	//for(std::vector<unsigned int>::size_type i =0; i < vertices.size(); i++){
	//	DSTR << i << " : " << &vertices[i] << std::endl;
	//}

}


void PHFemMeshThermo::SetTempToTVecAll(unsigned vtxid){
	TVecAll[vtxid] = vertices[vtxid].temp;
}

void PHFemMeshThermo::UpdateheatTransRatio(unsigned id,double heatTransRatio){
	//if(vertices[id].heatTransRatio != heatTransRatio){	//異なっていたら
	//	vertices[id].heatTransRatio = heatTransRatio;	
	//	///	αを含む行列の更新	K2,f3
	//	///	f3
	//	for(unsigned i =0; i < vertices[1].tets.size(); i++){
	//		CreateVecf3(tets[vertices[id].tets[i]]);
	//	}
	//	///	K3

	//}
	///	同じなら何もしない
}

void PHFemMeshThermo::SetLocalFluidTemp(unsigned i,double temp){
	vertices[i].Tc = temp;			//節点の周囲流体温度の設定
}

void PHFemMeshThermo::SetVertexTemp(unsigned i,double temp){
	vertices[i].temp = temp;
	SetTempToTVecAll(i);
}

void PHFemMeshThermo::SetVerticesTempAll(double temp){
	for(std::vector<unsigned int>::size_type i=0; i < vertices.size() ; i++){
			vertices[i].temp = temp;
		}
}


void PHFemMeshThermo::SetvecFAll(unsigned id,double dqdt){
	vecFAll[id][0] = dqdt;
}

}



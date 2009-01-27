#include "BoxStack.h"
#include "HapticProcess.h"
#include <vector>
#include <iostream>
#include <sstream>
#include <cmath>
#include <ctime>
#include <string>
#include <GL/glut.h>
#include <windows.h>
#include <Physics/PHConstraintEngine.h>
#include <Base/TMatrix.h>
#include <math.h>

BoxStack bstack;
#define FULLSCREAN 0;
void BoxStack::DesignObject(){
	CreateObject();
}
void BoxStack::CreateObject(){

//�@�`���desc ----------------------------------------------------------------------
	CDBoxDesc dBox;
	{
		dBox.boxsize = Vec3f(1.0,0.5,1.5);
		shapeBox = XCAST(GetSdk()->GetPHSdk()->CreateShape(dBox));
		shapeBox->SetName("shapeBox");
	}
	CDSphereDesc dSphere;
	{
		dSphere.radius = 1.2;
		shapeSphere = XCAST(GetSdk()->GetPHSdk()->CreateShape(dSphere));
		shapeSphere->SetName("shapeSphere");
	}
	CDBoxDesc dBoxFloor;
	{
		dBoxFloor.boxsize=Vec3f(100.0f, 1.0f, 100.0f);
		shapeFloor = DCAST(CDBoxIf, GetSdk()->GetPHSdk()->CreateShape(dBoxFloor));
		shapeFloor->SetName("shapeFloor");
	}
//�@solid��desc-----------------------------------------------------------------------------------
	// soFloor�p��desc
	PHSolidDesc floorDesc;
	floorDesc.mass = 1e20f;
	floorDesc.inertia *= 1e30f;
	soFloor = phscene->CreateSolid(floorDesc);		// ���̂�desc�Ɋ�Â��č쐬
	{
		soFloor->SetDynamical(false);
		soFloor->SetGravity(false);
		soFloor->SetFramePosition(Vec3d(0.0, -5.0, 0.0));//���̈ʒu���w�肷��
	}

	// soObject�p��desc
	desc.mass = 0.05;
	desc.inertia = 0.033 * Matrix3d::Unit();

	soBox2[0] = phscene->CreateSolid(desc);
	{
		soBox2[0]->SetDynamical(false);
		soBox2[0]->SetGravity(false);
		soBox2[0]->SetFramePosition(Vec3d(0.0, 0.0, 0.0));//���̈ʒu���w�肷��
	}
	soBox2[1] = phscene->CreateSolid(desc);
	{
		soBox2[1]->SetDynamical(true);
		soBox2[1]->SetGravity(false);
		soBox2[1]->SetFramePosition(Vec3d(0.0, 0.0, 2.0));//���̈ʒu���w�肷��
	}
	

//	�`��o�^-----------------------------------------------------------------------------------
	//�@soFloor�̌`��o�^
	soFloor->AddShape(shapeFloor);
	soFloor->SetName("solidFloor");

	soBox2[0]->AddShape(shapeBox);
	soBox2[0]->SetName("solidBox");

	soBox2[1]->AddShape(shapeBox);
	soBox2[1]->SetName("solidBox2");
// �W���C���g�̐ڑ�-----------------------------------------------------------------------------
	double K=3;
	double D1=0.001;
	double D2=0.01;
	double l=1.5;
	PH3ElementBallJointDesc d3Ball2;

	{
		d3Ball2.poseSocket.Pos()	= Vec3f(0.0f, 0.0f, l/2);
		d3Ball2.poseSocket.Ori()	= Quaternionf::Rot(Rad(-180.0f), 'x');
		d3Ball2.posePlug.Pos()	= Vec3f(0.0f, 0.0f, -l/2);
		d3Ball2.posePlug.Ori()	= Quaternionf::Rot(Rad(-180.0f), 'x');
		d3Ball2.spring			= K;
		d3Ball2.damper			= D1;
		d3Ball2.secondDamper		= D2;
		d3Ball2.yieldStress			=0.1;
		d3Ball2.hardnessRate		=1e3;
	}
	jointBox = phscene->CreateJoint( soBox2[0], soBox2[1], d3Ball2);
	phscene->SetContactMode(soBox2[0], soBox2[1], PHSceneDesc::MODE_NONE);
}


void BoxStack::GrabSpring(double dt){
	double K,D;
	double h=dt;
	Vec3d Vg,Xg,Vp,Xp,grabSpringForce;

	//�|�C���^�ƍ��̊Ԃ̃o�l�_���p�̐ݒ�
	K=2.5;
	D=0.1;
	
	if(grabObject.blocal && grabObjectFlag==true){
		phscene->SetContactMode(grabObject.phSolidIf, soPointer, PHSceneDesc::MODE_NONE);

		Xg = grabObject.phSolidIf->GetCenterPosition();//+grabObject.phSolid.GetPose()*grabObject.closestPoint;
		Vg = grabObject.phSolidIf->GetVelocity();

		Xp = hprocess.hpointer.GetCenterPosition();
		Vp = hprocess.hpointer.GetVelocity();

		//�o�l�_���p�̌v�Z
		grabSpringForce=(K*(Xp-Xg)+(K*h+D)*(Vp-Vg))/(1+h);

		grabObject.phSolidIf->AddForce(grabSpringForce,Xg);	//�Ώە��̂ɗ͂�������
		grabForce=grabSpringForce;							//�|�C���^�ɉ������
		
	}else if(grabObject.blocal && !grabObjectFlag==true){
		grabForce=Vec3d(0.0, 0.0, 0.0);
	}

}


BoxStack::BoxStack(){
	// xfile�̃p�X�̎w��
	filePath			= "./xFiles/";
	fileName			= "sceneHapticJointTest.x";
	hPointerName = "soHapticPointer";

	// �ϐ��̏�����
	bsync=false;
	calcPhys=true;
	dt = 0.02;
	gravity =  Vec3d(0, -9.8 , 0);
	nIter = 15;
	bGravity = true;
	phscene = NULL;
	render = NULL;
	range = 0.5;
	bDebug = true;
	neighborObjects.clear();
	bStep = true;
	countKey = 0;
	grabObjectFlag = false;
	grabForceRate = 1;
	
}
namespace Spr{
void FASTCALL ContFindCommonPointSaveParam(const CDConvex* a, const CDConvex* b,
	const Posed& a2w, const Posed& b2w, const Vec3d& dir, double start, double end,
	Vec3d& normal, Vec3d& pa, Vec3d& pb, double& dist);
}

void BoxStack::Init(int argc, char* argv[]){
	//glutGameModeString("width=800 height=600 bpp~24 hertz>=100");
	//glutEnterGameMode();
	FWAppGLUT::Init(argc, argv);

	GetSdk()->Clear();															// SDK�̍쐬
	// xfile�̃��[�h
	stringstream imputFileName;
	imputFileName << filePath << fileName;
	GetSdk()->LoadScene(imputFileName.str().c_str());				// �V�[����xfile���烍�[�h
	phscene = GetSdk()->GetScene()->GetPHScene();
	phscene->SetStateMode(true);
	DesignObject();

	states = ObjectStatesIf::Create();
	states2 = ObjectStatesIf::Create();

	

	phscene->SetGravity(gravity);				
	phscene->SetTimeStep(dt);
	phscene->SetNumIteration(nIter);

	SetHapticPointer();

	FWWinDesc windowDesc;
#if FULLSCREAN
	windowDesc.height=1024;
	windowDesc.width=768;
	windowDesc.fullscreen=true;
#endif
	windowDesc.title = "Test";
	window = CreateWin(windowDesc);
	window->scene = GetSdk()->GetScene();

	InitCameraView();
}

void BoxStack::SetHapticPointer(){
	soPointer = DCAST(PHSolidIf, phscene->FindObject(hPointerName));
	soPointer->SetDynamical(false);
	soPointer->SetFrozen(true);
	phscene->SetContactMode(soPointer, PHSceneDesc::MODE_NONE);
}

void BoxStack::InitCameraView(){
	std::istringstream issView(
		"((0.999833 0.00643529 -0.0170968 -0.114308)"
		"(-7.16819e-010 0.935897 0.352274 2.35527)"
		"(0.0182679 -0.352215 0.935741 6.25627)"
		"(     0      0      0      1))"
	);
	issView >> cameraInfo.view;
}

void BoxStack::Start(){
	instance = this;
	if (!NWin()){
		CreateWin();
		wins.back()->SetScene(GetSdk()->GetScene());
	}
	glutIdleFunc(FWAppGLUT::GlutIdleFunc);
	glutMainLoop();
}

void BoxStack::Idle(){
	PhysicsStep();
}

void BoxStack::PhysicsStep(){
	if (bsync) return;
	if (calcPhys){
		UpdateHapticPointer();
		for(unsigned i = 0; i < neighborObjects.size(); i++){
			if(!neighborObjects[i].blocal) continue;
			neighborObjects[i].lastvel.v() = neighborObjects[i].phSolidIf->GetVelocity();
			neighborObjects[i].lastvel.w() = neighborObjects[i].phSolidIf->GetAngularVelocity();
		}
		naga=true;
		if(bStep) phscene->Step();
		else if (bOneStep){
			phscene->Step();
			bOneStep = false;
		}

		for(unsigned i = 0; i < neighborObjects.size(); i++){
			if(!neighborObjects[i].blocal) continue;
			SpatialVector curvel;
			curvel.v() = neighborObjects[i].phSolidIf->GetVelocity();
			curvel.w() = neighborObjects[i].phSolidIf->GetAngularVelocity();
			neighborObjects[i].curb = (curvel - neighborObjects[i].lastvel) / dt;
		}
		calcPhys = false;
		naga=false;
		FindNearestObject();	// �ߖT���̂̎擾
		PredictSimulation();
		glutPostRedisplay();
		
	}
	if (hapticcount < dt/hprocess.dt) return;
	hapticcount -= dt/hprocess.dt;
	bsync = true;
	calcPhys = true;

	static DWORD last;
	//if (phscene->GetCount() % 10==0){
	//	DWORD t = timeGetTime();
	//	DSTR << phscene->GetCount() << " dt:" << t-last << std::endl;
	//	last = t;
	//}
}

void BoxStack::Display(){
	// �`��̐ݒ�
	GetSdk()->SetDebugMode(bDebug);
	render = window->render->Cast();
	DCAST(GRDebugRenderIf, render)->EnableRenderForce(bDebug, 0.7);

//	// �`�惂�[�h�̐ݒ�
	if(GetSdk() && GetSdk()->GetScene())
		GetSdk()->GetScene()->Draw(render, bDebug);
	// �J�������W�̎w��
	GRCameraIf* cam = window->scene->GetGRScene()->GetCamera();
	if (cam && cam->GetFrame()){
		cam->GetFrame()->SetTransform(cameraInfo.view);
	}else{
		window->render->SetViewMatrix(cameraInfo.view.inv());
	}

	// �`��̎��s
	if(!GetCurrentWin()) return;
	GRRenderIf*curRender =  GetCurrentWin()->GetRender();
	FWSceneIf* curScene = GetCurrentWin()->GetScene();

	GetSdk()->SwitchScene(curScene);
	GetSdk()->SwitchRender(curRender);

	if(!curRender) return;
	curRender->ClearBuffer();
	curRender->BeginScene();

	if (curScene) curScene->Draw(curRender, bDebug);

	//	�����̒ǉ�
	GRLightDesc ld;
	ld.diffuse = Vec4f(1,1,1,1) * 0.8f;
	ld.specular = Vec4f(1,1,1,1) * 0.8f;
	ld.ambient = Vec4f(1,1,1,1) * 0.4f;
	ld.position = Vec4f(1,1,1,0);
	render->PushLight(ld);
	if(bDebug){
	DisplayContactPlane();
	DisplayLineToNearestPoint();			// �͊o�|�C���^�ƍ��̂̋ߖT�_�̊Ԃ��Ȃ�
	//	DrawHapticSolids();
	}
	render->PopLight();	//	�����̍폜

	curRender->EndScene();
	glFinish();
	glutSwapBuffers();
}

void BoxStack::UpdateHapticPointer(){
	// cameraInfo.view.Rot()�������ė͊o�|�C���^�̑�����J��������]�ɂ��킹��
	soPointer->SetFramePosition(phpointer.GetFramePosition());//cameraInfo.view.Rot() * phpointer.GetFramePosition());				
	soPointer->SetOrientation(phpointer.GetOrientation());					
	soPointer->SetVelocity(phpointer.GetVelocity());
	soPointer->SetAngularVelocity(phpointer.GetAngularVelocity());	
	soPointer->SetDynamical(false);
};

void BoxStack::FindNearestObject(){
	// GJK���g���ċߖT���̂ƋߖT���̂̍ŋߓ_���擾
	// ��������ׂĂ�shape�������̂ɂ��Ă��

	// �V�[���ŐV���ɍ��̂��������ꂽ�烍�[�J���ŃV�~�����[�V�������Ă��邩�ǂ����̏�������sceneSolids�Ɋi�[����
	// sceneSolids�͗͊o�|�C���^�������Ă���
	sceneSolids.clear();
	PHSolidIf** solids = phscene->GetSolids();
	for(int i = 0; i < phscene->NSolids(); i++){
		sceneSolids.resize(sceneSolids.size() + 1);
		sceneSolids.back().phSolidIf = solids[i];
		sceneSolids.back().bneighbor = false;
		sceneSolids.back().blocal = false;
	}

	// sceneSolids�ŐV��������������neighborObjects�ɒǉ�����
	//neighborObjects����������clear���Ă�ƁC�̂�neighborObjects��ۑ�����K�v������̂� 
	//����neighborObjects��scene�̂��ׂĂ�solid���i�[���Ă���D
	//neighborObjects�͗͊o�|�C���^������blocal�����false�ɂ��Ă����K�v������D
	for(unsigned i = (int)neighborObjects.size(); i < sceneSolids.size(); i++){
		neighborObjects.resize(i + 1);
		neighborObjects.back().phSolidIf = sceneSolids[i].phSolidIf;
		neighborObjects.back().bneighbor = false;
		neighborObjects.back().blocal = false;
	}

#if 1		
	// �����ōi�������̂ɂ���GJK���s���D�����ōi�邱�Ƃ�GJK������񐔂����Ȃ��ł���D
	// Solid��BBox���x���ł̌�������(z���\�[�g)�D�����̂�����̖����g�����O		
	//1. BBox���x���̏Փ˔���
	size_t N = neighborObjects.size();
	Vec3f dir(0,0,1);
	Edges edges;
	edges.resize(2 * N);
	Edges::iterator eit = edges.begin();
	for(int i = 0; i < N; ++i){
		// ���[�J����������ׂ�false�ɂ���
		neighborObjects[i].bneighbor = false;
		DCAST(PHSolid, neighborObjects[i].phSolidIf)->GetBBoxSupport(dir, eit[0].edge, eit[1].edge);
		Vec3d dPos = neighborObjects[i].phSolidIf->GetDeltaPosition();
		float dLen = (float) (dPos * dir);
		if (dLen < 0){
			eit[0].edge += dLen;
		}else{
			eit[1].edge += dLen;
		}
		eit[0].index = i; eit[0].bMin = true;
		eit[1].index = i; eit[1].bMin = false;
		eit += 2;
	}
	sort(edges.begin(), edges.end());
	//�[���猩�Ă����āC�ڐG�̉\��������m�[�h�̔��������D
	typedef set<int> SolidSet;
	SolidSet cur;							//	���݂�Solid�̃Z�b�g
	bool found = false;

	for(Edges::iterator it = edges.begin(); it != edges.end(); ++it){
		if (it->bMin){						//	���[��������C���X�g���̕��̂Ɣ���
			for(SolidSet::iterator itf=cur.begin(); itf != cur.end(); ++itf){
				int f1 = it->index;
				int f2 = *itf;
				if (f1 > f2) std::swap(f1, f2);
				// �ߖT���̂Ƃ��Č���
				if(neighborObjects[f1].phSolidIf == soPointer){
					neighborObjects[f2].bneighbor = true;
				}else if(neighborObjects[f2].phSolidIf == soPointer){
					neighborObjects[f1].bneighbor = true;
				}
			}
			cur.insert(it->index);
		}else{
			cur.erase(it->index);			//	�I�[�Ȃ̂ō폜�D
		}
	}

	for(size_t i = 0; i < neighborObjects.size(); i++){
		if(neighborObjects[i].bneighbor) continue;
		neighborObjects[i].bfirstlocal = false;			//�ߖT���̂łȂ��̂�false�ɂ���
		neighborObjects[i].blocal = false;
	}
#else
	// ���ׂĂ̕��̂ɂ���GJK���s��
	for(int i = 0; i < neighborObjects.size(); i++){
		neighborObjects[i].bneighbor = true;
		if(neighborObjects[i].phSolidIf == soPointer) neighborObjects[i].bneighbor = false;
	}
#endif

	// GJK�ŋߖT�_�����߁C�͊o�|�C���^�ŋߖT�̕��̂����肷��
	// �ŋߖT���̂�������blocal��true�ɂ��CphSolid��phSolidIf���R�s�[����
	// blocal�����ł�true��������R�s�[�ς݂Ȃ̂ŋߖT�_�����R�s�[����
	for(unsigned i = 0; i < (int)neighborObjects.size(); i++){
		if(!neighborObjects[i].bneighbor) continue;															// �ߖT�łȂ���Ύ���

		CDConvex* a = DCAST(CDConvex, neighborObjects[i].phSolidIf->GetShape(0));		// ���̂����ʌ`��
		CDConvex* b = DCAST(CDConvex, soPointer->GetShape(0));									// �͊o�|�C���^�̓ʌ`��
		Posed a2w, b2w;																								// ���̂̃��[���h���W
/*		if(neighborObjects[i].blocal) 	a2w = neighborObjects[i].phSolid.GetPose();				// blocal��true�Ȃ�ŐV�̏��ł��
		else */								a2w = neighborObjects[i].phSolidIf->GetPose();
		b2w = soPointer->GetPose();																			// �͊o�|�C���^�̃��[���h���W
		Vec3d pa ,pb;																									// pa:���̂̋ߖT�_�Cpb:�͊o�|�C���^�̋ߖT�_�i���[�J�����W�j
		pa = pb = Vec3d(0.0, 0.0, 0.0);
		FindClosestPoints(a, b, a2w, b2w, pa, pb);															// GJK�ŋߖT�_�̎Z�o
		Vec3d wa = a2w * pa;																						// ���̋ߖT�_�̃��[���h���W
		Vec3d wb = b2w * pb;																						// �͊o�|�C���^�ߖT�_�̃��[���h���W
		Vec3d a2b = wb - wa;																						// ���̂���͊o�|�C���^�ւ̃x�N�g��
		Vec3d normal = a2b.unit();
		if(a2b.norm() < range){																					// �ߖT�_�܂ł̒�������ߖT���̂��i��
			if(a2b.norm() < 0.01){																				// �͊o�|�C���^�ƍ��̂����łɐڐG���Ă�����CCDGJK�Ŗ@�������߂�		
				pa = pb = Vec3d(0.0, 0.0, 0.0);
				Vec3d dir = -1.0 * neighborObjects[i].face_normal;
				if(dir == Vec3d(0.0, 0.0, 0.0) ){
					dir = -(soPointer->GetCenterPosition() - wa);
				}
				double dist = 0.0;
				int cp = ContFindCommonPoint(a, b, a2w, b2w, dir, -DBL_MAX, 1, normal, pa, pb, dist);
				if(cp != 1){
					ContFindCommonPointSaveParam(a, b, a2w, b2w, dir, -DBL_MAX, 1, normal, pa, pb, dist);
					DSTR << "ContFindCommonPoint do not find contact point" << endl;
				}
			}
			
			grabObject.phSolid = *DCAST(PHSolid, neighborObjects[i].phSolidIf);	

			if(!neighborObjects[i].blocal){																			// ���߂čŋߖT���̂ɂȂ�����
				neighborObjects[i].bfirstlocal = true;
				neighborObjects[i].phSolid = *DCAST(PHSolid, neighborObjects[i].phSolidIf);		// �V�[���������̂̒��g��͊o�v���Z�X�Ŏg�����́i���́j�Ƃ��ăR�s�[����
#ifdef _DEBUG
				if (neighborObjects[i].face_normal * normal < 0.8){
					DSTR << "Too big change on normal" << normal << std::endl;
				}
#endif
				neighborObjects[i].face_normal = normal;														// ���߂čŋߖT���̂ɂȂ����̂ŁC�O��̖@���͎g��Ȃ��D										
			}
			neighborObjects[i].blocal = true;																		// �ŋߖT���̂Ȃ̂�blocal��true�ɂ���
			neighborObjects[i].closestPoint = pa;																	// ���̋ߖT�_�̃��[�J�����W
			neighborObjects[i].pointerPoint = pb;																// �͊o�|�C���^�ߖT�_�̃��[�J�����W
			neighborObjects[i].last_face_normal = neighborObjects[i].face_normal;					// �O��̖@���i�@���̕�ԂɎg���j
			neighborObjects[i].face_normal = normal;															// ���̂���͊o�|�C���^�ւ̖@��



			if(grabObjectFlag==false){
				grabObject.phSolidIf=neighborObjects[i].phSolidIf;	
				grabObject.blocal = true ;
				grabObject.closestPoint = pa;																	// ���̋ߖT�_�̃��[�J�����W
				grabObject.pointerPoint = pb;																// �͊o�|�C���^�ߖT�_�̃��[�J�����W
				grabObject.last_face_normal = neighborObjects[i].face_normal;					// �O��̖@���i�@���̕�ԂɎg���j
				grabObject.face_normal = normal;												// ���̂���͊o�|�C���^�ւ̖@��
			}
			
		}else{
			neighborObjects[i].blocal = false;																		// �ŋߖT���̂ł͂Ȃ��̂�blocal��false�ɂ���
			neighborObjects[i].bfirstlocal = false;
		}
	}
}

#define DIVIDE_STEP

void BoxStack::PredictSimulation(){
	// neighborObjets��blocal��true�̕��̂ɑ΂��ĒP�ʗ͂������C�ڐG���Ă��邷�ׂĂ̕��̂ɂ��āC�^���W�����v�Z����
#ifdef DIVIDE_STEP

	states2->SaveState(phscene);			// �\���V�~�����[�V�����̂��߂Ɍ��݂̍��̂̏�Ԃ�ۑ�����
	//	LCP�̒��O�܂ŃV�~�����[�V�������Ă��̏�Ԃ�ۑ�
	phscene->ClearForce();
	phscene->GenerateForce();
	phscene->IntegratePart1();
#endif
	states->SaveState(phscene);			// �\���V�~�����[�V�����̂��߂Ɍ��݂̍��̂̏�Ԃ�ۑ�����
	
	for(unsigned i = 0; i < neighborObjects.size(); i++){
		if(!neighborObjects[i].blocal) continue;
		
		// ���݂̑��x��ۑ�
		SpatialVector currentvel, nextvel; 
		currentvel.v() = neighborObjects[i].phSolidIf->GetVelocity();											// ���݂̑��x
		currentvel.w() = neighborObjects[i].phSolidIf->GetAngularVelocity();									// ���݂̊p���x									
		Vec3d cPoint = neighborObjects[i].phSolidIf->GetPose() * neighborObjects[i].closestPoint;	// �͂�������_
		const float minTestForce = 0.5;
		if(neighborObjects[i].test_force_norm < minTestForce){
			neighborObjects[i].test_force_norm = minTestForce;		// �e�X�g�͂�0�Ȃ�1�ɂ��� 
		}

		// �S�����W�n����邽�߂̏���
		Vec3d rpjabs, vpjabs;
		rpjabs = cPoint - soPointer->GetCenterPosition();																							//�͊o�|�C���^�̒��S����ڐG�_�܂ł̃x�N�g��
		vpjabs = soPointer->GetVelocity() + soPointer->GetAngularVelocity() % rpjabs;													//�ڐG�_�ł̑��x
		Vec3d rjabs, vjabs;
		rjabs = cPoint - neighborObjects[i].phSolidIf->GetCenterPosition();																	//���̂̒��S����ڐG�_�܂ł̃x�N�g��
		vjabs = neighborObjects[i].phSolidIf->GetVelocity() + neighborObjects[i].phSolidIf->GetAngularVelocity() % rjabs;	//�ڐG�_�ł̑��x

		//�ڐ��x�N�g��t[0], t[1] (t[0]�͑��Α��x�x�N�g���ɕ��s�ɂȂ�悤�ɂ���)
		Vec3d n, t[2], vjrel, vjrelproj;
		n = -neighborObjects[i].face_normal;
		vjrel = vjabs - vpjabs;										// ���Α��x
		vjrelproj = vjrel - (n * vjrel) * n;						// ���Α��x�x�N�g����@���ɒ������镽�ʂɎˉe�����x�N�g��
		double vjrelproj_norm = vjrelproj.norm();
		if(vjrelproj_norm < 1.0e-10){							// �ˉe�x�N�g���̃m�������������Ƃ�
			t[0] = n % Vec3d(1.0, 0.0, 0.0);					// t[0]��@����Vec3d(1.0, 0.0, 0.0)�̊O�ςƂ���
			if(t[0].norm() < 1.0e-10)								// ����ł��m������������������
				t[0] = n % Vec3d(0.0, 1.0, 0.0);				// t[0]��@����Vec3d(0.0, 1.0, 0.0)�̊O�ςƂ���
			t[0].unitize();												// t[0]��P�ʃx�N�g���ɂ���
		}
		else{
			t[0] = vjrelproj / vjrelproj_norm;					// �m�������������Ȃ�������C�ˉe�x�N�g���̂܂�
		}
		t[1] = n % t[0];												// t[1]�͖@����t[0]�̊O�ςł��܂�

		// �����͂������Ȃ��ŃV�~�����[�V������1�X�e�b�v�i�߂�
#ifdef DIVIDE_STEP
		phscene->IntegratePart2();
#else
		phscene->Step();
#endif
		nextvel.v() = neighborObjects[i].phSolidIf->GetVelocity();
		nextvel.w() = neighborObjects[i].phSolidIf->GetAngularVelocity();
		neighborObjects[i].lastb = neighborObjects[i].b;
		neighborObjects[i].b = (nextvel - currentvel) / dt;

		TMatrixRow<6, 3, double> u;
		TMatrixRow<3, 3, double> force;
		// �@�������ɗ͂�������
		states->LoadState(phscene);
		force.col(0) = neighborObjects[i].test_force_norm * n;
		neighborObjects[i].phSolidIf->AddForce(force.col(0), cPoint);
#ifdef DIVIDE_STEP
		phscene->IntegratePart2();
#else
		phscene->Step();
#endif
		nextvel.v() = neighborObjects[i].phSolidIf->GetVelocity();
		nextvel.w() = neighborObjects[i].phSolidIf->GetAngularVelocity();
		u.col(0) = (nextvel - currentvel) /dt - neighborObjects[i].b;

		// n + t[0]�����ɗ͂�������
		states->LoadState(phscene);
		force.col(1) = neighborObjects[i].test_force_norm * (n + t[0]);
		neighborObjects[i].phSolidIf->AddForce(force.col(1), cPoint);
		phscene->IntegratePart2();
		nextvel.v() = neighborObjects[i].phSolidIf->GetVelocity();
		nextvel.w() = neighborObjects[i].phSolidIf->GetAngularVelocity();
		u.col(1) = (nextvel - currentvel) /dt - neighborObjects[i].b;

		// n+t[1]�����͂�������
		states->LoadState(phscene);
		force.col(2) = neighborObjects[i].test_force_norm * (n + t[1]);
		neighborObjects[i].phSolidIf->AddForce(force.col(2), cPoint);
#ifdef DIVIDE_STEP
		phscene->IntegratePart2();
#else
		phscene->Step();
#endif
		nextvel.v() = neighborObjects[i].phSolidIf->GetVelocity();
		nextvel.w() = neighborObjects[i].phSolidIf->GetAngularVelocity();
		u.col(2) = (nextvel - currentvel) /dt - neighborObjects[i].b;
		
		neighborObjects[i].A = u  * force.inv();				// �^���W��A�̌v�Z
		states->LoadState(phscene);								// ����state�ɖ߂��V�~�����[�V������i�߂�
	}
#ifdef DIVIDE_STEP
		states2->LoadState(phscene);								// ����state�ɖ߂��V�~�����[�V������i�߂�
#endif

}

void BoxStack::DisplayContactPlane(){
	for(unsigned int i = 0; i <  neighborObjects.size(); i++){
		if(!neighborObjects[i].blocal) continue;
		Vec3d pPoint = soPointer->GetPose() * neighborObjects[i].pointerPoint;
		Vec3d cPoint = neighborObjects[i].phSolidIf->GetPose() * neighborObjects[i].closestPoint;
		Vec3d normal = neighborObjects[i].face_normal;
		Vec3d v1(0,1,0);
		v1 +=  Vec3d(0, 0, 0.5) - Vec3d(0, 0, 0.5)*normal*normal;
		v1 -= v1*normal * normal;
		v1.unitize();
		Vec3d v2 = normal ^ v1;
		Vec4f moon(1.0, 1.0, 0.8, 0.3);
		render->SetMaterial( GRMaterialDesc(moon) );
		render->PushModelMatrix();
		Vec3d offset = 0.02 * normal;
		render->SetLighting( false );
		render->SetAlphaTest(true);
		render->SetAlphaMode(render->BF_SRCALPHA, render->BF_ONE);
		cPoint += offset/2;
		glBegin(GL_QUADS);
			// �ڐG�ʒ��	
			glVertex3d(cPoint[0] + v1[0] + v2[0], cPoint[1] + v1[1] + v2[1], cPoint[2] + v1[2] + v2[2]);
			glVertex3d(cPoint[0] - v1[0] + v2[0], cPoint[1] - v1[1] + v2[1], cPoint[2] - v1[2] + v2[2]);
			glVertex3d(cPoint[0] - v1[0] - v2[0], cPoint[1] - v1[1] - v2[1], cPoint[2] - v1[2] - v2[2]);
			glVertex3d(cPoint[0] + v1[0] - v2[0], cPoint[1] + v1[1] - v2[1], cPoint[2] + v1[2] - v2[2]);
			// ����1
			glVertex3d(cPoint[0] + v1[0] + v2[0] + offset[0], 
							cPoint[1] + v1[1] + v2[1] + offset[1], 
							cPoint[2] + v1[2] + v2[2] + offset[2]);
			glVertex3d(cPoint[0] + v1[0] + v2[0], 
							cPoint[1] + v1[1] + v2[1], 
							cPoint[2] + v1[2] + v2[2]);
			glVertex3d(cPoint[0] - v1[0] + v2[0], 
							cPoint[1] - v1[1] + v2[1], 
							cPoint[2] - v1[2] + v2[2]);
			glVertex3d(cPoint[0] - v1[0] + v2[0] + offset[0], 
							cPoint[1] - v1[1] + v2[1] + offset[1], 
							cPoint[2] - v1[2] + v2[2] + offset[2]);
			// ����2
			glVertex3d(cPoint[0] - v1[0] + v2[0] + offset[0], 
							cPoint[1] - v1[1] + v2[1] + offset[1], 
							cPoint[2] - v1[2] + v2[2] + offset[2]);
			glVertex3d(cPoint[0] - v1[0] + v2[0], 
							cPoint[1] - v1[1] + v2[1], 
							cPoint[2] - v1[2] + v2[2]);
			glVertex3d(cPoint[0] - v1[0] - v2[0], 
							cPoint[1] - v1[1] - v2[1], 
							cPoint[2] - v1[2] - v2[2]);
			glVertex3d(cPoint[0] - v1[0] - v2[0] + offset[0], 
							cPoint[1] - v1[1] - v2[1] + offset[1], 
							cPoint[2] - v1[2] - v2[2] + offset[2]);
			// ����3
			glVertex3d(cPoint[0] - v1[0] - v2[0] + offset[0], 
							cPoint[1] - v1[1] - v2[1] + offset[1], 
							cPoint[2] - v1[2] - v2[2] + offset[2]);
			glVertex3d(cPoint[0] - v1[0] - v2[0], 
							cPoint[1] - v1[1] - v2[1], 
							cPoint[2] - v1[2] - v2[2]);
			glVertex3d(cPoint[0] + v1[0] - v2[0], 
							cPoint[1] + v1[1] - v2[1], 
							cPoint[2] + v1[2] - v2[2]);
			glVertex3d(cPoint[0] + v1[0] - v2[0] + offset[0], 
							cPoint[1] + v1[1] - v2[1] + offset[1], 
							cPoint[2] + v1[2] - v2[2] + offset[2]);
			// ����4
			glVertex3d(cPoint[0] + v1[0] - v2[0] + offset[0], 
							cPoint[1] + v1[1] - v2[1] + offset[1], 
							cPoint[2] + v1[2] - v2[2] + offset[2]);
			glVertex3d(cPoint[0] + v1[0] - v2[0], 
							cPoint[1] + v1[1] - v2[1], 
							cPoint[2] + v1[2] - v2[2]);
			glVertex3d(cPoint[0] + v1[0] + v2[0], 
							cPoint[1] + v1[1] + v2[1],
							cPoint[2] + v1[2] + v2[2]);
			glVertex3d(cPoint[0] + v1[0] + v2[0] + offset[0], 
							cPoint[1] + v1[1] + v2[1] + offset[1], 
							cPoint[2] + v1[2] + v2[2] + offset[2]);
			// �ڐG�ʏ��
			glVertex3d(cPoint[0] - v1[0] + v2[0] + offset[0], 
							cPoint[1] - v1[1] + v2[1] + offset[1], 
							cPoint[2] - v1[2] + v2[2] + offset[2]);
			glVertex3d(cPoint[0] + v1[0] + v2[0] + offset[0], 
							cPoint[1] + v1[1] + v2[1] + offset[1], 
							cPoint[2] + v1[2] + v2[2] + offset[2]);
			glVertex3d(cPoint[0] + v1[0] - v2[0] + offset[0], 
							cPoint[1] + v1[1] - v2[1] + offset[1], 
							cPoint[2] + v1[2] - v2[2] + offset[2]);
			glVertex3d(cPoint[0] - v1[0] - v2[0] + offset[0], 
							cPoint[1] - v1[1] - v2[1] + offset[1], 
							cPoint[2] - v1[2] - v2[2] + offset[2]);
		glEnd();
		render->SetLighting( true);
		render->SetAlphaTest(false);
		render->PopModelMatrix();
		glEnable(GL_DEPTH_TEST);
	}
}

void BoxStack::DisplayLineToNearestPoint(){
	GLfloat moon[]={0.8,0.8,0.8};
	for(unsigned int i = 0; i <  neighborObjects.size(); i++){
		if(!neighborObjects[i].blocal) continue;
		Vec3d pPoint = soPointer->GetPose() * neighborObjects[i].pointerPoint;
		Vec3d cPoint = neighborObjects[i].phSolidIf->GetPose() * neighborObjects[i].closestPoint;
		Vec3d normal = neighborObjects[i].face_normal;
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, moon);
		glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, moon);
		glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, moon);
		glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, moon);
		glDisable(GL_DEPTH_TEST);
		glBegin(GL_LINES);
		glVertex3f(pPoint.X() + normal[0], pPoint.Y() + normal[1], pPoint.Z() + normal[2]);
		glVertex3f(cPoint.X(), cPoint.Y(), cPoint.Z());
		glEnd();
		glEnable(GL_DEPTH_TEST);
	}
}

void BoxStack::DrawHapticSolids(){
	GLfloat purple[] = {1.0, 0.0, 1.0, 0.0};
	GRDebugRenderIf* render = GetCurrentWin()->GetRender()->Cast();
	render->SetMaterialSample(GRDebugRenderIf::GRAY);
	for(unsigned int i = 0; i < hapticsolids.size(); i++){
		PHSolid* solid = &hapticsolids[i];		
		PHSolidIf* solidIf = solid->Cast(); 
		render->DrawSolid(solidIf);
	}
};


void BoxStack::Keyboard(unsigned char key){
	states->ReleaseState(phscene);
	states2->ReleaseState(phscene);
	switch (key) {
		case 'w':
			InitCameraView();
			DSTR << "InitCameraView" << endl;
			break;
		case 'd':
			{
				if(bDebug){
					bDebug = false;
//					DCAST(GRDebugRenderIf, render)->EnableRenderAxis(bDebug, 0.1);
					DCAST(GRDebugRenderIf, render)->EnableRenderForce(bDebug, 100);
					DCAST(GRDebugRenderIf, render)->EnableRenderContact(bDebug);
					DSTR << "Debug Mode OFF" << endl;
				}
				else if(!bDebug) {
					bDebug = true;
//					DCAST(GRDebugRenderIf, render)->EnableRenderAxis(bDebug, 0.1);
					DCAST(GRDebugRenderIf, render)->EnableRenderForce(bDebug, 0.7);
					DCAST(GRDebugRenderIf, render)->EnableRenderContact(bDebug);
					DSTR << "Debug Mode ON" << endl;
				}
			}
			break;
		case 'g':
			if(bGravity){
				bGravity = false;
				Vec3d zeroG = Vec3d(0.0, 0.0, 0.0);
				phscene->SetGravity(zeroG);
				DSTR << "Gravity OFF" << endl;
			}else{
				bGravity = true;
				phscene->SetGravity(gravity);
				DSTR << "Gravity ON" << endl;
			}
			break;
		case 's':
			bStep = false;
			bOneStep = true;
			break;
		case 'a':
			bStep = true;
			break;

		case ' ':
			if(grabObjectFlag==true){
				grabObjectFlag=false;
			}else{
				grabObjectFlag=true;
			}
			break;
		case '/':
			grabForceRate+=0.01;
			cout<<"grabForceRate"<<grabForceRate<<endl;
			break;
		case '.':
			grabForceRate-=0.01;
			cout<<"grabForceRate"<<grabForceRate<<endl;
			break;


		default:
			break;
	}
}
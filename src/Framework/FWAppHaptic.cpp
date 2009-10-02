#include "Framework/SprFWAppHaptic.h"
#include <iostream>
#include <sstream>
#include <Framework/FWInteractScene.h>

using namespace std;

FWAppHaptic::FWAppHaptic(){
	bStep = true;
	bDrawInfo = false;
}

void FWAppHaptic::Init(int argc, char* argv[]){
	/// Sdk�̏������C�V�[���̍쐬
	CreateSdk();
	GetSdk()->Clear();										// SDK�̏�����
	GetSdk()->CreateScene(PHSceneDesc(), GRSceneDesc());	// Scene�̍쐬
	GetSdk()->GetScene()->GetPHScene()->SetTimeStep(0.02);	// ���݂̐ݒ�

	/// �`�惂�[�h�̐ݒ�
	SetGRAdaptee(TypeGLUT);									// GLUT���[�h�ɐݒ�
	GetGRAdaptee()->Init(argc, argv);						// Sdk�̍쐬

	/// �`��Window�̍쐬�C������
	FWWinDesc windowDesc;									// GL�̃E�B���h�E�f�B�X�N���v�^
	windowDesc.title = "Springhead2";						// �E�B���h�E�̃^�C�g��
	CreateWin(windowDesc);									// �E�B���h�E�̍쐬
	InitWindow();											// �E�B���h�E�̏�����
	InitCameraView();										// �J�����r���[�̏�����

	/// HumanInterface�̏�����
	InitHumanInterface();

	/// InteractScene�̍쐬
	FWInteractSceneDesc desc;
	desc.fwScene = GetSdk()->GetScene();					// fwScene�ɑ΂���interactscene�����
	desc.mode = LOCAL_DYNAMICS;								// humaninterface�̃����_�����O���[�h�̐ݒ�
	desc.hdt = 0.001;										// �}���`���[�g�̏ꍇ�̍X�V[s]
	CreateINScene(desc);									// interactScene�̍쐬

	/// �����V�~�����[�V�������鍄�̂��쐬
	BuildScene();
	BuildPointer();

	/// �^�C�}�̍쐬�C�ݒ�
	FWTimer* timer = CreateTimer(MMTimer);
	timer->SetInterval(1);
	timer->SetResolution(1);
	FWTimer* timer2 = CreateTimer(GLUTTimer);

}

void FWAppHaptic::InitCameraView(){
	std::istringstream issView(
		"((0.9996 0.0107463 -0.0261432 -0.389004)"
		"(-6.55577e-010 0.924909 0.380188 5.65711)"
		"(0.0282657 -0.380037 0.92454 13.7569)"
		"(     0      0      0      1))"
	);
	issView >> cameraInfo.view;
}

void FWAppHaptic::InitHumanInterface(){
	/// HISdk�̍쐬
	CreateHISdk();

	DRUsb20SimpleDesc usbSimpleDesc;
	GetHISdk()->AddRealDevice(DRUsb20SimpleIf::GetIfInfoStatic(), &usbSimpleDesc);
	DRUsb20Sh4Desc usb20Sh4Desc;
	for(int i=0; i<10; ++i){
		usb20Sh4Desc.number = i;
		GetHISdk()->AddRealDevice(DRUsb20Sh4If::GetIfInfoStatic(), &usb20Sh4Desc);
	}
	GetHISdk()->AddRealDevice(DRKeyMouseWin32If::GetIfInfoStatic());
	GetHISdk()->Init();
	//GetHISdk()->Print(DSTR);

	/// SPIDARG6��2��g���ꍇ
	UTRef<HISpidarGIf> spg[2];
	for(size_t i = 0; i < 2; i++){
		spg[i] = GetHISdk()->CreateHumanInterface(HISpidarGIf::GetIfInfoStatic())->Cast();
		if(i == 0) spg[i]->Init(&HISpidarGDesc("SpidarG6X3R"));
		if(i == 1) spg[i]->Init(&HISpidarGDesc("SpidarG6X3L"));
		AddHI(spg[i]);
	}
}

void FWAppHaptic::Reset(){
	ReleaseAllTimer();
	GetSdk()->Clear();
	INClear();
	GetSdk()->CreateScene(PHSceneDesc(), GRSceneDesc());	// Scene�̍쐬
	GetSdk()->GetScene()->GetPHScene()->SetTimeStep(0.02);	// ���݂̐ݒ�
	FWInteractSceneDesc desc;
	desc.fwScene = GetSdk()->GetScene();					// fwScene�ɑ΂���interactscene�����
	desc.mode = LOCAL_DYNAMICS;								// humaninterface�̃����_�����O���[�h�̐ݒ�
	desc.hdt = 0.001;										// �}���`���[�g�̏ꍇ�̍X�V[s]
	CreateINScene(desc);									// interactScene�̍쐬
	BuildScene();
	BuildPointer();
	GetCurrentWin()->SetScene(GetSdk()->GetScene());
	CreateAllTimer();
}

void FWAppHaptic::Start(){
	StartMainLoop();
}

void FWAppHaptic::TimerFunc(int id){	
	switch(id){
		case 0:
			((FWAppHaptic*)instance)->GetINScene()->CallBackHapticLoop();
			GetGRAdaptee()->PostRedisplay();
			break;
		case 1:
			/// �V�~�����[�V������i�߂�(interactscene������ꍇ�͂��������Ă�)
			if(bStep) 	FWAppHaptic::instance->GetINScene()->Step();
			else if (bOneStep){
					FWAppHaptic::instance->GetINScene()->Step();
					bOneStep = false;
			}
			break;
	}
}
void FWAppHaptic::IdleFunc(){
	
	glutPostRedisplay();
}

void FWAppHaptic::Display(){
	/// �`�惂�[�h�̐ݒ�
	GetSdk()->SetDebugMode(true);
	GRDebugRenderIf* render = GetCurrentWin()->render->Cast();
	render->SetRenderMode(true, false);
	render->EnableRenderAxis(bDrawInfo);
	render->EnableRenderForce(bDrawInfo);
	render->EnableRenderContact(bDrawInfo);

	/// �J�������W�̎w��
	GRCameraIf* cam = GetCurrentWin()->scene->GetGRScene()->GetCamera();
	if (cam && cam->GetFrame()){
		cam->GetFrame()->SetTransform(cameraInfo.view);
	}else{
		GetCurrentWin()->render->SetViewMatrix(cameraInfo.view.inv());
	}

	/// �`��̎��s
	if(!GetCurrentWin()) return;
	GetSdk()->SwitchScene(GetCurrentWin()->GetScene());
	GetSdk()->SwitchRender(GetCurrentWin()->GetRender());
	GetSdk()->Draw();
	
	DisplayContactPlane();
	DisplayLineToNearestPoint();

	glutSwapBuffers();
}

void FWAppHaptic::BuildScene(){
	PHSceneIf* phscene = GetSdk()->GetScene()->GetPHScene();
	PHSolidDesc desc;
	CDBoxDesc bd;

	/// ��(�����@���ɏ]��Ȃ��C�^�����ω����Ȃ�)
	{
		/// ����(soFloor)�̍쐬
		desc.mass = 1e20f;
		desc.inertia *= 1e30f;
		PHSolidIf* soFloor = phscene->CreateSolid(desc);		// ���̂�desc�Ɋ�Â��č쐬
		soFloor->SetDynamical(false);
		soFloor->SetGravity(false);
		/// �`��(shapeFloor)�̍쐬
		bd.boxsize = Vec3f(50, 10, 50);
		CDShapeIf* shapeFloor = GetSdk()->GetPHSdk()->CreateShape(bd);
		/// ���̂Ɍ`���t������
		soFloor->AddShape(shapeFloor);
		soFloor->SetFramePosition(Vec3d(0, -10, 0));
	}
}
void FWAppHaptic::BuildPointer(){
	PHSceneIf* phscene = GetSdk()->GetScene()->GetPHScene();
	PHSolidDesc desc;
	/// �|�C���^
	for(int i= 0; i < 2; i++){
		PHSolidIf* soPointer = phscene->CreateSolid(desc);
		CDSphereDesc sd;
		sd.radius = 0.5;//1.0;
		CDSphereIf* shapePointer = DCAST(CDSphereIf,  GetSdk()->GetPHSdk()->CreateShape(sd));
		soPointer->AddShape(shapePointer);
		soPointer->SetDynamical(false);
		soPointer->GetShape(0)->SetStaticFriction(1.0);
		soPointer->GetShape(0)->SetDynamicFriction(1.0);
		GetSdk()->GetScene()->GetPHScene()->SetContactMode(soPointer, PHSceneDesc::MODE_NONE);
		FWInteractPointerDesc idesc;			// interactpointer�̃f�B�X�N���v�^
		idesc.pointerSolid = soPointer;			// soPointer��ݒ�
		idesc.humanInterface = GetHI(i);		// humaninterface��ݒ�
		idesc.springK = 0.8;						// haptic rendering�̃o�l�W��
		idesc.damperD = 0.01;					// haptic rendering�̃_���p�W��
		idesc.posScale = 300;					// soPointer�̉���̐ݒ�(�`�{)
		idesc.localRange = 1.0;					// LocalDynamics���g���ꍇ�̋ߖT�͈�
		//if(i==0) idesc.defaultPosition =Posed(1,0,0,0,5,0,0);	// �����ʒu�̐ݒ�
		//if(i==1) idesc.defaultPosition =Posed(1,0,0,0,-5,0,0);
		GetINScene()->CreateINPointer(idesc);	// interactpointer�̍쐬
	}
}

void FWAppHaptic::DisplayContactPlane(){
	FWInteractScene* inScene = GetINScene()->Cast();
	int N = inScene->NINSolids();
	for(int i = 0; i <  N; i++){
		FWInteractSolid* inSolid = inScene->GetINSolid(i);
		if(!inSolid->bSim) continue;
		for(int j = 0; j < inScene->NINPointers(); j++){
			FWInteractPointer* inPointer = inScene->GetINPointer(j)->Cast();
			FWInteractInfo* inInfo = &inPointer->interactInfo[i];
			if(!inInfo->flag.blocal) continue;
			Vec3d pPoint = inPointer->pointerSolid->GetPose() * inInfo->neighborInfo.pointer_point;
			Vec3d cPoint = inSolid->sceneSolid->GetPose() * inInfo->neighborInfo.closest_point;
			Vec3d normal = inInfo->neighborInfo.face_normal;
			Vec3d v1(0,1,0);

			v1 +=  Vec3d(0, 0, 0.5) - Vec3d(0, 0, 0.5)*normal*normal;
			v1 -= v1*normal * normal;
			v1.unitize();
			Vec3d v2 = normal ^ v1;

			Vec4f moon(1.0, 1.0, 0.8, 0.3);
			GRDebugRenderIf* render = GetCurrentWin()->render->Cast();
			render->SetMaterial( GRMaterialDesc(moon) );
			render->PushModelMatrix();
			Vec3d offset = 0.02 * normal;
			render->SetLighting( false );
			render->SetAlphaTest(true);
			render->SetAlphaMode(render->BF_SRCALPHA, render->BF_ONE);
			cPoint += offset/2;
			glBegin(GL_QUADS);
				/// �ڐG�ʒ��	
				glVertex3d(cPoint[0] + v1[0] + v2[0], cPoint[1] + v1[1] + v2[1], cPoint[2] + v1[2] + v2[2]);
				glVertex3d(cPoint[0] - v1[0] + v2[0], cPoint[1] - v1[1] + v2[1], cPoint[2] - v1[2] + v2[2]);
				glVertex3d(cPoint[0] - v1[0] - v2[0], cPoint[1] - v1[1] - v2[1], cPoint[2] - v1[2] - v2[2]);
				glVertex3d(cPoint[0] + v1[0] - v2[0], cPoint[1] + v1[1] - v2[1], cPoint[2] + v1[2] - v2[2]);
				/// ����1
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
				/// ����2
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
				/// ����3
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
				/// ����4
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
				/// �ڐG�ʏ��
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
}

void FWAppHaptic::DisplayLineToNearestPoint(){
	FWInteractScene* inScene = DCAST(FWInteractScene, GetINScene());
	int N = inScene->NINSolids();
	GLfloat moon[]={0.8,0.8,0.8};
	for(int i = 0; i <  N; i++){
		FWInteractSolid* inSolid = inScene->GetINSolid(i);
		if(!inSolid->bSim) continue;
		for(int j = 0; j < inScene->NINPointers(); j++){
			FWInteractPointer* inPointer = inScene->GetINPointer(j)->Cast();
			FWInteractInfo* inInfo = &inPointer->interactInfo[i];
			if(!inInfo->flag.blocal) continue;
			Vec3d pPoint = inPointer->pointerSolid->GetPose() * inInfo->neighborInfo.pointer_point;
			Vec3d cPoint = inSolid->sceneSolid->GetPose() * inInfo->neighborInfo.closest_point;
			Vec3d normal = inInfo->neighborInfo.face_normal;
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
}

void FWAppHaptic::Keyboard(int key, int x, int y){
	//Solid��ǉ�����ꍇ�CObjectStatesIf�̃����[�X���K�v
	BeginKeyboard();

		switch (key) {
		case  27: //ESC
		case 'q':
			
			exit(0);
			break;
		case 'r':
			Reset();
			break;
		case 'd':
			bDrawInfo = !bDrawInfo;
			break;
		case 's':
			bStep = false;
			bOneStep = true;
			DSTR << "Stepwise Execution" << endl;
			break;
		case 'a':
			bStep = true;
			DSTR << "Play" << endl;
			break;
		case 'c':
			{
				ReleaseAllTimer();
				for(int i = 0; i < GetINScene()->NINPointers(); i++){
					GetINScene()->GetINPointer(i)->Calibration();
				}
				CreateAllTimer();
			}
			break;
		case 'f':
			{
				static bool bf = false;
				bf = !bf;
				for(int i = 0; i < GetINScene()->NINPointers(); i++){
					GetINScene()->GetINPointer(i)->EnableForce(bf);
					if(!bf){
						HIBaseIf* hib = GetINScene()->GetINPointer(i)->GetHI();
						if(DCAST(HIForceInterface6DIf, hib)){
							HIForceInterface6DIf* hif = hib->Cast();
							hif->SetForce(Vec3d(), Vec3d());
						}
						if(DCAST(HIForceInterface3DIf, hib)){
							HIForceInterface3DIf* hif = hib->Cast();
							hif->SetForce(Vec3d());
						}
					}
				}
				if(bf){
					DSTR << "Enable Force Feedback" << std::endl;
				}else{
					DSTR << "Disable Force Feedback" << std::endl;
				}
			}
			break;
		case 'o':
			{
				static bool bv = false;
				bv = !bv;
				for(int i = 0; i < GetINScene()->NINPointers(); i++){
					GetINScene()->GetINPointer(i)->EnableVibration(bv);
				}
				if(bv){
					DSTR << "Enable Vibration Feedback" << std::endl;
				}else{
					DSTR << "Disable Vibration Feedback" << std::endl;
				}
			}
			break;
		case 'j':
			{
					GetINScene()->SetHMode(PENALTY);
					DSTR << "PENALTY MODE" << std::endl;
					break;
			}
		case 'k':
			{
					GetINScene()->SetHMode(PROXY);
					DSTR << "PROXY MODE" << std::endl;
					break;
			}
		case 'l':
			{
					GetINScene()->SetHMode(PROXYSIMULATION);
					DSTR << "PROXYSIMULATION MODE" << std::endl;
					break;
			}
		default:
			break;
	}
		EndKeyboard();
}
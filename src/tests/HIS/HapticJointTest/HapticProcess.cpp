#include "HapticProcess.h"
#include "BoxStack.h"
#include <iostream>
#include <sstream>
#include <cmath>
#include <ctime>
#include <string>
#include <GL/glut.h>
#include <vector>
#include <windows.h>
#include <fstream>

bool vhaptic = false;
bool bproxy = false;

HapticProcess hprocess;	

HapticProcess::HapticProcess(){
	dt = 0.001f;
	K = 30;
	D = 0.001;		// �_���p���C0�ɂ����珰�̂����������Ȃ��Ȃ����D
	posScale = 100;
	bDisplayforce = false;
	bInter = true;
	hpointer.SetDynamical(false);
	hpointer.SetFrozen(true);
	stepcount = 1;
	countmax = 100;
};

void HapticProcess::Init(){
	InitDevice();
}

void HapticProcess::InitDevice(){
	// �f�o�C�X�}�l�[�W���̏�����
	devMan.RPool().Register(new DRUsb20Sh4(0));	    // USB2.0�ŃR���g���[�� 8���[�^
	devMan.RPool().Register(new DRUsb20Sh4(1));	    // USB2.0�ŃR���g���[�� 8���[�^
	devMan.Init();														// �f�o�C�X�̏�����
	DSTR << devMan;												// �������̌��ʂ�\��

	// SPIDAR�̏�����
	spidarG6.Init(devMan, false);
	for(int i = 0; i < 8; ++i){
		spidarG6.motor[i].maxForce = 5.0f;
	}
	spidarG6.Calib();
}

void HapticProcess::Step(){
	UpdateSpidar();
	HapticRendering();
	LocalDynamics();
}

void HapticProcess::UpdateSpidar(){
	spidarG6.Update(dt);
	Vec3f pos = spidarG6.GetPos();
	pos.y += 0.0;												// ���ɂ߂荞�܂Ȃ��悤�ɐݒ�
	hpointer.SetFramePosition(pos * posScale);
	hpointer.SetOrientation(spidarG6.GetOri());
	hpointer.SetVelocity(spidarG6.GetVel() * posScale);
	hpointer.SetAngularVelocity(spidarG6.GetAngVel());
}

void HapticProcess::HapticRendering(){
	static double vibT = 0;
	static bool vibFlag = false;
	Vec3d vibV = spidarG6.GetVel();
	static Vec3d vibVo = vibV;
	double vibforce = 0;
	static Vec3d proxy[100];

	displayforce = Vec3d(0.0, 0.0, 0.0);								
	bool noContact = true;
	if(bstack.grabObjectFlag==true){
		displayforce = -bstack.grabForce*bstack.grabForceRate;			// ���[�U�ւ̒񎦗�
	}
	bstack.GrabSpring(dt);

	for(unsigned i = 0; i < neighborObjects.size(); i++){
		if(!neighborObjects[i].blocal) continue;
		Vec3d cPoint = neighborObjects[i].phSolid.GetPose() * neighborObjects[i].closestPoint;			// ���̂̋ߖT�_�̃��[���h���W�n
		Vec3d pPoint = hpointer.GetPose() * neighborObjects[i].pointerPoint;									// �͊o�|�C���^�̋ߖT�_�̃��[���h���W�n
		Vec3d force_dir = pPoint - cPoint;
		Vec3d interpolation_normal;																								// �񎦗͌v�Z�ɂ��悤����@���i�O��̖@���Ƃ̊Ԃ��Ԃ���j
		if(bInter){
			// ���̖̂ʂ̖@�����
			// �O��̖@���ƌ��݂̖@���̊Ԃ��Ԃ��Ȃ���X�V
			double synccount = bstack.dt / hprocess.dt;		// �v���Z�X�̍��ݎ��Ԃ̔�
			interpolation_normal = (stepcount * neighborObjects[i].face_normal + ((double)synccount - stepcount) * neighborObjects[i].last_face_normal) / (double)synccount;															
			if(stepcount > synccount)		interpolation_normal = neighborObjects[i].face_normal;
		}else{
			// ���݂̖@�����g��
			interpolation_normal = neighborObjects[i].face_normal;
		}

		float	f = force_dir * interpolation_normal;								// ���̖̂ʂ̖@���Ɠ��ς��Ƃ�
		if(f < 0.0){																			// ���ς����Ȃ�͂��v�Z
			Vec3d ortho = f * interpolation_normal;								// �ߖT�_����͊o�|�C���^�ւ̃x�N�g���̖ʂ̖@���ւ̐��ˉe
			Vec3d dv = neighborObjects[i].phSolid.GetPointVelocity(cPoint) - hpointer.GetPointVelocity(pPoint);
			Vec3d dvortho = dv.norm() * interpolation_normal;
			K = neighborObjects[i].phSolid.GetShape(0)->GetSpringK();			// ���̂��Ƃɂ΂ˌW����ς���
			D = neighborObjects[i].phSolid.GetShape(0)->GetDamperD();			// ���̂��ƂɃ_���p�W����ς���
			static Vec3d addforce = Vec3d(0,0,0);
			if(!bproxy){
				addforce = -K * ortho + D * dvortho;// * ortho.norm();						// �񎦗͌v�Z (*�_���p�̍���ortho�̃m�����������Ă݂�)
			}else{
				addforce = -K * (pPoint - (proxy[i]+neighborObjects[i].phSolid.GetCenterPosition())) + D * dvortho;	// �񎦗͌v�Z(proxy)
			}
//			Vec3d addforce = -K * ortho + D * dvortho;// * ortho.norm();						// �񎦗͌v�Z (*�_���p�̍���ortho�̃m�����������Ă݂�)
			
			if(!vibFlag){
				vibT = 0;
				vibVo = vibV;
			}
			vibFlag = true;
#if 0
			if(vhaptic){
				double vibA = -200;
				double vibB = 120;
				double vibW = 300;
				vibforce = vibA * (vibVo * addforce.unit()) * exp(-vibB * vibT) * sin(2 * M_PI * vibW * vibT);		//�U���v�Z
			}
#else
			if(vhaptic){
				double vibA = neighborObjects[i].phSolid.GetShape(0)->GetVibA();
				double vibB = neighborObjects[i].phSolid.GetShape(0)->GetVibB();
				double vibW = neighborObjects[i].phSolid.GetShape(0)->GetVibW();
				vibforce = vibA * (vibVo * addforce.unit()) * exp(-vibB * vibT) * sin(2 * M_PI * vibW * vibT);		//�U���v�Z
			}			
#endif
			// proxy�@�ł̖��C�̌v�Z
			Vec3d posVec = pPoint - (proxy[i] + neighborObjects[i].phSolid.GetCenterPosition());
			double posDot = dot(neighborObjects[i].face_normal,posVec);
			Vec3d tVec = posDot * neighborObjects[i].face_normal;
			Vec3d tanjent = posVec - tVec;
			double mu0 = neighborObjects[i].phSolidIf->GetShape(0)->GetStaticFriction();
			double mu1 = neighborObjects[i].phSolidIf->GetShape(0)->GetDynamicFriction();
			if(tanjent.norm() > abs(mu0 * posDot)){
				proxy[i] += (tanjent.norm() - abs(mu1 * posDot)) * tanjent.unit();
//				proxyPos += (tanjent.norm() - abs(1.0 * posDot)) * tanjent.unit();
			}
			if(bstack.grabObjectFlag==false){
			displayforce += addforce + (vibforce * addforce.unit());			// ���[�U�ւ̒񎦗�		
			neighborObjects[i].phSolid.AddForce(-addforce, cPoint);			// �v�Z�����͂����̂ɉ�����
			neighborObjects[i].test_force_norm = addforce.norm();
			noContact = false;
			}else{
				//displayforce = bstack.grabForce;			// ���[�U�ւ̒񎦗�
			}

		}else{
				proxy[i] = pPoint - neighborObjects[i].phSolid.GetCenterPosition();
		}
	}
	if (noContact) vibFlag = false;

	vibT += dt;
	if(bDisplayforce) spidarG6.SetForce(displayforce);								// �͊o��
}

void HapticProcess::LocalDynamics(){
	for(unsigned i = 0; i < neighborObjects.size(); i++){
		if(!neighborObjects[i].blocal) continue;
		SpatialVector vel;																				// ���̂̑��x�i���[���h���W�n�j
		vel.v() = neighborObjects[i].phSolid.GetVelocity();
		vel.w() = neighborObjects[i].phSolid.GetAngularVelocity();
		if(stepcount == 1) vel += (neighborObjects[i].curb - neighborObjects[i].lastb) *  bstack.dt;		// �Փ˂̉e���𔽉f
		vel += (neighborObjects[i].A * neighborObjects[i].phSolid.nextForce + neighborObjects[i].b) * dt;	// �͊o�|�C���^����̗͂ɂ�鑬�x�ω�
		neighborObjects[i].phSolid.SetVelocity(vel.v());																		
		neighborObjects[i].phSolid.SetAngularVelocity(vel.w());
		neighborObjects[i].phSolid.SetCenterPosition(neighborObjects[i].phSolid.GetCenterPosition() + vel.v() * dt);
		neighborObjects[i].phSolid.SetOrientation(( Quaterniond::Rot(vel.w() * dt) * neighborObjects[i].phSolid.GetOrientation()).unit());
 		neighborObjects[i].phSolid.SetUpdated(true);
		neighborObjects[i].phSolid.Step();
	}
}

void HapticProcess::Keyboard(unsigned char key){
	switch(key){
		case 'f':
			if(bDisplayforce){
				bDisplayforce = false;
				spidarG6.SetForce(Vec3d(0, 0, 0));
				DSTR << "Force OFF" << endl;
			}else{
				bDisplayforce = true;
				spidarG6.SetForce(Vec3d(0, 0, 0));
				DSTR << "Force ON" << endl;
			}
			break;
			case 'p':
			if(bproxy){
				bproxy = false;
				DSTR << "proxy mode stop" << endl;
			}else{
				bproxy = true;
				DSTR << "proxy mode stert" << endl;
			}
			break;
		case 'o':
			if(vhaptic){
				vhaptic = false;
				DSTR << "Vibration Disconnect" << endl;
			}else{
				vhaptic = true;
				DSTR << "Vibration Connect" << endl;
			}
			break;
		case 'c':
			spidarG6.SetForce(Vec3d(0, 0, 0));
			spidarG6.Calib();
			DSTR << "Calibration" << endl;
			break;
		case 'i':
			if(bInter){
				bInter = false;
				DSTR << "Use Current Face_Normal" << endl;
			}else{
				bInter = true;
				DSTR << "Use Interpolate Face_Normal" << endl;
			}
			break;
		default:
			break;
	}
}
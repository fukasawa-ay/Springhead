/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#include "CRBallHumanBody.h"

#ifdef USE_HDRSTOP
#pragma hdrstop
#endif

namespace Spr{

//�R���X�g���N�^
CRBallHumanBodyDesc::CRBallHumanBodyDesc(){
	bodyMass = 1.0f;  /// �f�t�H���g������Ȃ̂͂ǂ����ς���(mitake)

	// �����Ɋւ���f�[�^
	double hBody			= 1.7219; // �ő�g��
	double hCervicale		= 1.4564; // ��ō�
	double hSubsternale		= 1.2253; // ����������
	double hWaist			= 1.0281; // ���͍�
	double hIliocristal		= 1.0142; // �����ō�
	double hOmphalion		= 1.0036; // �`��
	double hTrochanterion	= 0.8757; // �]�q��
	double hTibiale			= 0.4420; // �����㉏��
	double hCalf			= 0.3150; // ���ڍő�͍�

	// ���Ɋւ���f�[�^
	double wBideltoid		= 0.4562; // ����
	double wChest			= 0.2887; // �������a
	double wWaist			= 0.2611; // �������a
	double wBicristal		= 0.2722; // �����ŕ�
	double wBitrochanteric	= 0.3067; // ��]�q�ԕ�

	waistBreadth    = wBicristal;
	waistLength     = hOmphalion - (hIliocristal - (waistBreadth/2));
	abdomenBreadth  = wWaist;
	abdomenLength   = hSubsternale - hOmphalion;
	chestBreadth    = wChest;
	chestLength     = hCervicale - hSubsternale - (chestBreadth/2);

	bideltoidBreadth = wBideltoid;

	headBreadth = 0.1619; // 0.5699 / 3.1415;
	headHeight  = 0.2387;
	
	neckLength   = 1.5796 - 1.4564; // ���̍��� - ��ō�  // 1.7219 - 1.3882 - 0.2387;
	neckDiameter = 0.3563 / 3.1415;

	double occiputToNeckFront  = 0.1301;
	neckPosZ     = headBreadth/2 - (occiputToNeckFront - neckDiameter/2);
	neckPosY     = -neckDiameter/2;

	bigonialBreadth = 0.1117;

	upperArmLength   = 0.3406;
	upperArmDiameter = 0.2808 / 3.1415;

	lowerArmLength   = 0.2766;
	lowerArmDiameter = 0.1677 / 3.1415;

	handLength    = 0.1908;
	handBreadth   = 0.1018;
	handThickness = 0.0275;

	upperLegLength   = 0.4092;
	upperLegDiameter = 0.5289 / 3.1415; //  0.1682;
	interLegDistance = wBitrochanteric - upperLegDiameter;
	legPosY = -(hIliocristal - hTrochanterion - waistBreadth/2);

	lowerLegLength   = 0.3946;
	lowerLegDiameter = 0.1109;
	kneeDiameter     = 0.1076;
	ankleDiameter    = 0.2194 / 3.1415;
	calfDiameter     = 0.3623 / 3.1415;
	calfPosY         = hTibiale - hCalf;
	calfPosZ         = (calfDiameter/2 - kneeDiameter/2)*0.9;

	footLength       = 0.2544;
	footBreadth      = 0.0994;
	footThickness    = 0.0619;
	ankleToeDistance = 0.2010;

	vertexToEyeHeight     = 0.1255;
	occiputToEyeDistance  = 0.1688;
	eyeDiameter           = 0.0240;
	interpupillaryBreadth = 0.0619;

	springWaistAbdomen = -1;        damperWaistAbdomen = -1;
	springAbdomenChest = -1;        damperAbdomenChest = -1;
	springChestNeck    = -1;        damperChestNeck    = -1;
	springNeckHead     = -1;        damperNeckHead     = -1;
	springShoulder     = -1;        damperShoulder     = -1;
	springElbow        = -1;        damperElbow        = -1;
	springWrist        = -1;        damperWrist        = -1;
	springWaistLeg     = -1;        damperWaistLeg     = -1;
	springKnee         = -1;        damperKnee         = -1;
	springAnkle        = -1;        damperAnkle        = -1;
	springEye          = 100000.0;  damperEye          =  2000.0;

	shapeType = CRBallHumanBodyDesc::HST_ROUNDCONE;
}

// --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---
// CRBallHumanBody


// --- --- ---
void CRBallHumanBody::Init(){
	CRBody::Init();
}

// --- --- ---
void CRBallHumanBody::InitBody(){
	CreateWaist();
	CreateAbdomen();
	CreateChest();
}

void CRBallHumanBody::CreateWaist(){
	PHSolidDesc        solidDesc;
	Posed              pose1, pose2, pose3;

	// Solid
	solidDesc.mass     = 0.17 * bodyMass;
	solidDesc.inertia  = Matrix3d::Unit() * solidDesc.mass;
	solids[SO_WAIST]   = phScene->CreateSolid(solidDesc);
	CreateIKControlPoint(SO_WAIST);

	// Shape
	int sn=0;
	if (shapeType == CRBallHumanBodyDesc::HST_ROUNDCONE) {
		CDRoundConeDesc    rcDesc;

		// ���`��
		rcDesc.radius[0]   = abdomenBreadth/2;
		rcDesc.radius[1]   = waistBreadth/2;
		rcDesc.length      = waistLength;
		solids[SO_WAIST]->AddShape(phSdk->CreateShape(rcDesc));
		pose1=Posed(); pose1.Pos() = Vec3d(0,0,rcDesc.length/2);
		pose2=Posed(); pose2.Ori() = Quaterniond::Rot(Rad(-90),'x');
		solids[SO_WAIST]->SetShapePose(sn,pose2*pose1);
		sn++;

		// ���`��
		double cX = interLegDistance/2;
		double cY = -legPosY;
		double theta = atan(cX/cY);
		rcDesc.radius[0]   = waistBreadth/2;
		rcDesc.radius[1]   = upperLegDiameter/2;
		rcDesc.length      = sqrt(cX*cX + cY*cY);
		pose1=Posed(); pose1.Pos() = Vec3d(0,0,-rcDesc.length/2);
		pose2=Posed(); pose2.Ori() = Quaterniond::Rot(Rad(-90),'x');
		/// -- ����
		pose3=Posed(); pose3.Ori() = Quaterniond::Rot(-theta,'z');
		solids[SO_WAIST]->AddShape(phSdk->CreateShape(rcDesc));
		solids[SO_WAIST]->SetShapePose(sn,pose3*pose2*pose1);
		sn++;
		/// -- �E��
		pose3=Posed(); pose3.Ori() = Quaterniond::Rot(theta,'z');
		solids[SO_WAIST]->AddShape(phSdk->CreateShape(rcDesc));
		solids[SO_WAIST]->SetShapePose(sn,pose3*pose2*pose1);
		sn++;

	} else if (shapeType == CRBallHumanBodyDesc::HST_BOX) {
		CDBoxDesc        boxDesc;

		boxDesc.boxsize  = Vec3f(waistBreadth, waistLength+waistBreadth, waistBreadth);
		solids[SO_WAIST]->AddShape(phSdk->CreateShape(boxDesc));
		sn++;

	}

	solids[SO_WAIST]->SetFramePosition(Vec3f(0,0,0));
}

void CRBallHumanBody::CreateAbdomen(){
	PHSolidDesc        solidDesc;
	PHBallJointDesc    ballDesc;

	// Solid
	solidDesc.mass     = 0.028 * bodyMass;
	solidDesc.inertia  = Matrix3d::Unit() * solidDesc.mass;
	solids[SO_ABDOMEN] = phScene->CreateSolid(solidDesc);
	CreateIKControlPoint(SO_ABDOMEN);

	// Shape
	if (shapeType == CRBallHumanBodyDesc::HST_ROUNDCONE) {
		CDRoundConeDesc    rcDesc;
		rcDesc.radius[0]   = abdomenBreadth/2;
		rcDesc.radius[1]   = abdomenBreadth/2;
		rcDesc.length      = abdomenLength;
		solids[SO_ABDOMEN]->AddShape(phSdk->CreateShape(rcDesc));
		Posed pose=Posed(); pose.Ori() = Quaterniond::Rot(Rad(-90),'x');
		solids[SO_ABDOMEN]->SetShapePose(0,pose);

	} else if (shapeType == CRBallHumanBodyDesc::HST_BOX) {
		CDBoxDesc          boxDesc;
		boxDesc.boxsize    = Vec3f(abdomenBreadth, abdomenLength+abdomenBreadth, abdomenBreadth);
		solids[SO_ABDOMEN]->AddShape(phSdk->CreateShape(boxDesc));
		
	}

	// Joint -- [p]Waist-[c]Abdomen
	ballDesc                  = PHBallJointDesc();
	ballDesc.posePlug.Pos()   = Vec3d(0,  waistLength, 0);
	ballDesc.poseSocket.Pos() = Vec3d(0, -abdomenLength / 2.0, 0);
	ballDesc.limitDir         = Vec3d(0,0,1);
	ballDesc.limitTwist       = Vec2d(Rad(-45), Rad(45));
	ballDesc.limitSwing       = Vec2d(Rad(0), Rad(45));
	SetJointSpringDamper(ballDesc, springWaistAbdomen, damperWaistAbdomen, solids[SO_WAIST]->GetMass());
	joints[JO_WAIST_ABDOMEN] = CreateJoint(solids[SO_ABDOMEN], solids[SO_WAIST], ballDesc);
	CreateIKNode(JO_WAIST_ABDOMEN);

	phScene->SetContactMode(solids[SO_ABDOMEN], solids[SO_WAIST], PHSceneDesc::MODE_NONE);
}

void CRBallHumanBody::CreateChest(){
	PHSolidDesc        solidDesc;
	PHBallJointDesc    ballDesc;
	Posed              pose1,pose2,pose3;

	// Solid
	solidDesc.mass   = 0.252 * bodyMass;
	solidDesc.inertia  = Matrix3d::Unit() * solidDesc.mass;
	solids[SO_CHEST] = phScene->CreateSolid(solidDesc);
	CreateIKControlPoint(SO_CHEST);

	// Shape
	int sn = 0;
	if (shapeType == CRBallHumanBodyDesc::HST_ROUNDCONE) {
		CDRoundConeDesc    rcDesc;

		// ���S�`��t��
		rcDesc.radius[0] = neckDiameter/2;
		rcDesc.radius[1] = chestBreadth/2;
		rcDesc.length    = chestBreadth/2;
		solids[SO_CHEST]->AddShape(phSdk->CreateShape(rcDesc));
		pose1=Posed(); pose1.Pos() = Vec3d(0,0,rcDesc.length/2);
		pose2=Posed(); pose2.Ori() = Quaterniond::Rot(Rad(-90),'x');
		solids[SO_CHEST]->SetShapePose(sn,pose2*pose1);
		sn++;

		// ���S�`��
		rcDesc.radius[0] = chestBreadth/2;
		rcDesc.radius[1] = abdomenBreadth/2;
		rcDesc.length    = chestLength;
		solids[SO_CHEST]->AddShape(phSdk->CreateShape(rcDesc));
		pose1=Posed(); pose1.Pos() = Vec3d(0,0,-rcDesc.length/2);
		pose2=Posed(); pose2.Ori() = Quaterniond::Rot(Rad(-90),'x');
		solids[SO_CHEST]->SetShapePose(sn,pose2*pose1);
		sn++;

		// ���S�`��
		double sX = bideltoidBreadth/2 - upperArmDiameter/2;
		double sY = chestBreadth/2 - upperArmDiameter/2;
		double theta = atan(sX/sY);
		rcDesc.radius[0] = upperArmDiameter/2;
		rcDesc.radius[1] = chestBreadth/2;
		rcDesc.length    = sqrt(sX*sX + sY*sY);
		pose1=Posed(); pose1.Pos() = Vec3d(0,0,rcDesc.length/2);
		pose2=Posed(); pose2.Ori() = Quaterniond::Rot(Rad(-90),'x');
		/// -- ����
		pose3=Posed(); pose3.Ori() = Quaterniond::Rot(theta,'z');
		solids[SO_CHEST]->AddShape(phSdk->CreateShape(rcDesc));
		solids[SO_CHEST]->SetShapePose(sn,pose3*pose2*pose1);
		sn++;
		// -- �E��
		pose3=Posed(); pose3.Ori() = Quaterniond::Rot(-theta,'z');
		solids[SO_CHEST]->AddShape(phSdk->CreateShape(rcDesc));
		solids[SO_CHEST]->SetShapePose(sn,pose3*pose2*pose1);
		sn++;
	
	} else if (shapeType == CRBallHumanBodyDesc::HST_BOX) {
		CDBoxDesc          boxDesc;

		boxDesc.boxsize  = Vec3f(chestBreadth, chestLength+neckDiameter/2, chestBreadth);
		solids[SO_CHEST]->AddShape(phSdk->CreateShape(boxDesc));
		sn++;

	}

	// Joint -- [p]Abdomen-[c]Chest
	ballDesc                  = PHBallJointDesc();
	ballDesc.posePlug.Pos()   = Vec3d(0,  abdomenLength/2, 0);
	ballDesc.poseSocket.Pos() = Vec3d(0, -chestLength, 0);
	ballDesc.limitDir         = Vec3d(0,0,1);
	ballDesc.limitTwist       = Vec2d(Rad(-45), Rad(45));
	ballDesc.limitSwing       = Vec2d(Rad(0), Rad(45));
	SetJointSpringDamper(ballDesc, springAbdomenChest, damperAbdomenChest, solids[SO_ABDOMEN]->GetMass());
	joints[JO_ABDOMEN_CHEST]  = CreateJoint(solids[SO_CHEST], solids[SO_ABDOMEN], ballDesc);
	CreateIKNode(JO_ABDOMEN_CHEST);

	phScene->SetContactMode(solids[SO_CHEST], solids[SO_ABDOMEN], PHSceneDesc::MODE_NONE);
}

// --- --- ---
void CRBallHumanBody::InitHead(){
	CreateNeck();
	CreateHead();
}

void CRBallHumanBody::CreateNeck(){
	PHSolidDesc        solidDesc;
	PHBallJointDesc    ballDesc;

	// Solid
	solidDesc.mass  = 0.028 * bodyMass;
	solidDesc.inertia  = Matrix3d::Unit() * solidDesc.mass;
	solids[SO_NECK] = phScene->CreateSolid(solidDesc);
	CreateIKControlPoint(SO_NECK);

	// Shape
	int sn = 0;
	if (shapeType == CRBallHumanBodyDesc::HST_ROUNDCONE) {
		CDCapsuleDesc      capsuleDesc;

		capsuleDesc.radius = neckDiameter/2;
		capsuleDesc.length = neckLength;
		solids[SO_NECK]->AddShape(phSdk->CreateShape(capsuleDesc));
		Posed pose=Posed(); pose.Ori() = Quaterniond::Rot(Rad(90),'x');
		solids[SO_NECK]->SetShapePose(0,pose);
		sn++;

	} else if (shapeType == CRBallHumanBodyDesc::HST_BOX) {
		CDBoxDesc boxDesc;
		boxDesc.boxsize = Vec3d(neckDiameter, neckLength+neckDiameter, neckDiameter);
		solids[SO_NECK]->AddShape(phSdk->CreateShape(boxDesc));
		sn++;

	}

	// Joint -- [p]Chest-[c]Neck
	ballDesc                  = PHBallJointDesc();
	ballDesc.posePlug.Pos()   = Vec3d(0, chestBreadth / 2.0, 0);
	ballDesc.poseSocket.Pos() = Vec3d(0, -neckLength / 2.0, 0);
	ballDesc.limitDir         = Vec3d(0,0,1);
	ballDesc.limitTwist       = Vec2d(Rad(-45), Rad(45));
	ballDesc.limitSwing       = Vec2d(Rad(0), Rad(30));
	SetJointSpringDamper(ballDesc, springChestNeck, damperChestNeck, solids[SO_CHEST]->GetMass());
	joints[JO_CHEST_NECK] = CreateJoint(solids[SO_NECK], solids[SO_CHEST], ballDesc);
	CreateIKNode(JO_CHEST_NECK);

	phScene->SetContactMode(solids[SO_NECK], solids[SO_CHEST], PHSceneDesc::MODE_NONE);
}

void CRBallHumanBody::CreateHead(){
	PHSolidDesc       solidDesc;
	PHBallJointDesc   ballDesc;

	// Solid
	solidDesc.mass    = 0.07 * bodyMass;
	solidDesc.inertia  = Matrix3d::Unit() * solidDesc.mass;
	solids[SO_HEAD]   = phScene->CreateSolid(solidDesc);
	CreateIKControlPoint(SO_HEAD);

	// Shape
	int sn = 0;
	if (shapeType == CRBallHumanBodyDesc::HST_ROUNDCONE) {
		CDRoundConeDesc   rcDesc;

		rcDesc.radius[0] = headBreadth / 2.0;
		rcDesc.radius[1] = bigonialBreadth / 2.0;
		double Z = headBreadth/2 - bigonialBreadth/2 ;
		double Y = headHeight - headBreadth/2 - bigonialBreadth/2;
		double theta = atan(Y/Z);
		rcDesc.length = sqrt(Z*Z + Y*Y);
		solids[SO_HEAD]->AddShape(phSdk->CreateShape(rcDesc));
		Posed pose1=Posed(); pose1.Pos() = Vec3d(0,0,-rcDesc.length/2);
		Posed pose2=Posed(); pose2.Ori() = Quaterniond::Rot(-theta,'x');
		solids[SO_HEAD]->SetShapePose(0,pose2*pose1);
		sn++;

	} else if (shapeType == CRBallHumanBodyDesc::HST_BOX) {
		CDBoxDesc boxDesc;

		boxDesc.boxsize = Vec3d(headBreadth, headHeight, headBreadth);
		solids[SO_HEAD]->AddShape(phSdk->CreateShape(boxDesc));
		sn++;

	} 

	// Joint -- [p]Neck-[c]Head
	ballDesc                  = PHBallJointDesc();
	ballDesc.posePlug.Pos()   = Vec3d(0, neckLength / 2.0, 0);
	ballDesc.poseSocket.Pos() = Vec3d(0, neckPosY, neckPosZ);
	ballDesc.limitDir         = Vec3d(0,0,1);
	ballDesc.limitTwist       = Vec2d(Rad(-45), Rad(45));
	ballDesc.limitSwing       = Vec2d(Rad(0), Rad(20));
	SetJointSpringDamper(ballDesc, springNeckHead, damperNeckHead, solids[SO_NECK]->GetMass());
	joints[JO_NECK_HEAD] = CreateJoint(solids[SO_HEAD], solids[SO_NECK], ballDesc);
	CreateIKNode(JO_NECK_HEAD);

	phScene->SetContactMode(solids[SO_HEAD], solids[SO_NECK], PHSceneDesc::MODE_NONE);
}

// --- --- ---
void CRBallHumanBody::InitArms(){
	CreateUpperArm(LEFTPART);
	CreateLowerArm(LEFTPART);
	CreateHand(LEFTPART);

	CreateUpperArm(RIGHTPART);
	CreateLowerArm(RIGHTPART);
	CreateHand(RIGHTPART);
}

void CRBallHumanBody::CreateUpperArm(LREnum lr){
	PHSolidDesc        solidDesc;
	PHBallJointDesc    ballDesc;

	CRBallHumanBodyDesc::CRHumanSolids soNUpperArm;
	CRBallHumanBodyDesc::CRHumanJoints joNShoulder;
	if (lr==LEFTPART) {
		soNUpperArm   = SO_LEFT_UPPER_ARM;
		joNShoulder   = JO_LEFT_SHOULDER;
	} else {
		soNUpperArm   = SO_RIGHT_UPPER_ARM;
		joNShoulder   = JO_RIGHT_SHOULDER;
	}

	// Solid
	solidDesc.mass      = 0.04 * bodyMass;
	solidDesc.inertia   = Matrix3d::Unit() * solidDesc.mass;
	solids[soNUpperArm] = phScene->CreateSolid(solidDesc);
	CreateIKControlPoint(soNUpperArm);

	// Shape
	int sn = 0;
	if (shapeType == CRBallHumanBodyDesc::HST_ROUNDCONE) {
		CDRoundConeDesc    rcDesc;

		rcDesc.radius[0]    = upperArmDiameter / 2.0;
		rcDesc.radius[1]    = lowerArmDiameter / 2.0;
		rcDesc.length       = upperArmLength;
		solids[soNUpperArm]->AddShape(phSdk->CreateShape(rcDesc));
		Posed pose=Posed(); pose.Ori() = Quaterniond::Rot(Rad(-lr*90),'y');
		solids[soNUpperArm]->SetShapePose(0,pose);
		sn++;

	} else if (shapeType == CRBallHumanBodyDesc::HST_BOX) {
		CDBoxDesc boxDesc;

		boxDesc.boxsize = Vec3d(upperArmLength, upperArmDiameter, upperArmDiameter);
		solids[soNUpperArm]->AddShape(phSdk->CreateShape(boxDesc));
		sn++;

	}

	// Joint -- Shoulder ([p]Chest-[c]UpperArm)
	ballDesc                    = PHBallJointDesc();
	ballDesc.posePlug.Pos() = Vec3d(lr*(bideltoidBreadth/2.0 - upperArmDiameter/2.0), chestBreadth/2.0-upperArmDiameter/2.0, 0);
	ballDesc.poseSocket.Pos() = Vec3d(-lr*upperArmLength/2.0, 0, 0);
	SetJointSpringDamper(ballDesc, springShoulder, damperShoulder, solids[SO_CHEST]->GetMass());
	joints[joNShoulder]       = CreateJoint(solids[soNUpperArm], solids[SO_CHEST], ballDesc);
	CreateIKNode(joNShoulder);

	phScene->SetContactMode(solids[soNUpperArm], solids[SO_CHEST], PHSceneDesc::MODE_NONE);
}

void CRBallHumanBody::CreateLowerArm(LREnum lr){
	PHSolidDesc        solidDesc;
	PHHingeJointDesc   hingeDesc;

	CRBallHumanBodyDesc::CRHumanSolids soNUpperArm, soNLowerArm;
	CRBallHumanBodyDesc::CRHumanJoints joNElbow;
	if (lr==LEFTPART) {
		soNUpperArm = SO_LEFT_UPPER_ARM; soNLowerArm = SO_LEFT_LOWER_ARM;
		joNElbow = JO_LEFT_ELBOW;
	} else {
		soNUpperArm = SO_RIGHT_UPPER_ARM; soNLowerArm = SO_RIGHT_LOWER_ARM;
		joNElbow = JO_RIGHT_ELBOW;
	}

	// Solid
	solidDesc.mass      = 0.02 * bodyMass;
	solidDesc.inertia  = Matrix3d::Unit() * solidDesc.mass;
	solids[soNLowerArm] = phScene->CreateSolid(solidDesc);
	CreateIKControlPoint(soNLowerArm);

	// Shape
	int sn = 0;
	if (shapeType == CRBallHumanBodyDesc::HST_ROUNDCONE) {
		CDCapsuleDesc      capsuleDesc;

		capsuleDesc.radius  = lowerArmDiameter / 2.0;
		capsuleDesc.length  = lowerArmLength;
		solids[soNLowerArm]->AddShape(phSdk->CreateShape(capsuleDesc));
		Posed pose=Posed(); pose.Ori() = Quaterniond::Rot(Rad(-lr*90),'y');
		solids[soNLowerArm]->SetShapePose(0,pose);
		sn++;

	} else if (shapeType == CRBallHumanBodyDesc::HST_BOX) {
		CDBoxDesc boxDesc;

		boxDesc.boxsize = Vec3d(lowerArmLength, lowerArmDiameter, lowerArmDiameter);
		solids[soNLowerArm]->AddShape(phSdk->CreateShape(boxDesc));
		sn++;

	}

	// Joint -- Elbow ([p]UpperArm-[c]LowerArm)
	hingeDesc                  = PHHingeJointDesc();
	hingeDesc.posePlug.Pos()   = Vec3d(lr*upperArmLength/2.0, 0, 0);
	hingeDesc.posePlug.Ori()   = Quaterniond::Rot(Rad(lr*90), 'x');
	hingeDesc.poseSocket.Pos() = Vec3d(-lr*lowerArmLength/2.0, 0, 0);
	hingeDesc.poseSocket.Ori() = Quaterniond::Rot(Rad(lr*90), 'x');
	SetJointSpringDamper(hingeDesc, springElbow, damperElbow, solids[soNUpperArm]->GetMass());
	hingeDesc.origin           = Rad(0);
	joints[joNElbow] = CreateJoint(solids[soNLowerArm], solids[soNUpperArm], hingeDesc);
	CreateIKNode(joNElbow);

	phScene->SetContactMode(solids[soNLowerArm], solids[soNUpperArm], PHSceneDesc::MODE_NONE);
}

void CRBallHumanBody::CreateHand(LREnum lr){
	CDBoxDesc          boxDesc;
	PHSolidDesc        solidDesc;
	PHBallJointDesc    ballDesc;

	CRBallHumanBodyDesc::CRHumanSolids soNLowerArm, soNHand;
	CRBallHumanBodyDesc::CRHumanJoints joNWrist;
	if (lr==LEFTPART) {
		soNLowerArm = SO_LEFT_LOWER_ARM; soNHand = SO_LEFT_HAND;
		joNWrist = JO_LEFT_WRIST; 
	} else {
		soNLowerArm = SO_RIGHT_LOWER_ARM; soNHand = SO_RIGHT_HAND;
		joNWrist = JO_RIGHT_WRIST; 
	}

	// Solid
	solidDesc.mass     = 0.05 * bodyMass;
	solidDesc.inertia  = Matrix3d::Unit() * solidDesc.mass;
	solids[soNHand]    = phScene->CreateSolid(solidDesc);
	CreateIKControlPoint(soNHand);

	boxDesc.boxsize    = Vec3f(handLength, handThickness, handBreadth);
	solids[soNHand]->AddShape(phSdk->CreateShape(boxDesc));

	// Joint -- Wrist ([p]LowerArm-[c]Hand)
	ballDesc                  = PHBallJointDesc();
	ballDesc.posePlug.Pos()   = Vec3d(lr*lowerArmLength/2.0, 0, 0);
	ballDesc.poseSocket.Pos() = Vec3d(-lr*handLength/2.0, 0, 0);
	SetJointSpringDamper(ballDesc, springWrist, damperWrist, solids[soNLowerArm]->GetMass());
	joints[joNWrist]          = CreateJoint(solids[soNHand], solids[soNLowerArm], ballDesc);
	CreateIKNode(joNWrist);

	phScene->SetContactMode(solids[soNHand], solids[soNLowerArm], PHSceneDesc::MODE_NONE);
}

// --- --- ---
void CRBallHumanBody::InitEyes(){
	CreateEye(LEFTPART);
	CreateEye(RIGHTPART);
}

void CRBallHumanBody::CreateEye(LREnum lr){
	CDSphereDesc       sphereDesc;
	PHSolidDesc        solidDesc;
	PHBallJointDesc    ballDesc;

	CRBallHumanBodyDesc::CRHumanSolids soNEye;
	CRBallHumanBodyDesc::CRHumanJoints joNEye;
	if (lr==LEFTPART) {
		soNEye = SO_LEFT_EYE;
		joNEye = JO_LEFT_EYE;
	} else {
		soNEye = SO_RIGHT_EYE;
		joNEye = JO_RIGHT_EYE;
	}

	// Solid
	solidDesc.mass     = 0.001 * bodyMass;
	solidDesc.inertia  = Matrix3d::Unit() * 0.001;
	solids[soNEye]     = phScene->CreateSolid(solidDesc);
	CreateIKControlPoint(soNEye);

	sphereDesc.radius  = (float)(eyeDiameter/2.0);
	solids[soNEye]->AddShape(phSdk->CreateShape(sphereDesc));
	sphereDesc.radius  = (float)(eyeDiameter/4.0);
	solids[soNEye]->AddShape(phSdk->CreateShape(sphereDesc));
	Posed pose=Posed(); pose.Pos() = Vec3d(0,0,-eyeDiameter/3.0);
	solids[soNEye]->SetShapePose(1,pose);

	// Joint -- Eye ([p]Head-[c]Eye)
	ballDesc                  = PHBallJointDesc();
	ballDesc.posePlug.Pos()   = Vec3d(lr*interpupillaryBreadth/2.0, headHeight/2.0 - vertexToEyeHeight, -headBreadth/2.0);
	ballDesc.spring           = springEye;
	ballDesc.damper           = damperEye;
	joints[joNEye]            = CreateJoint(solids[soNEye], solids[SO_HEAD], ballDesc);

	phScene->SetContactMode(solids[soNEye], solids[SO_HEAD], PHSceneDesc::MODE_NONE);
}

// --- --- ---
void CRBallHumanBody::InitLegs(){
	CreateUpperLeg(LEFTPART);
	CreateLowerLeg(LEFTPART);
	CreateFoot(LEFTPART);

	CreateUpperLeg(RIGHTPART);
	CreateLowerLeg(RIGHTPART);
	CreateFoot(RIGHTPART);
}

void CRBallHumanBody::CreateUpperLeg(LREnum lr){
	PHSolidDesc        solidDesc;
	PHBallJointDesc    ballDesc;

	CRBallHumanBodyDesc::CRHumanSolids soNUpperLeg;
	CRBallHumanBodyDesc::CRHumanJoints joNWaistLeg;
	if (lr==LEFTPART) {
		soNUpperLeg = SO_LEFT_UPPER_LEG;
		joNWaistLeg = JO_LEFT_WAIST_LEG;
	} else {
		soNUpperLeg = SO_RIGHT_UPPER_LEG;
		joNWaistLeg = JO_RIGHT_WAIST_LEG;
	}

	// Solid
	solidDesc.mass      = 0.06 * bodyMass;
	solidDesc.inertia   = Matrix3d::Unit() * solidDesc.mass;
	solids[soNUpperLeg] = phScene->CreateSolid(solidDesc);
	CreateIKControlPoint(soNUpperLeg);

	// Shape
	int sn = 0;
	if (shapeType == CRBallHumanBodyDesc::HST_ROUNDCONE) {
		CDRoundConeDesc   rcDesc;

		rcDesc.radius[0]    = upperLegDiameter / 2.0;
		rcDesc.radius[1]    = kneeDiameter / 2.0;
		rcDesc.length       = upperLegLength;
		solids[soNUpperLeg]->AddShape(phSdk->CreateShape(rcDesc));
		Posed pose=Posed(); pose.Ori() = Quaterniond::Rot(Rad(-90),'x');
		solids[soNUpperLeg]->SetShapePose(0,pose);

	} else if (shapeType == CRBallHumanBodyDesc::HST_BOX) {
		CDBoxDesc boxDesc;

		boxDesc.boxsize = Vec3d(upperLegDiameter, upperLegLength, upperLegDiameter);
		solids[soNUpperLeg]->AddShape(phSdk->CreateShape(boxDesc));
		sn++;

	}

	// Joint -- WaistLeg ([p]Waist-[c]UpperLeg)
	ballDesc                  = PHBallJointDesc();
	ballDesc.posePlug.Pos()   = Vec3d(lr*interLegDistance/2.0, legPosY, 0);
	ballDesc.poseSocket.Pos() = Vec3d(0, upperLegLength / 2.0, 0.0);
	SetJointSpringDamper(ballDesc, springWaistLeg, damperWaistLeg, solids[SO_WAIST]->GetMass());
	joints[joNWaistLeg]       = CreateJoint(solids[soNUpperLeg], solids[SO_WAIST], ballDesc);
	CreateIKNode(joNWaistLeg);

	phScene->SetContactMode(solids[soNUpperLeg], solids[SO_WAIST], PHSceneDesc::MODE_NONE);
}

void CRBallHumanBody::CreateLowerLeg(LREnum lr){
	PHSolidDesc        solidDesc;
	PHHingeJointDesc   hingeDesc;
	Posed              pose1,pose2,pose3;
	double             theta;

	CRBallHumanBodyDesc::CRHumanSolids soNUpperLeg, soNLowerLeg;
	CRBallHumanBodyDesc::CRHumanJoints joNKnee;
	if (lr==LEFTPART) {
		soNUpperLeg = SO_LEFT_UPPER_LEG; soNLowerLeg = SO_LEFT_LOWER_LEG;
		joNKnee = JO_LEFT_KNEE;
	} else {
		soNUpperLeg = SO_RIGHT_UPPER_LEG; soNLowerLeg = SO_RIGHT_LOWER_LEG;
		joNKnee = JO_RIGHT_KNEE;
	}

	// Solid
	solidDesc.mass      = 0.05 * bodyMass;
	solidDesc.inertia   = Matrix3d::Unit() * solidDesc.mass;
	solids[soNLowerLeg] = phScene->CreateSolid(solidDesc);
	CreateIKControlPoint(soNLowerLeg);

	// Shape
	int sn = 0;
	if (shapeType == CRBallHumanBodyDesc::HST_ROUNDCONE) {
		CDRoundConeDesc   rcDesc;

		rcDesc.radius[0]    = kneeDiameter/2;
		rcDesc.radius[1]    = calfDiameter/2;
		rcDesc.length       = sqrt(calfPosY*calfPosY + calfPosZ*calfPosZ);
		theta               = atan(calfPosZ / calfPosY);
		pose1=Posed(); pose1.Pos() = Vec3d(0,0,-rcDesc.length/2);
		pose2=Posed(); pose2.Ori() = Quaterniond::Rot(Rad(-90)+theta,'x');
		pose3=Posed(); pose3.Pos() = Vec3d(0,lowerLegLength/2,0);
		solids[soNLowerLeg]->AddShape(phSdk->CreateShape(rcDesc));
		solids[soNLowerLeg]->SetShapePose(sn,pose3*pose2*pose1);
		sn++;

		double cY = lowerLegLength - calfPosY;
		rcDesc.radius[0]    = calfDiameter/2;
		rcDesc.radius[1]    = ankleDiameter/2;
		rcDesc.length       = sqrt(cY*cY + calfPosZ*calfPosZ);
		theta               = atan(calfPosZ / cY);
		pose1=Posed(); pose1.Pos() = Vec3d(0,0,rcDesc.length/2);
		pose2=Posed(); pose2.Ori() = Quaterniond::Rot(Rad(-90)-theta,'x');
		pose3=Posed(); pose3.Pos() = Vec3d(0,-lowerLegLength/2,0);
		solids[soNLowerLeg]->AddShape(phSdk->CreateShape(rcDesc));
		solids[soNLowerLeg]->SetShapePose(sn,pose3*pose2*pose1);
		sn++;

	} else if (shapeType == CRBallHumanBodyDesc::HST_BOX) {
		CDBoxDesc boxDesc;

		boxDesc.boxsize = Vec3d(calfDiameter, lowerLegLength, calfDiameter);
		solids[soNLowerLeg]->AddShape(phSdk->CreateShape(boxDesc));
		sn++;

	}

	// Joint -- Knee ([p]UpperLeg-[c]LowerLeg)
	hingeDesc                  = PHHingeJointDesc();
	hingeDesc.posePlug.Pos()   = Vec3d(0, -upperLegLength / 2.0, 0);
	hingeDesc.posePlug.Ori()   = Quaterniond::Rot(Rad(90), 'y');
	hingeDesc.poseSocket.Pos() = Vec3d(0, lowerLegLength / 2.0, 0);
	hingeDesc.poseSocket.Ori() = Quaterniond::Rot(Rad(90), 'y');
	SetJointSpringDamper(hingeDesc, springKnee, damperKnee, solids[soNUpperLeg]->GetMass());
	joints[joNKnee] = CreateJoint(solids[soNLowerLeg], solids[soNUpperLeg], hingeDesc);
	CreateIKNode(joNKnee);

	phScene->SetContactMode(solids[soNLowerLeg], solids[soNUpperLeg], PHSceneDesc::MODE_NONE);
}

void CRBallHumanBody::CreateFoot(LREnum lr){
	CDBoxDesc          boxDesc;
	PHSolidDesc        solidDesc;
	PHBallJointDesc    ballDesc;

	CRBallHumanBodyDesc::CRHumanSolids soNLowerLeg, soNFoot;
	CRBallHumanBodyDesc::CRHumanJoints joNAnkle;
	if (lr==LEFTPART) {
		soNLowerLeg = SO_LEFT_LOWER_LEG; soNFoot = SO_LEFT_FOOT;
		joNAnkle = JO_LEFT_ANKLE;
	} else {
		soNLowerLeg = SO_RIGHT_LOWER_LEG; soNFoot = SO_RIGHT_FOOT;
		joNAnkle = JO_RIGHT_ANKLE;
	}

	// Solid
	solidDesc.mass   = 0.01 * bodyMass;
	solidDesc.inertia  = Matrix3d::Unit() * solidDesc.mass;
	solids[soNFoot]  = phScene->CreateSolid(solidDesc);
	CreateIKControlPoint(soNFoot);

	boxDesc.boxsize  = Vec3f(footBreadth, footThickness, footLength);
	solids[soNFoot]->AddShape(phSdk->CreateShape(boxDesc));

	// Joint -- Ankle ([p]LowerLeg-[c]Foot)
	ballDesc                  = PHBallJointDesc();
	ballDesc.posePlug.Pos()   = Vec3d(0.0, -lowerLegLength / 2.0, 0);
	ballDesc.poseSocket.Pos() = Vec3d(0, footThickness / 2.0, (ankleToeDistance - footLength/2.0));
	SetJointSpringDamper(ballDesc, springAnkle, damperAnkle, solids[soNLowerLeg]->GetMass());
	joints[joNAnkle]          = CreateJoint(solids[soNFoot], solids[soNLowerLeg], ballDesc);
	CreateIKNode(joNAnkle);

	phScene->SetContactMode(solids[soNFoot], solids[soNLowerLeg], PHSceneDesc::MODE_NONE);
}

// --- --- ---
void CRBallHumanBody::InitContact(){
	// �����͋߂����đ��̑�������ł͏Փ˂��Ă��܂����߁D
	// �O�̂��ߍ��͎c���Ă��邪���̃R�[�h������Εs�v�ȋC������D(mitake)
	// phScene->SetContactMode(solids[SO_LEFT_UPPER_LEG], solids[SO_RIGHT_UPPER_LEG], PHSceneDesc::MODE_NONE);

	// �����ɑ����鍄�̓��m�̐ڐG��Off�i�܂����Ȃ����邩���H�Œ���̐ڐG�͎c�������i07/09/25, mitake�j�j
	/**/
	for (unsigned int i=0; i<solids.size(); ++i) {
		for (unsigned int j=0; j<solids.size(); ++j) {
			if (i!=j) {
				phScene->SetContactMode(solids[i], solids[j], PHSceneDesc::MODE_NONE);
			}
		}
	}
	/**/

	// �����ȊO�ɂ��ł�Body������΂���Body�ɑ����鍄�̂Ƃ�Contact���؂�
	/*/
	for (int i=0; i<creature->NBodies(); ++i) {
		CRBodyIf* body = creature->GetBody(i);
		if (DCAST(CRBallHumanBodyIf,body)!=(this->Cast())) {
			for (int s=0; s<body->NSolids(); ++s) {
				for (unsigned int j=0; j<solids.size(); ++j) {
					phScene->SetContactMode(body->GetSolid(s), solids[j], PHSceneDesc::MODE_NONE);
				}
			}
		}
	}
	/**/
}

void CRBallHumanBody::SetJointSpringDamper(PHBallJointDesc &ballDesc, double springOrig, double damperOrig, double actuatorMass){
	ballDesc.spring = 100000;
	ballDesc.damper =  12000;
	/*
	if (springOrig > 0 && damperOrig > 0) {
		ballDesc.spring = springOrig;
		ballDesc.damper = damperOrig;
	} else {
		double dt = phScene->GetTimeStep();
		ballDesc.spring = spring*2 * actuatorMass / (dt*dt);
		ballDesc.damper = damper   * actuatorMass /  dt;
	}
	*/
}

void CRBallHumanBody::SetJointSpringDamper(PHHingeJointDesc &hingeDesc, double springOrig, double damperOrig, double actuatorMass){
	hingeDesc.spring = 100000;
	hingeDesc.damper =  12000;
}

void CRBallHumanBody::CreateIKNode(int n) {
	if (DCAST(PHBallJointIf,joints[n])) {
		PHIKBallJointDesc descIKNode; descIKNode.joint  = joints[n]->Cast();
		ikNodes[n] = phScene->CreateIKNode(descIKNode);
	} else if (DCAST(PHHingeJointIf,joints[n])) {
		PHIKHingeJointDesc descIKNode; descIKNode.joint = joints[n]->Cast();
		ikNodes[n] = phScene->CreateIKNode(descIKNode);
	}
}

void CRBallHumanBody::CreateIKControlPoint(int n) {
	PHIKPosCtlDesc descIKCPPos;
	descIKCPPos.solid = solids[n];
	descIKCPPos.pos   = Vec3d(0,0,0);
	ikControlPoints[2*n] = phScene->CreateIKControlPoint(descIKCPPos);
	ikControlPoints[2*n]->Enable(false);

	PHIKOriCtlDesc descIKCPOri;
	descIKCPOri.solid = solids[n];
	ikControlPoints[2*n+1] = phScene->CreateIKControlPoint(descIKCPOri);
	ikControlPoints[2*n+1]->Enable(false);
}
}
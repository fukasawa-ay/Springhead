/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#ifndef SPR_CRBodyIf_H
#define SPR_CRBodyIf_H

#include <SprFoundation.h>
#include <SprPhysics.h>

namespace Spr{;

//@{

struct PHSolidIf;
struct PHJointIf;

struct CRBodyIf;
struct CRHingeHumanBodyIf;
struct CRFourLegsAnimalBodyIf;
struct CRIKControlIf;
struct CRIKMovableIf;
struct CRIKControlDesc;
struct CRIKMovableDesc;

// ------------------------------------------------------------------------------
/// クリーチャのボディモデルのインターフェイス
struct CRBodyIf : SceneObjectIf{
	SPR_IFDEF(CRBody);

	/** @brief 初期化を行う
	*/
	void Init();

	/** @brief 剛体の数を得る
	*/
	int NSolids();

	/** @brief i番目の剛体を得る
	*/
	PHSolidIf* GetSolid(int i);

	/** @brief 関節の数を得る
	*/
	int NJoints();

	/** @brief i番目の関節を得る
	*/
	PHJointIf* GetJoint(int i);

	/** @brief IK用の制御点を追加する
	*/
	CRIKControlIf* CreateIKControl(const IfInfo* ii, const CRIKControlDesc& desc);
	template <class T> CRIKControlIf* CreateIKControl(const T& desc){
		return CreateIKControl(T::GetIfInfo(), desc);
	}
	
	/** @brief IK用の可動物を追加する
	*/
	CRIKMovableIf* CreateIKMovable(const IfInfo* ii, const CRIKMovableDesc& desc);
	template <class T> CRIKMovableIf* CreateIKMovable(const T& desc){
		return CreateIKMovable(T::GetIfInfo(), desc);
	}

	/** @brief IKを計算する
	*/
	void CalcIK();

	/** @brief ボディの重心座標を得る
	*/
	Vec3d GetCenterOfMass();

	/** @brief ボディの質量を得る
	*/
	double GetSumOfMass();

	
};

/// クリーチャのボディモデルのデスクリプタ
struct CRBodyDesc{
	SPR_DESCDEF(CRBody);

	/// プラグ側を親剛体にするかソケット側を親剛体にするか
	//enum CRHumanJointOrder{
	enum CRCreatureJointOrder{
		SOCKET_PARENT,
		PLUG_PARENT,
	} jointOrder;

	CRBodyDesc(){
	}
};

// ------------------------------------------------------------------------------
/// ヒンジジョイント人体モデルのインターフェイス
struct CRHingeHumanBodyIf : CRBodyIf {
	SPR_IFDEF(CRHingeHumanBody);

	/** @brief 上半身の関節の堅さを変える
		@param stifness 堅さ：デフォルトの堅さに対する倍数で指定
	*/
	void SetUpperBodyStiffness(float stiffness);

	/** @brief 上半身のポーズを保持する
	*/
	void KeepUpperBodyPose();

	/** @brief 上半身のポーズを初期位置に戻す
	*/
	void ResetUpperBodyPose();
};

/// ヒンジジョイント人体モデルのデスクリプタ
struct CRHingeHumanBodyDesc : CRBodyDesc {
	SPR_DESCDEF(CRHingeHumanBody);

	enum CRHumanSolids{
		// Center
		SO_WAIST=0,
		SO_ABDOMEN, SO_CHEST, SO_NECK, SO_HEAD,

		// Right
		SO_RIGHT_UPPER_ARM, SO_RIGHT_LOWER_ARM, SO_RIGHT_HAND,
		SO_RIGHT_UPPER_LEG, SO_RIGHT_LOWER_LEG, SO_RIGHT_FOOT,
		SO_RIGHT_EYE,

		// Left
		SO_LEFT_UPPER_ARM, SO_LEFT_LOWER_ARM, SO_LEFT_HAND,
		SO_LEFT_UPPER_LEG, SO_LEFT_LOWER_LEG, SO_LEFT_FOOT,
		SO_LEFT_EYE,

		// 関節の中継ぎ用の形状を有しない剛体
		SO_CHEST_NECK_XZ, SO_CHEST_NECK_ZY,
		SO_NECK_HEAD_XZ,

		SO_RIGHT_SHOULDER_ZX, SO_RIGHT_SHOULDER_XY, 
		SO_RIGHT_WRIST_YX, SO_RIGHT_WRIST_XZ,
		SO_RIGHT_WAIST_LEG_ZX, SO_RIGHT_WAIST_LEG_XY, 
		SO_RIGHT_ANKLE_YX, SO_RIGHT_ANKLE_XZ,
		SO_RIGHT_EYE_YX,

		SO_LEFT_SHOULDER_ZX, SO_LEFT_SHOULDER_XY, 
		SO_LEFT_WRIST_YX, SO_LEFT_WRIST_XZ, 
		SO_LEFT_WAIST_LEG_ZX, SO_LEFT_WAIST_LEG_XY, 
		SO_LEFT_ANKLE_YX, SO_LEFT_ANKLE_XZ,
		SO_LEFT_EYE_YX,

		// 剛体の数
		SO_NSOLIDS
	};

	enum CRHumanJoints{
		// -- Center
		JO_WAIST_ABDOMEN=0, JO_ABDOMEN_CHEST,
		JO_CHEST_NECK_X, JO_CHEST_NECK_Z, JO_CHEST_NECK_Y,
		JO_NECK_HEAD_X, JO_NECK_HEAD_Z,

		// -- Right
		JO_RIGHT_SHOULDER_Z, JO_RIGHT_SHOULDER_X, JO_RIGHT_SHOULDER_Y,
		JO_RIGHT_ELBOW,
		JO_RIGHT_WRIST_Y, JO_RIGHT_WRIST_X, JO_RIGHT_WRIST_Z,

		JO_RIGHT_WAIST_LEG_Z, JO_RIGHT_WAIST_LEG_X, JO_RIGHT_WAIST_LEG_Y,
		JO_RIGHT_KNEE,
		JO_RIGHT_ANKLE_Y, JO_RIGHT_ANKLE_X, JO_RIGHT_ANKLE_Z,

		JO_RIGHT_EYE_Y, JO_RIGHT_EYE_X,

		// -- Left
		JO_LEFT_SHOULDER_Z, JO_LEFT_SHOULDER_X, JO_LEFT_SHOULDER_Y,
		JO_LEFT_ELBOW,
		JO_LEFT_WRIST_Y, JO_LEFT_WRIST_X, JO_LEFT_WRIST_Z,

		JO_LEFT_WAIST_LEG_Z, JO_LEFT_WAIST_LEG_X, JO_LEFT_WAIST_LEG_Y,
		JO_LEFT_KNEE,
		JO_LEFT_ANKLE_Y, JO_LEFT_ANKLE_X, JO_LEFT_ANKLE_Z,

		JO_LEFT_EYE_Y, JO_LEFT_EYE_X,

		// 関節の数
		JO_NJOINTS
	};

	/// 体重
	double bodyMass;

	/// サイズに関するパラメータ
	double waistHeight, waistBreadth, waistThickness;
	double abdomenHeight, abdomenBreadth, abdomenThickness;
	double chestHeight, chestBreadth, chestThickness;
	double neckLength, neckDiameter;
	double headDiameter;
	double upperArmLength, upperArmDiameter;
	double lowerArmLength, lowerArmDiameter;
	double handLength, handBreadth, handThickness;
	double upperLegLength, upperLegDiameter, interLegDistance;
	double lowerLegLength, lowerLegDiameter;
	double footLength, footBreadth, footThickness, ankleToeDistance;
	double vertexToEyeHeight, occiputToEyeDistance;
	double eyeDiameter, interpupillaryBreadth;

	/// 関節バネダンパ係数
	double spring, damper;

	/// 各関節のバネダンパ
	double springWaistAbdomen, damperWaistAbdomen;
	double springAbdomenChest, damperAbdomenChest;
	double springChestNeckX,   damperChestNeckX;
	double springChestNeckY,   damperChestNeckY;
	double springChestNeckZ,   damperChestNeckZ;
	double springNeckHeadX,    damperNeckHeadX;
	double springNeckHeadZ,    damperNeckHeadZ;
	double springShoulderZ,    damperShoulderZ;
	double springShoulderX,    damperShoulderX;
	double springShoulderY,    damperShoulderY;
	double springElbow,        damperElbow;
	double springWristY,       damperWristY;
	double springWristX,       damperWristX;
	double springWristZ,       damperWristZ;
	double springWaistLegZ,    damperWaistLegZ;
	double springWaistLegX,    damperWaistLegX;
	double springWaistLegY,    damperWaistLegY;
	double springKnee,         damperKnee;
	double springAnkleY,       damperAnkleY;
	double springAnkleX,       damperAnkleX;
	double springAnkleZ,       damperAnkleZ;
	double springEyeY,         damperEyeY;
	double springEyeX,         damperEyeX;

	/// 関節取り付け角度・位置
	Vec3d       posRightUpperArm;
	Quaterniond oriRightUpperArm;
	Quaterniond oriRightLowerArm;
	Quaterniond oriRightHand;

	/// 可動域制限
	Vec2d rangeWaistAbdomen;
	Vec2d rangeAbdomenChest;
	Vec2d rangeChestNeckX;
	Vec2d rangeChestNeckY;
	Vec2d rangeChestNeckZ;
	Vec2d rangeNeckHeadX;
	Vec2d rangeNeckHeadZ;
	Vec2d rangeShoulderZ;
	Vec2d rangeShoulderX;
	Vec2d rangeShoulderY;
	Vec2d rangeElbow;
	Vec2d rangeWristY;
	Vec2d rangeWristX;
	Vec2d rangeWristZ;
	Vec2d rangeWaistLegZ;
	Vec2d rangeWaistLegX;
	Vec2d rangeWaistLegY;
	Vec2d rangeKnee;
	Vec2d rangeAnkleY;
	Vec2d rangeAnkleX;
	Vec2d rangeAnkleZ;
	Vec2d rangeRightEyeY;
	Vec2d rangeEyeX;

	/// 裏オプション
	bool noLegs;

	CRHingeHumanBodyDesc();
};


// ------------------------------------------------------------------------------
/// ４足動物モデルのインターフェイス
struct CRFourLegsAnimalBodyIf : CRBodyIf {
	SPR_IFDEF(CRFourLegsAnimalBody);

	/** @brief 初期化を実行する
	*/
	void Init();
	
	/** @brief 状態の重心座標を返す
	*/
	Vec3d GetUpperCenterOfMass();

	/** @brief 剛体の数を返す
	*/
	int NSolids();

	/** @brief 関節の数を返す
	*/
	int NJoints();

	/** @brief ボールジョイントの数を返す
	*/
	int NBallJoints();

	/** @brief ヒンジジョイントの数を返す
	*/
	int NHingeJoints();
	/** @brief i番目の剛体の体積を返す
	*/
	double VSolid(int i);
	/** @brief 剛体の体積総和を返す
	*/
	double VSolids();
	/** @brief ボディの総質量を返す
	*/
	double	GetTotalMass();
	/** @brief ボディの総質量を設定する
	*/
	void	SetTotalMass(double value);
	/** @brief ボディの脚の長さを返す
	*/
	double GetLegLength(int i);

};

/// 4足動物モデルのデスクリプタ
struct CRFourLegsAnimalBodyDesc : CRBodyDesc {
	SPR_DESCDEF(CRFourLegsAnimalBody);

	enum CRAnimalSolids{
		// Center part of the solids
		SO_WAIST=0,
		SO_CHEST, SO_TAIL1, SO_TAIL2, SO_TAIL3,
		SO_NECK, SO_HEAD, 

		// -- Left part of the solids
		SO_LEFT_BREASTBONE, SO_LEFT_RADIUS, SO_LEFT_FRONT_CANNON_BONE, SO_LEFT_FRONT_TOE,
		SO_LEFT_FEMUR , SO_LEFT_TIBIA, SO_LEFT_REAR_CANNON_BONE, SO_LEFT_REAR_TOE,
		
		// -- Right part of the solids
		SO_RIGHT_BREASTBONE, SO_RIGHT_RADIUS, SO_RIGHT_FRONT_CANNON_BONE, SO_RIGHT_FRONT_TOE,
		SO_RIGHT_FEMUR , SO_RIGHT_TIBIA, SO_RIGHT_REAR_CANNON_BONE, SO_RIGHT_REAR_TOE,

		// -- The number of the solids
		SO_NSOLIDS
	};

	enum CRAnimalJoints{

		//////////////////////////////////////////////////////////////////
		//																//
		// < ボディの定義の順番 >										//
		// PHBallJoint⇒PHHingeJointにしていると遺伝子を組んだ時に		//
		// うまく一点交叉しなくなってしまうので混合で定義している		//
		//																//
		//////////////////////////////////////////////////////////////////

		// -- Center part of the joints
		JO_WAIST_CHEST=0,
		JO_CHEST_NECK,
		JO_NECK_HEAD,
		JO_WAIST_TAIL, JO_TAIL_12, JO_TAIL_23,

		// -- Left part of the joints
		JO_LEFT_SHOULDER, JO_LEFT_ELBOW, JO_LEFT_FRONT_KNEE, JO_LEFT_FRONT_ANKLE,
		JO_LEFT_HIP, JO_LEFT_STIFLE, JO_LEFT_REAR_KNEE, JO_LEFT_REAR_ANKLE,

		// -- Right part of the joints
		JO_RIGHT_SHOULDER,JO_RIGHT_ELBOW, JO_RIGHT_FRONT_KNEE, JO_RIGHT_FRONT_ANKLE,
		JO_RIGHT_HIP, JO_RIGHT_STIFLE, JO_RIGHT_REAR_KNEE, JO_RIGHT_REAR_ANKLE,

		// -- The number of the all joints (ball + hinge)
		JO_NJOINTS								//(nHingeJoints = nJoints - nBallJoints - 1)
	};

	enum CRAnimalLegs{
		LEG_RIGHT_FRONT=0, LEG_LEFT_FRONT,
		LEG_RIGHT_REAR, LEG_LEFT_REAR
	};

	int soNSolids;
	int joNBallJoints;
	int joNHingeJoints;
	int joNJoints;

	/// サイズに関するパラメータ
	double waistBreadth,	       waistHeight,			  waistThickness;
	double chestBreadth,	       chestHeight,			  chestThickness;
	double tailBreadth,		       tailHeight,		      tailThickness;
	double neckBreadth,		       neckHeight,			  neckThickness;
	double headBreadth,		       headHeight,			  headThickness;
	double breastboneBreadth,      breastboneHeight,	  breastboneThickness;
	double radiusBreadth,	       radiusHeight,		  radiusThickness;
	double frontCannonBoneBreadth, frontCannonBoneHeight, frontCannonBoneThickness;
	double frontToeBreadth,		   frontToeHeight,		  frontToeThickness;
	double femurBreadth,		   femurHeight,			  femurThickness;
	double tibiaBreadth,		   tibiaHeight,			  tibiaThickness;
	double rearCannonBoneBreadth,  rearCannonBoneHeight,  rearCannonBoneThickness;
	double rearToeBreadth,		   rearToeHeight,		  rearToeThickness;

	/// 各BallJointのバネダンパ
	double springWaistChest,   damperWaistChest;	//腰-胸
	double springWaistTail,	   damperWaistTail;		//腰-尾
	double springTail,		   damperTail;			//尾
	double springChestNeck,	   damperChestNeck;     //胸-首
	double springNeckHead,	   damperNeckHead;		//首-頭
	double springShoulder,	   damperShoulder;		//肩
	double springFrontAnkle,   damperFrontAnkle;	//かかと（前足）
	double springHip,		   damperHip;			//尻
	double springRearAnkle,    damperRearAnkle;		//かかと（後足）
	
	// 各HingeJointのバネダンパ
	double springElbow,		   damperElbow;			//肘（前足）
	double springFrontKnee,	   damperFrontKnee;		//膝（前足）
	double springStifle,	   damperStifle;		//肘？（後足）
	double springRearKnee,	   damperRearKnee;		//膝（後足）
	
	/// HingeJoint可動域制限
	Vec2d  rangeElbow;
	Vec2d  rangeFrontKnee;
	Vec2d  rangeStifle;
	Vec2d  rangeRearKnee;
	
	// BallJoint制御目標
	Quaterniond goalWaistChest;
	Quaterniond goalWaistTail;
	Quaterniond goalTail;
	Quaterniond goalChestNeck;
	Quaterniond goalNeckHead;
	Quaterniond goalShoulder;
	Quaterniond goalFrontAnkle;
	Quaterniond goalHip;
	Quaterniond goalRearAnkle;

	// HingeJoint制御目標
	double originElbow;
	double originFrontKnee;
	double originStifle;
	double originRearKnee;

	// BallJoint可動域制限の中心
	Vec3d limitDirWaistChest;
	Vec3d limitDirWaistTail;
	Vec3d limitDirTail;
	Vec3d limitDirChestNeck;
	Vec3d limitDirNeckHead;
	Vec3d limitDirShoulder;
	Vec3d limitDirFrontAnkle;
	Vec3d limitDirHip;
	Vec3d limitDirRearAnkle;

	/// BallJointのswing可動域:
	Vec2d limitSwingWaistChest;
	Vec2d limitSwingWaistTail;
	Vec2d limitSwingTail;
	Vec2d limitSwingChestNeck;
	Vec2d limitSwingNeckHead;
	Vec2d limitSwingShoulder;
	Vec2d limitSwingFrontAnkle;
	Vec2d limitSwingHip;
	Vec2d limitSwingRearAnkle;

	/// BallJointのtwist可動域
	Vec2d limitTwistWaistChest;
	Vec2d limitTwistWaistTail;
	Vec2d limitTwistTail;
	Vec2d limitTwistChestNeck;
	Vec2d limitTwistNeckHead;
	Vec2d limitTwistShoulder;
	Vec2d limitTwistFrontAnkle;
	Vec2d limitTwistHip;
	Vec2d limitTwistRearAnkle;

	// 関節の出せる力の最大値
	double fMaxWaistChest;
	double fMaxChestNeck;
	double fMaxNeckHead;
	double fMaxWaistTail;
	double fMaxTail12;
	double fMaxTail23;
	double fMaxLeftShoulder;
	double fMaxLeftElbow;
	double fMaxLeftFrontKnee;
	double fMaxLeftFrontAnkle;
	double fMaxLeftHip;
	double fMaxLeftStifle;
	double fMaxLeftRearKnee;
	double fMaxLeftRearAnkle;
	double fMaxRightShoulder;
	double fMaxRightElbow;
	double fMaxRightFrontKnee;
	double fMaxRightFrontAnkle;
	double fMaxRightHip;
	double fMaxRightStifle;
	double fMaxRightRearKnee;
	double fMaxRightRearAnkle;

	// 物体の摩擦係数
	float materialMu;

	/// 裏オプション
	bool noLegs;
	/// ダイナミカルを入れるかどうか
	bool dynamicalMode;
	/// 全体の体重
	double totalMass;
	/// fMaxを入れるかどうか
	bool flagFMax;
	/// 稼働域制限を入れるかどうか
	bool flagRange;

	CRFourLegsAnimalBodyDesc(bool enableRange = false, bool enableFMax = false);
};

// ------------------------------------------------------------------------------
/// 手首から先の手モデルのインターフェイス
struct CRManipulatorIf : CRBodyIf {
	SPR_IFDEF(CRManipulator);

	/** @brief 剛体の数を返す
	*/
	int NSolids();

	/** @brief 関節の数を返す
	*/
	int NJoints();

	/** @brief ボールジョイントの数を返す
	*/
	int NBallJoints();

	/** @brief ヒンジジョイントの数を返す
	*/
	int NHingeJoints();
	
};

/// 手首から先の手モデルのデスクリプタ
struct CRManipulatorDesc : CRBodyDesc {
	
	SPR_DESCDEF(CRManipulator);

	enum CRFingerSolids{

		///////////////////////////////////////////////////////////////////
		//																 //
		// < 剛体定義 >													 //
		// ROOT：手のひらの付け根										 //
		// 指番号：  [0]親指，[1]人差し指, [2]中指, [3]薬指, [4]小指	 //
		// 指内番号：[0]付け根に近い方									 //
		//																 //
		///////////////////////////////////////////////////////////////////

		// Center part of the solids
		SO_ROOT = 0,
		SO_FINGER_0_0, SO_FINGER_0_1, SO_FINGER_0_2,
		SO_FINGER_1_0, SO_FINGER_1_1, SO_FINGER_1_2, SO_FINGER_1_3,
		SO_FINGER_2_0, SO_FINGER_2_1, SO_FINGER_2_2, SO_FINGER_2_3,
		SO_FINGER_3_0, SO_FINGER_3_1, SO_FINGER_3_2, SO_FINGER_3_3,
		SO_FINGER_4_0, SO_FINGER_4_1, SO_FINGER_4_2, SO_FINGER_4_3,
		
		// -- The number of the solids
		SO_NSOLIDS
	};

	enum CRFingerJoints{

		///////////////////////////////////////////////////////////////////
		//																 //
		// < 関節定義 >													 //
		// ROOT：手のひらの付け根										 //
		// 関節番号：  [0]親指，[1]人差し指, [2]中指, [3]薬指, [4]小指	 //
		// 関節内番号：[0]付け根に近い方								 //
		//																 //
		///////////////////////////////////////////////////////////////////

		// -- Center part of the joints
		JO_00 = 0, JO_01, JO_02,
		JO_10,	   JO_11, JO_12, JO_13,
		JO_20,	   JO_21, JO_22, JO_23,
		JO_30,	   JO_31, JO_32, JO_33,
		JO_40,	   JO_41, JO_42, JO_43,

		// -- The number of the all joints (ball + hinge)
		JO_NJOINTS								//(nHingeJoints = nJoints - nBallJoints - 1)
	};

	int soNSolids;
	int joNBallJoints;
	int joNHingeJoints;
	int joNJoints;

	/// サイズに関するパラメータ(breadth, length, thickness)
	double bRoot;
	double bFinger00, lFinger00, tFinger00;
	double bFinger01, lFinger01, tFinger01;
	double bFinger02, lFinger02, tFinger02;
	double bFinger10, lFigner10, tFinger10;
	double bFinger11, lFinger11, tFinger11;
	double bFinger12, lFigner12, tFinger12;
	double bFinger13, lFinger13, tFigner13;
	double bFinger20, lFigner20, tFinger20;
	double bFinger21, lFinger21, tFinger21;
	double bFinger22, lFigner22, tFinger22;
	double bFinger23, lFinger23, tFigner23;
	double bFinger30, lFigner30, tFinger30;
	double bFinger31, lFinger31, tFinger31;
	double bFinger32, lFigner32, tFinger32;
	double bFinger33, lFinger33, tFigner33;
	double bFinger40, lFigner40, tFinger40;
	double bFinger41, lFinger41, tFinger41;
	double bFinger42, lFigner42, tFinger42;
	double bFinger43, lFinger43, tFigner43;

	/// 各BallJointのバネダンパ
	double spring00, damper00;
	double spring01, damper01;
	double spring02, damper02;
	double spring10, damper10;
	double spring11, damper11;
	double spring12, damper12;
	double spring13, damper13;
	double spring20, damper20;
	double spring21, damper21;
	double spring22, damper22;
	double spring23, damper23;
	double spring30, damper30;
	double spring31, damper31;
	double spring32, damper32;
	double spring33, damper33;
	double spring40, damper40;
	double spring41, damper41;
	double spring42, damper42;
	double spring43, damper43;
	
	/// HingeJoint可動域制限
	Vec2d  range01, range02;
	Vec2d  range11, range12, range13;
	Vec2d  range21, range22, range23;
	Vec2d  range31, range32, range33;
	Vec2d  range41, range42, range43;
	
	// BallJoint制御目標
	Quaterniond goalFinger0;
	Quaterniond goalFinger1;
	Quaterniond goalFinger2;
	Quaterniond goalFigner3;
	Quaterniond goalFinger4;

	/// BallJointのswing可動域:
	Vec2d limitSwing00;
	Vec2d limitSwing10;
	Vec2d limitSwing20;
	Vec2d limitSwing30;
	Vec2d limitSwing40;

	/// BallJointのtwist可動域
	Vec2d limitTwist00;
	Vec2d limitTwist10;
	Vec2d limitTwist20;
	Vec2d limitTwist30;
	Vec2d limitTwist40;

	// 関節の出せる力の最大値
	double fMaxFinger0;
	double fMaxFinger1;
	double fMaxFinger2;
	double fMaxFinger3;
	double fMaxFinger4;

	// 物体の摩擦係数
	float materialMu;

	/// ダイナミカルを入れるかどうか
	bool dynamicalMode;
	/// fMaxを入れるかどうか
	bool flagFMax;
	/// 稼働域制限を入れるかどうか
	bool flagRange;
};



// ------------------------------------------------------------------------------
/// おもに胴体と足のみの人体モデルのインターフェイス（歩行制御用）
struct CRTrunkFootHumanBodyIf : CRBodyIf {
	SPR_IFDEF(CRTrunkFootHumanBody);

};

/// 胴体・足人体モデルのデスクリプタ
struct CRTrunkFootHumanBodyDesc : CRBodyDesc {
	SPR_DESCDEF(CRTrunkFootHumanBody);

	enum CRHumanSolids{
		// 剛体
		SO_WAIST=0, SO_CHEST, SO_HEAD, SO_RIGHT_FOOT, SO_LEFT_FOOT,
		// 剛体の数
		SO_NSOLIDS
	};

	enum CRHumanJoints{
		// 関節
		JO_WAIST_CHEST=0, JO_CHEST_HEAD,
		// 関節の数
		JO_NJOINTS
	};

	/// サイズに関するパラメータ
	double waistHeight, waistBreadth, waistThickness;
	double chestHeight, chestBreadth, chestThickness;
	double neckLength;
	double headDiameter;
	double footLength, footBreadth, footThickness;

	/// 各関節のバネダンパ
	double springWaistChest, damperWaistChest;
	double springChestHead,    damperChestHead;

	/// 可動域制限
	Vec2d rangeWaistChest;
	Vec2d rangeChestHead;

	CRTrunkFootHumanBodyDesc();

};
//@}

// ------------------------------------------------------------------------------
/// おもに胴体と足のみの哺乳類モデルのインターフェイス（制御用）
struct CRFourLegsTinyAnimalBodyIf : CRBodyIf{
	SPR_IFDEF(CRFourLegsTinyAnimalBody);
};

/// 胴体と足のみの哺乳類モデルのディスクリプタ
struct CRFourLegsTinyAnimalBodyDesc : CRBodyDesc{
	SPR_DESCDEF(CRFourLegsTinyAnimalBody);

	enum CRAnimalSolids{
		// 剛体
		SO_BODY=0, SO_HEAD,SO_RIGHT_FRONT_LEG, SO_LEFT_FRONT_LEG,
		SO_RIGHT_REAR_LEG, SO_LEFT_REAR_LEG,
		// 剛体の数
		SO_NSOLIDS
	};

	enum CRAnimalJoints{
		// 関節
		JO_BODY_HEAD=0,
		JO_BODY_RIGHT_FRONT_LEG, JO_BODY_LEFT_FRONT_LEG,
		JO_BODY_RIGHT_REAR_LEG,  JO_BODY_LEFT_REAR_LEG,
		// 関節の数
		JO_NJOINTS
	};

	/// サイズに関するパラメータ
	double bodyHeight, bodyBreadth, bodyThickness;
	double frontLegsBreadth, frontLegsHeight, frontLegsThickness;
	double rearLegsBreadth, rearLegsHeight, rearLegsThickness;

	/// 各関節のバネダンパ
	double springFront, damperFront;
	double springRear,  damperRear;

	/// 可動域制限
	Vec2d rangeFrontSwing;
	Vec2d rangeFrontTwist;
	Vec2d rangeRearSwing;
	Vec2d rangeRearTwist;

	CRFourLegsTinyAnimalBodyDesc();
};

}

#endif//SPR_CRBODY_H
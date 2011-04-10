/*
 *  Copyright (c) 2003-2006, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
/**
 Springhead2/src/Samples/Vehicles/main.cpp

�y�T�v�zSpringhead2�̑����I�ȃf����P�i�D8�����{�b�g�̕��s
 
�y�d�l�z

*/

#include "../../SampleApp.h"
#include "robot.h"

class MyApp : public SampleApp{
public:
	enum {
		ID_FORWARD,
		ID_BACKWARD,
		ID_TURN_LEFT,
		ID_TURN_RIGHT,
		ID_STOP,
	};

	Robot robot;

public:
	MyApp(){
		AddMenu(MENU_SCENE, "control robot");
		AddAction(MENU_SCENE, ID_FORWARD, "move forward");
		AddHotKey(MENU_SCENE, ID_FORWARD, 'f');
		AddAction(MENU_SCENE, ID_BACKWARD, "move backward");
		AddHotKey(MENU_SCENE, ID_BACKWARD, 'b');
		AddAction(MENU_SCENE, ID_TURN_LEFT, "turn left");
		AddHotKey(MENU_SCENE, ID_TURN_LEFT, 'l');
		AddAction(MENU_SCENE, ID_TURN_RIGHT, "turn right");
		AddHotKey(MENU_SCENE, ID_TURN_RIGHT, 'r');
		AddAction(MENU_SCENE, ID_STOP, "stop");
		AddHotKey(MENU_SCENE, ID_STOP, 's');

		running = false;
	}

	virtual void BuildScene(){
		//CreateFloor();								//	��
		Posed pose;
		pose.Pos() = Vec3d(3.0, 2.0, 0.0);
		robot.Build(pose, phScene, GetSdk()->GetPHSdk());			//	���{�b�g
		pose.Pos() = Vec3d(0.0, 1.0, 1.0);

		/*CDBoxDesc box;								//	�O�d�Ȃ��Ă��锠
		box.boxsize = Vec3f(1.0, 1.0, 2.0);
		CDBoxIf* boxBody = phSdk->CreateShape(box)->Cast();
		PHSolidDesc sd;
		sd.mass *= 0.7;
		sd.inertia *= 0.7;
		PHSolidIf* soBox;
		for(int i=0; i<3;++i){
			soBox = scene->CreateSolid(sd);
			soBox->AddShape(boxBody);
			soBox->SetPose(pose);
			pose.PosY()+=1.0;
			pose.PosX()-=0.1;
		}*/

		phScene->SetGravity(Vec3f(0.0, -9.8, 0.0));	//	�d�͂�ݒ�

		fwScene->EnableRenderAxis(false, false, false);
	}
	virtual void OnAction(int menu, int id){
		if(menu == MENU_SCENE){
			if(id == ID_FORWARD)
				robot.Forward();
			if(id == ID_BACKWARD)
				robot.Backward();
			if(id == ID_TURN_LEFT)
				robot.TurnLeft();
			if(id == ID_TURN_RIGHT)
				robot.TurnRight();
			if(id == ID_STOP)
				robot.Stop();
		}
		SampleApp::OnAction(menu, id);
	}
} app;

int main(int argc, char* argv[]){
	app.Init(argc, argv);
	app.StartMainLoop();
	return 0;
}
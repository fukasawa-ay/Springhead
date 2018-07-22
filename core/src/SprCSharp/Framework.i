#	Do not edit. RunSwig.bat will update this file.
%ignore Spr::FWDialogIf::CreateControl;
%ignore Spr::FWObjectIf::LoadMesh;
%ignore Spr::FWSceneIf::AddHumanInterface;
%ignore Spr::FWSdkIf::LoadScene;
%ignore Spr::FWSdkIf::SaveScene;
%ignore Spr::FWApp::timers;
%ignore Spr::FWApp::Timers;
%ignore Spr::FWApp::Init(int argc, char* argv[]);
%ignore Spr::FWApp::GRInit(int argc, char* argv[], int type);
%ignore Spr::FWAppBase::timers;
%ignore Spr::FWAppBase::Timers;
// %ignore Spr::FWApp;
%module Framework
%include "../../include/SprBase.h"
%include "../../include/Base/Env.h"
%include "../../include/Foundation/SprObject.h"
%include "../../include/Foundation/SprScene.h"
%include "../../include/Foundation/SprUTTimer.h"
%include "../../include/Collision/SprCDShape.h"
%include "../../include/HumanInterface/SprHIBase.h"
%include "../../include/HumanInterface/SprHIDevice.h"
%include "../../include/HumanInterface/SprHIKeyMouse.h"
%include "../../include/HumanInterface/SprHISkeletonSensor.h"
%include "../../include/Physics/SprPHFemMeshNew.h"
%include "../../include/Physics/SprPHSolid.h"
%include "../../include/Physics/SprPHHaptic.h"
%include "../../include/Physics/SprPHScene.h"
%include "../../include/Base/BaseUtility.h"
%include "../../include/Framework/SprFWApp.h"
%include "../../include/Framework/SprFWEditor.h"
%include "../../include/Framework/SprFWObject.h"
%include "../../include/Framework/SprFWOpObj.h"
%include "../../include/Framework/SprFWFemMesh.h"
%include "../../include/Framework/SprFWFemMeshNew.h"
%include "../../include/Framework/SprFWHapticPointer.h"
%include "../../include/Framework/SprFWScene.h"
%include "../../include/Framework/SprFWSdk.h"
%include "../../include/Framework/SprFWSkeletonSensor.h"
%include "../../include/Framework/SprFWWin.h"
%include "../../include/Framework/SprFWOptimizer.h"
%include "../../include/Framework/SprFWOpHapticHandler.h"
%include "../../include/Graphics/SprGRScene.h"
/*
#define DOUBLECOLON :: 
%include "../../include/Springhead.h"
%include "../../include/base/Env.h"
%include "../../include/Foundation/SprObject.h"
%include "../../include/Foundation/SprScene.h"
%include "../../include/Foundation/SprUTTimer.h"
%include "../../include/Framework/SprFWApp.h"
%include "../../include/Framework/SprFWEditor.h"
%include "../../include/Framework/SprFWFemMesh.h"
%include "../../include/Framework/SprFWFemMeshNew.h"
%include "../../include/Framework/SprFWHapticPointer.h"
%include "../../include/Framework/SprFWObject.h"
%include "../../include/Framework/SprFWOpObj.h"
%include "../../include/Framework/SprFWScene.h"
%include "../../include/Framework/SprFWSdk.h"
%include "../../include/Framework/SprFWSkeletonSensor.h"
%include "../../include/Framework/SprFWWin.h"
%include "../../include/Framework/SprFWOptimizer.h"
%include "../../include/Base/BaseDebug.h"
%include "../../src/Foundation/UTTypeDesc.h"
%include "../Foundation/Foundation.h"
%include "../Foundation/Object.h"
%include "../Foundation/Scene.h"
%include "../Foundation/UTBaseType.h"
%include "../Foundation/UTClapack.h"
%include "../Foundation/UTDllLoader.h"
%include "../Foundation/UTDllLoaderImpl.h"
%include "../Foundation/UTLoadContext.h"
%include "../Foundation/UTLoadHandler.h"
%include "../Foundation/UTMMTimer.h"
%include "../Foundation/UTPath.h"
%include "../Foundation/UTPreciseTimer.h"
%include "../Foundation/UTQPTimer.h"
%include "../Foundation/UTSocket.h"
%include "../Foundation/UTTimer.h"
%include "../Foundation/UTTypeDesc.h"
%include "../Framework/Framework.h"
%include "../Framework/FWFemMesh.h"
%include "../Framework/FWFemMeshNew.h"
%include "../Framework/FWGLUI.h"
%include "../Framework/FWGLUT.h"
%include "../Framework/FWGraphicsHandler.h"
%include "../Framework/FWHapticPointer.h"
%include "../Framework/FWObject.h"
%include "../Framework/FWOldSpringheadNode.h"
%include "../Framework/FWOldSpringheadNodeHandler.h"
%include "../Framework/FWOpObj.h"
%include "../Framework/FWScene.h"
%include "../Framework/FWSdk.h"
%include "../Framework/FWSkeletonSensor.h"
%include "../Framework/FWSprTetgen.h"
%include "../Framework/FWWin.h"
*/

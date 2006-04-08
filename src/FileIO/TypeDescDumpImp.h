//	Do not edit. MakeTypeDesc.bat will update this file.
	
	PHMaterial* pPHMaterial = NULL;
	desc = DBG_NEW FITypeDesc("PHMaterial");
	desc->size = sizeof(PHMaterial);
	desc->access = DBG_NEW FIAccess<PHMaterial>;
	field = desc->AddField("", "float", "mu", "");
	field->offset = (char*)&(pPHMaterial->mu) - (char*)pPHMaterial;
	field = desc->AddField("", "float", "mu0", "");
	field->offset = (char*)&(pPHMaterial->mu0) - (char*)pPHMaterial;
	field = desc->AddField("", "float", "e", "");
	field->offset = (char*)&(pPHMaterial->e) - (char*)pPHMaterial;
	db->RegisterDesc(desc);
	
	CDShapeDesc* pCDShapeDesc = NULL;
	desc = DBG_NEW FITypeDesc("CDShapeDesc");
	desc->size = sizeof(CDShapeDesc);
	desc->ifInfo = CDShapeIf::GetIfInfoStatic();
	desc->access = DBG_NEW FIAccess<CDShapeDesc>;
	field = desc->AddField("ShapeType", "enum", "type",  "");
	field->AddEnumConst("CONVEX");
	field->AddEnumConst("CONVEXMESH");
	field->AddEnumConst("SPHERE");
	field->AddEnumConst("SPHERE");
	field->offset = (char*)(&pCDShapeDesc->type) - (char*)pCDShapeDesc;
	db->RegisterDesc(desc);
	
	CDConvexMeshDesc* pCDConvexMeshDesc = NULL;
	desc = DBG_NEW FITypeDesc("CDConvexMeshDesc");
	desc->size = sizeof(CDConvexMeshDesc);
	desc->ifInfo = CDConvexMeshIf::GetIfInfoStatic();
	desc->access = DBG_NEW FIAccess<CDConvexMeshDesc>;
	field = desc->AddBase("CDShapeDesc");
	field->offset = (char*)(CDShapeDesc*)pCDConvexMeshDesc - (char*)pCDConvexMeshDesc;
	field = desc->AddField("vector", "Vec3f", "vertices", "");
	field->offset = (char*)&(pCDConvexMeshDesc->vertices) - (char*)pCDConvexMeshDesc;
	field = desc->AddField("", "PHMaterial", "material", "");
	field->offset = (char*)&(pCDConvexMeshDesc->material) - (char*)pCDConvexMeshDesc;
	db->RegisterDesc(desc);
	
	CDSphereDesc* pCDSphereDesc = NULL;
	desc = DBG_NEW FITypeDesc("CDSphereDesc");
	desc->size = sizeof(CDSphereDesc);
	desc->ifInfo = CDSphereIf::GetIfInfoStatic();
	desc->access = DBG_NEW FIAccess<CDSphereDesc>;
	field = desc->AddBase("CDShapeDesc");
	field->offset = (char*)(CDShapeDesc*)pCDSphereDesc - (char*)pCDSphereDesc;
	field = desc->AddField("", "Vec3f", "center", "");
	field->offset = (char*)&(pCDSphereDesc->center) - (char*)pCDSphereDesc;
	field = desc->AddField("", "float", "radius", "");
	field->offset = (char*)&(pCDSphereDesc->radius) - (char*)pCDSphereDesc;
	field = desc->AddField("", "PHMaterial", "material", "");
	field->offset = (char*)&(pCDSphereDesc->material) - (char*)pCDSphereDesc;
	db->RegisterDesc(desc);
	
	GRLight* pGRLight = NULL;
	desc = DBG_NEW FITypeDesc("GRLight");
	desc->size = sizeof(GRLight);
	desc->access = DBG_NEW FIAccess<GRLight>;
	field = desc->AddField("", "Vec4f", "ambient", "");
	field->offset = (char*)&(pGRLight->ambient) - (char*)pGRLight;
	field = desc->AddField("", "Vec4f", "diffuse", "");
	field->offset = (char*)&(pGRLight->diffuse) - (char*)pGRLight;
	field = desc->AddField("", "Vec4f", "specular", "");
	field->offset = (char*)&(pGRLight->specular) - (char*)pGRLight;
	field = desc->AddField("", "Vec4f", "position", "");
	field->offset = (char*)&(pGRLight->position) - (char*)pGRLight;
	field = desc->AddField("", "float", "range", "");
	field->offset = (char*)&(pGRLight->range) - (char*)pGRLight;
	field = desc->AddField("", "float", "attenuation0", "");
	field->offset = (char*)&(pGRLight->attenuation0) - (char*)pGRLight;
	field = desc->AddField("", "float", "attenuation1", "");
	field->offset = (char*)&(pGRLight->attenuation1) - (char*)pGRLight;
	field = desc->AddField("", "float", "attenuation2", "");
	field->offset = (char*)&(pGRLight->attenuation2) - (char*)pGRLight;
	field = desc->AddField("", "Vec3f", "spotDirection", "");
	field->offset = (char*)&(pGRLight->spotDirection) - (char*)pGRLight;
	field = desc->AddField("", "float", "spotFalloff", "");
	field->offset = (char*)&(pGRLight->spotFalloff) - (char*)pGRLight;
	field = desc->AddField("", "float", "spotInner", "");
	field->offset = (char*)&(pGRLight->spotInner) - (char*)pGRLight;
	field = desc->AddField("", "float", "spotCutoff", "");
	field->offset = (char*)&(pGRLight->spotCutoff) - (char*)pGRLight;
	db->RegisterDesc(desc);
	
	GRMaterial* pGRMaterial = NULL;
	desc = DBG_NEW FITypeDesc("GRMaterial");
	desc->size = sizeof(GRMaterial);
	desc->access = DBG_NEW FIAccess<GRMaterial>;
	field = desc->AddField("", "Vec4f", "ambient", "");
	field->offset = (char*)&(pGRMaterial->ambient) - (char*)pGRMaterial;
	field = desc->AddField("", "Vec4f", "diffuse", "");
	field->offset = (char*)&(pGRMaterial->diffuse) - (char*)pGRMaterial;
	field = desc->AddField("", "Vec4f", "specular", "");
	field->offset = (char*)&(pGRMaterial->specular) - (char*)pGRMaterial;
	field = desc->AddField("", "Vec4f", "emissive", "");
	field->offset = (char*)&(pGRMaterial->emissive) - (char*)pGRMaterial;
	field = desc->AddField("", "float", "power", "");
	field->offset = (char*)&(pGRMaterial->power) - (char*)pGRMaterial;
	field = desc->AddField("", "string", "texture", "");
	field->offset = (char*)&(pGRMaterial->texture) - (char*)pGRMaterial;
	db->RegisterDesc(desc);
	
	GRCamera* pGRCamera = NULL;
	desc = DBG_NEW FITypeDesc("GRCamera");
	desc->size = sizeof(GRCamera);
	desc->access = DBG_NEW FIAccess<GRCamera>;
	field = desc->AddField("", "Vec2f", "size", "");
	field->offset = (char*)&(pGRCamera->size) - (char*)pGRCamera;
	field = desc->AddField("", "Vec2f", "center", "");
	field->offset = (char*)&(pGRCamera->center) - (char*)pGRCamera;
	field = desc->AddField("", "float", "front", "");
	field->offset = (char*)&(pGRCamera->front) - (char*)pGRCamera;
	field = desc->AddField("", "float", "back", "");
	field->offset = (char*)&(pGRCamera->back) - (char*)pGRCamera;
	db->RegisterDesc(desc);
	
	PHJointDesc* pPHJointDesc = NULL;
	desc = DBG_NEW FITypeDesc("PHJointDesc");
	desc->size = sizeof(PHJointDesc);
	desc->ifInfo = PHJointIf::GetIfInfoStatic();
	desc->access = DBG_NEW FIAccess<PHJointDesc>;
	field = desc->AddField("JointType", "enum", "type",  "");
	field->AddEnumConst("JOINT_CONTACT");
	field->AddEnumConst("JOINT_HINGE");
	field->AddEnumConst("JOINT_SLIDER");
	field->AddEnumConst("JOINT_BALL");
	field->offset = (char*)(&pPHJointDesc->type) - (char*)pPHJointDesc;
	field = desc->AddField("", "bool", "bEnabled", "");
	field->offset = (char*)&(pPHJointDesc->bEnabled) - (char*)pPHJointDesc;
	field = desc->AddField("", "Posed", "poseJoint", "2");
	field->offset = (char*)&(pPHJointDesc->poseJoint) - (char*)pPHJointDesc;
	db->RegisterDesc(desc);
	
	PHJoint1DDesc* pPHJoint1DDesc = NULL;
	desc = DBG_NEW FITypeDesc("PHJoint1DDesc");
	desc->size = sizeof(PHJoint1DDesc);
	desc->ifInfo = PHJoint1DIf::GetIfInfoStatic();
	desc->access = DBG_NEW FIAccess<PHJoint1DDesc>;
	field = desc->AddBase("PHJointDesc");
	field->offset = (char*)(PHJointDesc*)pPHJoint1DDesc - (char*)pPHJoint1DDesc;
	field = desc->AddField("", "double", "lower", "");
	field->offset = (char*)&(pPHJoint1DDesc->lower) - (char*)pPHJoint1DDesc;
	field = desc->AddField("", "double", "upper", "");
	field->offset = (char*)&(pPHJoint1DDesc->upper) - (char*)pPHJoint1DDesc;
	field = desc->AddField("", "double", "spring", "");
	field->offset = (char*)&(pPHJoint1DDesc->spring) - (char*)pPHJoint1DDesc;
	field = desc->AddField("", "double", "origin", "");
	field->offset = (char*)&(pPHJoint1DDesc->origin) - (char*)pPHJoint1DDesc;
	field = desc->AddField("", "double", "damper", "");
	field->offset = (char*)&(pPHJoint1DDesc->damper) - (char*)pPHJoint1DDesc;
	field = desc->AddField("", "double", "torque", "");
	field->offset = (char*)&(pPHJoint1DDesc->torque) - (char*)pPHJoint1DDesc;
	db->RegisterDesc(desc);
	
	PHHingeJointDesc* pPHHingeJointDesc = NULL;
	desc = DBG_NEW FITypeDesc("PHHingeJointDesc");
	desc->size = sizeof(PHHingeJointDesc);
	desc->ifInfo = PHHingeJointIf::GetIfInfoStatic();
	desc->access = DBG_NEW FIAccess<PHHingeJointDesc>;
	field = desc->AddBase("PHJoint1DDesc");
	field->offset = (char*)(PHJoint1DDesc*)pPHHingeJointDesc - (char*)pPHHingeJointDesc;
	db->RegisterDesc(desc);
	
	PHSliderJointDesc* pPHSliderJointDesc = NULL;
	desc = DBG_NEW FITypeDesc("PHSliderJointDesc");
	desc->size = sizeof(PHSliderJointDesc);
	desc->ifInfo = PHSliderJointIf::GetIfInfoStatic();
	desc->access = DBG_NEW FIAccess<PHSliderJointDesc>;
	field = desc->AddBase("PHJoint1DDesc");
	field->offset = (char*)(PHJoint1DDesc*)pPHSliderJointDesc - (char*)pPHSliderJointDesc;
	db->RegisterDesc(desc);
	
	PHBallJointDesc* pPHBallJointDesc = NULL;
	desc = DBG_NEW FITypeDesc("PHBallJointDesc");
	desc->size = sizeof(PHBallJointDesc);
	desc->ifInfo = PHBallJointIf::GetIfInfoStatic();
	desc->access = DBG_NEW FIAccess<PHBallJointDesc>;
	field = desc->AddBase("PHJointDesc");
	field->offset = (char*)(PHJointDesc*)pPHBallJointDesc - (char*)pPHBallJointDesc;
	field = desc->AddField("", "double", "max_angle", "");
	field->offset = (char*)&(pPHBallJointDesc->max_angle) - (char*)pPHBallJointDesc;
	db->RegisterDesc(desc);
	
	PHSceneState* pPHSceneState = NULL;
	desc = DBG_NEW FITypeDesc("PHSceneState");
	desc->size = sizeof(PHSceneState);
	desc->access = DBG_NEW FIAccess<PHSceneState>;
	field = desc->AddField("", "double", "timeStep", "");
	field->offset = (char*)&(pPHSceneState->timeStep) - (char*)pPHSceneState;
	field = desc->AddField("", "unsigned", "count", "");
	field->offset = (char*)&(pPHSceneState->count) - (char*)pPHSceneState;
	db->RegisterDesc(desc);
	
	PHSceneDesc* pPHSceneDesc = NULL;
	desc = DBG_NEW FITypeDesc("PHSceneDesc");
	desc->size = sizeof(PHSceneDesc);
	desc->ifInfo = PHSceneIf::GetIfInfoStatic();
	desc->access = DBG_NEW FIAccess<PHSceneDesc>;
	field = desc->AddBase("PHSceneState");
	field->offset = (char*)(PHSceneState*)pPHSceneDesc - (char*)pPHSceneDesc;
	field = desc->AddField("SolverType", "enum", "contactSolver",  "");
	field->AddEnumConst("SOLVER_PENALTY");
	field->AddEnumConst("SOLVER_CONSTRAINT");
	field->AddEnumConst("SOLVER_CONSTRAINT");
	field->offset = (char*)(&pPHSceneDesc->contactSolver) - (char*)pPHSceneDesc;
	field = desc->AddField("", "Vec3f", "gravity", "");
	field->offset = (char*)&(pPHSceneDesc->gravity) - (char*)pPHSceneDesc;
	db->RegisterDesc(desc);
	
	PHSolidState* pPHSolidState = NULL;
	desc = DBG_NEW FITypeDesc("PHSolidState");
	desc->size = sizeof(PHSolidState);
	desc->access = DBG_NEW FIAccess<PHSolidState>;
	field = desc->AddField("", "Vec3d", "velocity", "");
	field->offset = (char*)&(pPHSolidState->velocity) - (char*)pPHSolidState;
	field = desc->AddField("", "Vec3d", "angVelocity", "");
	field->offset = (char*)&(pPHSolidState->angVelocity) - (char*)pPHSolidState;
	field = desc->AddField("", "Vec3d", "center", "");
	field->offset = (char*)&(pPHSolidState->center) - (char*)pPHSolidState;
	field = desc->AddField("", "Posed", "pose", "");
	field->offset = (char*)&(pPHSolidState->pose) - (char*)pPHSolidState;
	field = desc->AddField("", "Vec3d", "force", "");
	field->offset = (char*)&(pPHSolidState->force) - (char*)pPHSolidState;
	field = desc->AddField("", "Vec3d", "torque", "");
	field->offset = (char*)&(pPHSolidState->torque) - (char*)pPHSolidState;
	field = desc->AddField("", "Vec3d", "nextForce", "");
	field->offset = (char*)&(pPHSolidState->nextForce) - (char*)pPHSolidState;
	field = desc->AddField("", "Vec3d", "nextTorque", "");
	field->offset = (char*)&(pPHSolidState->nextTorque) - (char*)pPHSolidState;
	db->RegisterDesc(desc);
	
	PHSolidDesc* pPHSolidDesc = NULL;
	desc = DBG_NEW FITypeDesc("PHSolidDesc");
	desc->size = sizeof(PHSolidDesc);
	desc->ifInfo = PHSolidIf::GetIfInfoStatic();
	desc->access = DBG_NEW FIAccess<PHSolidDesc>;
	field = desc->AddBase("PHSolidState");
	field->offset = (char*)(PHSolidState*)pPHSolidDesc - (char*)pPHSolidDesc;
	field = desc->AddField("", "double", "mass", "");
	field->offset = (char*)&(pPHSolidDesc->mass) - (char*)pPHSolidDesc;
	field = desc->AddField("", "Matrix3d", "inertia", "");
	field->offset = (char*)&(pPHSolidDesc->inertia) - (char*)pPHSolidDesc;
	field = desc->AddField("", "bool", "gravity", "");
	field->offset = (char*)&(pPHSolidDesc->gravity) - (char*)pPHSolidDesc;
	db->RegisterDesc(desc);
	
	Vec2f* pVec2f = NULL;
	desc = DBG_NEW FITypeDesc("Vec2f");
	desc->size = sizeof(Vec2f);
	desc->access = DBG_NEW FIAccess<Vec2f>;
	field = desc->AddField("", "float", "x", "");
	field->offset = (char*)&(pVec2f->x) - (char*)pVec2f;
	field = desc->AddField("", "float", "y", "");
	field->offset = (char*)&(pVec2f->y) - (char*)pVec2f;
	db->RegisterDesc(desc);
	
	Vec2d* pVec2d = NULL;
	desc = DBG_NEW FITypeDesc("Vec2d");
	desc->size = sizeof(Vec2d);
	desc->access = DBG_NEW FIAccess<Vec2d>;
	field = desc->AddField("", "double", "x", "");
	field->offset = (char*)&(pVec2d->x) - (char*)pVec2d;
	field = desc->AddField("", "double", "y", "");
	field->offset = (char*)&(pVec2d->y) - (char*)pVec2d;
	db->RegisterDesc(desc);
	
	Vec3f* pVec3f = NULL;
	desc = DBG_NEW FITypeDesc("Vec3f");
	desc->size = sizeof(Vec3f);
	desc->access = DBG_NEW FIAccess<Vec3f>;
	field = desc->AddField("", "float", "x", "");
	field->offset = (char*)&(pVec3f->x) - (char*)pVec3f;
	field = desc->AddField("", "float", "y", "");
	field->offset = (char*)&(pVec3f->y) - (char*)pVec3f;
	field = desc->AddField("", "float", "z", "");
	field->offset = (char*)&(pVec3f->z) - (char*)pVec3f;
	db->RegisterDesc(desc);
	
	Vec3d* pVec3d = NULL;
	desc = DBG_NEW FITypeDesc("Vec3d");
	desc->size = sizeof(Vec3d);
	desc->access = DBG_NEW FIAccess<Vec3d>;
	field = desc->AddField("", "double", "x", "");
	field->offset = (char*)&(pVec3d->x) - (char*)pVec3d;
	field = desc->AddField("", "double", "y", "");
	field->offset = (char*)&(pVec3d->y) - (char*)pVec3d;
	field = desc->AddField("", "double", "z", "");
	field->offset = (char*)&(pVec3d->z) - (char*)pVec3d;
	db->RegisterDesc(desc);
	
	Vec4f* pVec4f = NULL;
	desc = DBG_NEW FITypeDesc("Vec4f");
	desc->size = sizeof(Vec4f);
	desc->access = DBG_NEW FIAccess<Vec4f>;
	field = desc->AddField("", "float", "x", "");
	field->offset = (char*)&(pVec4f->x) - (char*)pVec4f;
	field = desc->AddField("", "float", "y", "");
	field->offset = (char*)&(pVec4f->y) - (char*)pVec4f;
	field = desc->AddField("", "float", "z", "");
	field->offset = (char*)&(pVec4f->z) - (char*)pVec4f;
	field = desc->AddField("", "float", "w", "");
	field->offset = (char*)&(pVec4f->w) - (char*)pVec4f;
	db->RegisterDesc(desc);
	
	Vec4d* pVec4d = NULL;
	desc = DBG_NEW FITypeDesc("Vec4d");
	desc->size = sizeof(Vec4d);
	desc->access = DBG_NEW FIAccess<Vec4d>;
	field = desc->AddField("", "double", "x", "");
	field->offset = (char*)&(pVec4d->x) - (char*)pVec4d;
	field = desc->AddField("", "double", "y", "");
	field->offset = (char*)&(pVec4d->y) - (char*)pVec4d;
	field = desc->AddField("", "double", "z", "");
	field->offset = (char*)&(pVec4d->z) - (char*)pVec4d;
	field = desc->AddField("", "double", "w", "");
	field->offset = (char*)&(pVec4d->w) - (char*)pVec4d;
	db->RegisterDesc(desc);
	
	Quaternionf* pQuaternionf = NULL;
	desc = DBG_NEW FITypeDesc("Quaternionf");
	desc->size = sizeof(Quaternionf);
	desc->access = DBG_NEW FIAccess<Quaternionf>;
	field = desc->AddField("", "float", "x", "");
	field->offset = (char*)&(pQuaternionf->x) - (char*)pQuaternionf;
	field = desc->AddField("", "float", "y", "");
	field->offset = (char*)&(pQuaternionf->y) - (char*)pQuaternionf;
	field = desc->AddField("", "float", "z", "");
	field->offset = (char*)&(pQuaternionf->z) - (char*)pQuaternionf;
	field = desc->AddField("", "float", "w", "");
	field->offset = (char*)&(pQuaternionf->w) - (char*)pQuaternionf;
	db->RegisterDesc(desc);
	
	Quaterniond* pQuaterniond = NULL;
	desc = DBG_NEW FITypeDesc("Quaterniond");
	desc->size = sizeof(Quaterniond);
	desc->access = DBG_NEW FIAccess<Quaterniond>;
	field = desc->AddField("", "double", "x", "");
	field->offset = (char*)&(pQuaterniond->x) - (char*)pQuaterniond;
	field = desc->AddField("", "double", "y", "");
	field->offset = (char*)&(pQuaterniond->y) - (char*)pQuaterniond;
	field = desc->AddField("", "double", "z", "");
	field->offset = (char*)&(pQuaterniond->z) - (char*)pQuaterniond;
	field = desc->AddField("", "double", "w", "");
	field->offset = (char*)&(pQuaterniond->w) - (char*)pQuaterniond;
	db->RegisterDesc(desc);
	
	Posef* pPosef = NULL;
	desc = DBG_NEW FITypeDesc("Posef");
	desc->size = sizeof(Posef);
	desc->access = DBG_NEW FIAccess<Posef>;
	field = desc->AddField("", "float", "w", "");
	field->offset = (char*)&(pPosef->w) - (char*)pPosef;
	field = desc->AddField("", "float", "x", "");
	field->offset = (char*)&(pPosef->x) - (char*)pPosef;
	field = desc->AddField("", "float", "y", "");
	field->offset = (char*)&(pPosef->y) - (char*)pPosef;
	field = desc->AddField("", "float", "z", "");
	field->offset = (char*)&(pPosef->z) - (char*)pPosef;
	field = desc->AddField("", "float", "px", "");
	field->offset = (char*)&(pPosef->px) - (char*)pPosef;
	field = desc->AddField("", "float", "py", "");
	field->offset = (char*)&(pPosef->py) - (char*)pPosef;
	field = desc->AddField("", "float", "pz", "");
	field->offset = (char*)&(pPosef->pz) - (char*)pPosef;
	db->RegisterDesc(desc);
	
	Posed* pPosed = NULL;
	desc = DBG_NEW FITypeDesc("Posed");
	desc->size = sizeof(Posed);
	desc->access = DBG_NEW FIAccess<Posed>;
	field = desc->AddField("", "double", "w", "");
	field->offset = (char*)&(pPosed->w) - (char*)pPosed;
	field = desc->AddField("", "double", "x", "");
	field->offset = (char*)&(pPosed->x) - (char*)pPosed;
	field = desc->AddField("", "double", "y", "");
	field->offset = (char*)&(pPosed->y) - (char*)pPosed;
	field = desc->AddField("", "double", "z", "");
	field->offset = (char*)&(pPosed->z) - (char*)pPosed;
	field = desc->AddField("", "double", "px", "");
	field->offset = (char*)&(pPosed->px) - (char*)pPosed;
	field = desc->AddField("", "double", "py", "");
	field->offset = (char*)&(pPosed->py) - (char*)pPosed;
	field = desc->AddField("", "double", "pz", "");
	field->offset = (char*)&(pPosed->pz) - (char*)pPosed;
	db->RegisterDesc(desc);
	
	Matrix3f* pMatrix3f = NULL;
	desc = DBG_NEW FITypeDesc("Matrix3f");
	desc->size = sizeof(Matrix3f);
	desc->access = DBG_NEW FIAccess<Matrix3f>;
	field = desc->AddField("", "float", "data", "9");
	field->offset = (char*)&(pMatrix3f->data) - (char*)pMatrix3f;
	db->RegisterDesc(desc);
	
	Matrix3d* pMatrix3d = NULL;
	desc = DBG_NEW FITypeDesc("Matrix3d");
	desc->size = sizeof(Matrix3d);
	desc->access = DBG_NEW FIAccess<Matrix3d>;
	field = desc->AddField("", "double", "data", "9");
	field->offset = (char*)&(pMatrix3d->data) - (char*)pMatrix3d;
	db->RegisterDesc(desc);
	
	Affinef* pAffinef = NULL;
	desc = DBG_NEW FITypeDesc("Affinef");
	desc->size = sizeof(Affinef);
	desc->access = DBG_NEW FIAccess<Affinef>;
	field = desc->AddField("", "float", "data", "16");
	field->offset = (char*)&(pAffinef->data) - (char*)pAffinef;
	db->RegisterDesc(desc);
	
	Affined* pAffined = NULL;
	desc = DBG_NEW FITypeDesc("Affined");
	desc->size = sizeof(Affined);
	desc->access = DBG_NEW FIAccess<Affined>;
	field = desc->AddField("", "double", "data", "16");
	field->offset = (char*)&(pAffined->data) - (char*)pAffined;
	db->RegisterDesc(desc);

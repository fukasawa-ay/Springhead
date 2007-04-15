//	Do not edit. MakeTypeDesc.bat will update this file.
	
	PHMaterial* pPHMaterial = NULL;
	desc = DBG_NEW UTTypeDesc("PHMaterial");
	desc->size = sizeof(PHMaterial);
	desc->access = DBG_NEW UTAccess<PHMaterial>;
	field = desc->AddField("", "float", "mu", "");
	field->offset = int((char*)&(pPHMaterial->mu) - (char*)pPHMaterial);
	field = desc->AddField("", "float", "mu0", "");
	field->offset = int((char*)&(pPHMaterial->mu0) - (char*)pPHMaterial);
	field = desc->AddField("", "float", "e", "");
	field->offset = int((char*)&(pPHMaterial->e) - (char*)pPHMaterial);
	db->RegisterDesc(desc);
	
	CDShapeDesc* pCDShapeDesc = NULL;
	desc = DBG_NEW UTTypeDesc("CDShapeDesc");
	desc->size = sizeof(CDShapeDesc);
	desc->ifInfo = CDShapeIf::GetIfInfoStatic();
	((IfInfo*)CDShapeIf::GetIfInfoStatic())->desc = desc;
	desc->access = DBG_NEW UTAccess<CDShapeDesc>;
	field = desc->AddField("", "PHMaterial", "material", "");
	field->offset = int((char*)&(pCDShapeDesc->material) - (char*)pCDShapeDesc);
	db->RegisterDesc(desc);
	
	CDConvexMeshDesc* pCDConvexMeshDesc = NULL;
	desc = DBG_NEW UTTypeDesc("CDConvexMeshDesc");
	desc->size = sizeof(CDConvexMeshDesc);
	desc->ifInfo = CDConvexMeshIf::GetIfInfoStatic();
	((IfInfo*)CDConvexMeshIf::GetIfInfoStatic())->desc = desc;
	desc->access = DBG_NEW UTAccess<CDConvexMeshDesc>;
	field = desc->AddBase("CDShapeDesc");
	field->offset = int((char*)(CDShapeDesc*)pCDConvexMeshDesc - (char*)pCDConvexMeshDesc);
	field = desc->AddField("vector", "Vec3f", "vertices", "");
	field->offset = int((char*)&(pCDConvexMeshDesc->vertices) - (char*)pCDConvexMeshDesc);
	db->RegisterDesc(desc);
	
	CDSphereDesc* pCDSphereDesc = NULL;
	desc = DBG_NEW UTTypeDesc("CDSphereDesc");
	desc->size = sizeof(CDSphereDesc);
	desc->ifInfo = CDSphereIf::GetIfInfoStatic();
	((IfInfo*)CDSphereIf::GetIfInfoStatic())->desc = desc;
	desc->access = DBG_NEW UTAccess<CDSphereDesc>;
	field = desc->AddBase("CDShapeDesc");
	field->offset = int((char*)(CDShapeDesc*)pCDSphereDesc - (char*)pCDSphereDesc);
	field = desc->AddField("", "float", "radius", "");
	field->offset = int((char*)&(pCDSphereDesc->radius) - (char*)pCDSphereDesc);
	db->RegisterDesc(desc);
	
	CDCapsuleDesc* pCDCapsuleDesc = NULL;
	desc = DBG_NEW UTTypeDesc("CDCapsuleDesc");
	desc->size = sizeof(CDCapsuleDesc);
	desc->ifInfo = CDCapsuleIf::GetIfInfoStatic();
	((IfInfo*)CDCapsuleIf::GetIfInfoStatic())->desc = desc;
	desc->access = DBG_NEW UTAccess<CDCapsuleDesc>;
	field = desc->AddBase("CDShapeDesc");
	field->offset = int((char*)(CDShapeDesc*)pCDCapsuleDesc - (char*)pCDCapsuleDesc);
	field = desc->AddField("", "float", "radius", "");
	field->offset = int((char*)&(pCDCapsuleDesc->radius) - (char*)pCDCapsuleDesc);
	field = desc->AddField("", "float", "length", "");
	field->offset = int((char*)&(pCDCapsuleDesc->length) - (char*)pCDCapsuleDesc);
	db->RegisterDesc(desc);
	
	CDBoxDesc* pCDBoxDesc = NULL;
	desc = DBG_NEW UTTypeDesc("CDBoxDesc");
	desc->size = sizeof(CDBoxDesc);
	desc->ifInfo = CDBoxIf::GetIfInfoStatic();
	((IfInfo*)CDBoxIf::GetIfInfoStatic())->desc = desc;
	desc->access = DBG_NEW UTAccess<CDBoxDesc>;
	field = desc->AddBase("CDShapeDesc");
	field->offset = int((char*)(CDShapeDesc*)pCDBoxDesc - (char*)pCDBoxDesc);
	field = desc->AddField("", "Vec3f", "boxsize", "");
	field->offset = int((char*)&(pCDBoxDesc->boxsize) - (char*)pCDBoxDesc);
	db->RegisterDesc(desc);

//	Do not edit. MakeTypeDesc.bat will update this file.
	
	GRBlendMeshDesc* pGRBlendMeshDesc = NULL;
	desc = DBG_NEW UTTypeDesc("GRBlendMeshDesc");
	desc->size = sizeof(GRBlendMeshDesc);
	desc->ifInfo = GRBlendMeshIf::GetIfInfoStatic();
	((IfInfo*)GRBlendMeshIf::GetIfInfoStatic())->desc = desc;
	desc->access = DBG_NEW UTAccess<GRBlendMeshDesc>;
	field = desc->AddField("vector", "Vec3f", "positions", "");
	field->offset = int((char*)&(pGRBlendMeshDesc->positions) - (char*)pGRBlendMeshDesc);
	field = desc->AddField("vector", "Vec3f", "normals", "");
	field->offset = int((char*)&(pGRBlendMeshDesc->normals) - (char*)pGRBlendMeshDesc);
	field = desc->AddField("vector", "Vec4f", "colors", "");
	field->offset = int((char*)&(pGRBlendMeshDesc->colors) - (char*)pGRBlendMeshDesc);
	field = desc->AddField("vector", "Vec2f", "texCoords", "");
	field->offset = int((char*)&(pGRBlendMeshDesc->texCoords) - (char*)pGRBlendMeshDesc);
	field = desc->AddField("vector", "size_t", "faces", "");
	field->offset = int((char*)&(pGRBlendMeshDesc->faces) - (char*)pGRBlendMeshDesc);
	field = desc->AddField("vector", "Vec4f", "blends", "");
	field->offset = int((char*)&(pGRBlendMeshDesc->blends) - (char*)pGRBlendMeshDesc);
	field = desc->AddField("vector", "Vec4f", "matrixIndices", "");
	field->offset = int((char*)&(pGRBlendMeshDesc->matrixIndices) - (char*)pGRBlendMeshDesc);
	field = desc->AddField("vector", "Vec4f", "numMatrix", "");
	field->offset = int((char*)&(pGRBlendMeshDesc->numMatrix) - (char*)pGRBlendMeshDesc);
	db->RegisterDesc(desc);
	
	GRVisualDesc* pGRVisualDesc = NULL;
	desc = DBG_NEW UTTypeDesc("GRVisualDesc");
	desc->size = sizeof(GRVisualDesc);
	desc->ifInfo = GRVisualIf::GetIfInfoStatic();
	((IfInfo*)GRVisualIf::GetIfInfoStatic())->desc = desc;
	desc->access = DBG_NEW UTAccess<GRVisualDesc>;
	db->RegisterDesc(desc);
	
	GRFrameTransformMatrix* pGRFrameTransformMatrix = NULL;
	desc = DBG_NEW UTTypeDesc("GRFrameTransformMatrix");
	desc->size = sizeof(GRFrameTransformMatrix);
	desc->access = DBG_NEW UTAccess<GRFrameTransformMatrix>;
	field = desc->AddField("", "Affinef", "transform", "");
	field->offset = int((char*)&(pGRFrameTransformMatrix->transform) - (char*)pGRFrameTransformMatrix);
	db->RegisterDesc(desc);
	
	GRFrameDesc* pGRFrameDesc = NULL;
	desc = DBG_NEW UTTypeDesc("GRFrameDesc");
	desc->size = sizeof(GRFrameDesc);
	desc->ifInfo = GRFrameIf::GetIfInfoStatic();
	((IfInfo*)GRFrameIf::GetIfInfoStatic())->desc = desc;
	desc->access = DBG_NEW UTAccess<GRFrameDesc>;
	field = desc->AddBase("GRVisualDesc");
	field->offset = int((char*)(GRVisualDesc*)pGRFrameDesc - (char*)pGRFrameDesc);
	field = desc->AddField("", "Affinef", "transform", "");
	field->offset = int((char*)&(pGRFrameDesc->transform) - (char*)pGRFrameDesc);
	db->RegisterDesc(desc);
	
	GRMeshDesc* pGRMeshDesc = NULL;
	desc = DBG_NEW UTTypeDesc("GRMeshDesc");
	desc->size = sizeof(GRMeshDesc);
	desc->ifInfo = GRMeshIf::GetIfInfoStatic();
	((IfInfo*)GRMeshIf::GetIfInfoStatic())->desc = desc;
	desc->access = DBG_NEW UTAccess<GRMeshDesc>;
	field = desc->AddBase("GRVisualDesc");
	field->offset = int((char*)(GRVisualDesc*)pGRMeshDesc - (char*)pGRMeshDesc);
	field = desc->AddField("vector", "Vec3f", "positions", "");
	field->offset = int((char*)&(pGRMeshDesc->positions) - (char*)pGRMeshDesc);
	field = desc->AddField("vector", "Vec3f", "normals", "");
	field->offset = int((char*)&(pGRMeshDesc->normals) - (char*)pGRMeshDesc);
	field = desc->AddField("vector", "Vec4f", "colors", "");
	field->offset = int((char*)&(pGRMeshDesc->colors) - (char*)pGRMeshDesc);
	field = desc->AddField("vector", "Vec2f", "texCoords", "");
	field->offset = int((char*)&(pGRMeshDesc->texCoords) - (char*)pGRMeshDesc);
	field = desc->AddField("vector", "size_t", "faces", "");
	field->offset = int((char*)&(pGRMeshDesc->faces) - (char*)pGRMeshDesc);
	db->RegisterDesc(desc);
	
	GRLightDesc* pGRLightDesc = NULL;
	desc = DBG_NEW UTTypeDesc("GRLightDesc");
	desc->size = sizeof(GRLightDesc);
	desc->ifInfo = GRLightIf::GetIfInfoStatic();
	((IfInfo*)GRLightIf::GetIfInfoStatic())->desc = desc;
	desc->access = DBG_NEW UTAccess<GRLightDesc>;
	field = desc->AddBase("GRVisualDesc");
	field->offset = int((char*)(GRVisualDesc*)pGRLightDesc - (char*)pGRLightDesc);
	field = desc->AddField("", "Vec4f", "ambient", "");
	field->offset = int((char*)&(pGRLightDesc->ambient) - (char*)pGRLightDesc);
	field = desc->AddField("", "Vec4f", "diffuse", "");
	field->offset = int((char*)&(pGRLightDesc->diffuse) - (char*)pGRLightDesc);
	field = desc->AddField("", "Vec4f", "specular", "");
	field->offset = int((char*)&(pGRLightDesc->specular) - (char*)pGRLightDesc);
	field = desc->AddField("", "Vec4f", "position", "");
	field->offset = int((char*)&(pGRLightDesc->position) - (char*)pGRLightDesc);
	field = desc->AddField("", "float", "range", "");
	field->offset = int((char*)&(pGRLightDesc->range) - (char*)pGRLightDesc);
	field = desc->AddField("", "float", "attenuation0", "");
	field->offset = int((char*)&(pGRLightDesc->attenuation0) - (char*)pGRLightDesc);
	field = desc->AddField("", "float", "attenuation1", "");
	field->offset = int((char*)&(pGRLightDesc->attenuation1) - (char*)pGRLightDesc);
	field = desc->AddField("", "float", "attenuation2", "");
	field->offset = int((char*)&(pGRLightDesc->attenuation2) - (char*)pGRLightDesc);
	field = desc->AddField("", "Vec3f", "spotDirection", "");
	field->offset = int((char*)&(pGRLightDesc->spotDirection) - (char*)pGRLightDesc);
	field = desc->AddField("", "float", "spotFalloff", "");
	field->offset = int((char*)&(pGRLightDesc->spotFalloff) - (char*)pGRLightDesc);
	field = desc->AddField("", "float", "spotInner", "");
	field->offset = int((char*)&(pGRLightDesc->spotInner) - (char*)pGRLightDesc);
	field = desc->AddField("", "float", "spotCutoff", "");
	field->offset = int((char*)&(pGRLightDesc->spotCutoff) - (char*)pGRLightDesc);
	db->RegisterDesc(desc);
	
	GRMaterialDesc* pGRMaterialDesc = NULL;
	desc = DBG_NEW UTTypeDesc("GRMaterialDesc");
	desc->size = sizeof(GRMaterialDesc);
	desc->ifInfo = GRMaterialIf::GetIfInfoStatic();
	((IfInfo*)GRMaterialIf::GetIfInfoStatic())->desc = desc;
	desc->access = DBG_NEW UTAccess<GRMaterialDesc>;
	field = desc->AddBase("GRVisualDesc");
	field->offset = int((char*)(GRVisualDesc*)pGRMaterialDesc - (char*)pGRMaterialDesc);
	field = desc->AddField("", "Vec4f", "ambient", "");
	field->offset = int((char*)&(pGRMaterialDesc->ambient) - (char*)pGRMaterialDesc);
	field = desc->AddField("", "Vec4f", "diffuse", "");
	field->offset = int((char*)&(pGRMaterialDesc->diffuse) - (char*)pGRMaterialDesc);
	field = desc->AddField("", "Vec4f", "specular", "");
	field->offset = int((char*)&(pGRMaterialDesc->specular) - (char*)pGRMaterialDesc);
	field = desc->AddField("", "Vec4f", "emissive", "");
	field->offset = int((char*)&(pGRMaterialDesc->emissive) - (char*)pGRMaterialDesc);
	field = desc->AddField("", "float", "power", "");
	field->offset = int((char*)&(pGRMaterialDesc->power) - (char*)pGRMaterialDesc);
	field = desc->AddField("", "string", "texname", "");
	field->offset = int((char*)&(pGRMaterialDesc->texname) - (char*)pGRMaterialDesc);
	db->RegisterDesc(desc);
	
	GRCameraDesc* pGRCameraDesc = NULL;
	desc = DBG_NEW UTTypeDesc("GRCameraDesc");
	desc->size = sizeof(GRCameraDesc);
	desc->ifInfo = GRCameraIf::GetIfInfoStatic();
	((IfInfo*)GRCameraIf::GetIfInfoStatic())->desc = desc;
	desc->access = DBG_NEW UTAccess<GRCameraDesc>;
	field = desc->AddBase("GRVisualDesc");
	field->offset = int((char*)(GRVisualDesc*)pGRCameraDesc - (char*)pGRCameraDesc);
	field = desc->AddField("", "Vec2f", "size", "");
	field->offset = int((char*)&(pGRCameraDesc->size) - (char*)pGRCameraDesc);
	field = desc->AddField("", "Vec2f", "center", "");
	field->offset = int((char*)&(pGRCameraDesc->center) - (char*)pGRCameraDesc);
	field = desc->AddField("", "float", "front", "");
	field->offset = int((char*)&(pGRCameraDesc->front) - (char*)pGRCameraDesc);
	field = desc->AddField("", "float", "back", "");
	field->offset = int((char*)&(pGRCameraDesc->back) - (char*)pGRCameraDesc);
	db->RegisterDesc(desc);
	
	GRSceneDesc* pGRSceneDesc = NULL;
	desc = DBG_NEW UTTypeDesc("GRSceneDesc");
	desc->size = sizeof(GRSceneDesc);
	desc->ifInfo = GRSceneIf::GetIfInfoStatic();
	((IfInfo*)GRSceneIf::GetIfInfoStatic())->desc = desc;
	desc->access = DBG_NEW UTAccess<GRSceneDesc>;
	db->RegisterDesc(desc);
	
	GRSdkDesc* pGRSdkDesc = NULL;
	desc = DBG_NEW UTTypeDesc("GRSdkDesc");
	desc->size = sizeof(GRSdkDesc);
	desc->ifInfo = GRSdkIf::GetIfInfoStatic();
	((IfInfo*)GRSdkIf::GetIfInfoStatic())->desc = desc;
	desc->access = DBG_NEW UTAccess<GRSdkDesc>;
	db->RegisterDesc(desc);
	
	GRShaderFormat* pGRShaderFormat = NULL;
	desc = DBG_NEW UTTypeDesc("GRShaderFormat");
	desc->size = sizeof(GRShaderFormat);
	desc->access = DBG_NEW UTAccess<GRShaderFormat>;
	db->RegisterDesc(desc);
	
	GRSphereDesc* pGRSphereDesc = NULL;
	desc = DBG_NEW UTTypeDesc("GRSphereDesc");
	desc->size = sizeof(GRSphereDesc);
	desc->ifInfo = GRSphereIf::GetIfInfoStatic();
	((IfInfo*)GRSphereIf::GetIfInfoStatic())->desc = desc;
	desc->access = DBG_NEW UTAccess<GRSphereDesc>;
	field = desc->AddField("", "float", "radius", "");
	field->offset = int((char*)&(pGRSphereDesc->radius) - (char*)pGRSphereDesc);
	field = desc->AddField("", "int", "slices", "");
	field->offset = int((char*)&(pGRSphereDesc->slices) - (char*)pGRSphereDesc);
	field = desc->AddField("", "int", "stacks", "");
	field->offset = int((char*)&(pGRSphereDesc->stacks) - (char*)pGRSphereDesc);
	db->RegisterDesc(desc);
	
	GRVertexElement* pGRVertexElement = NULL;
	desc = DBG_NEW UTTypeDesc("GRVertexElement");
	desc->size = sizeof(GRVertexElement);
	desc->access = DBG_NEW UTAccess<GRVertexElement>;
	field = desc->AddField("", "short", "stream", "");
	field->offset = int((char*)&(pGRVertexElement->stream) - (char*)pGRVertexElement);
	field = desc->AddField("", "short", "offset", "");
	field->offset = int((char*)&(pGRVertexElement->offset) - (char*)pGRVertexElement);
	field = desc->AddField("", "char", "type", "");
	field->offset = int((char*)&(pGRVertexElement->type) - (char*)pGRVertexElement);
	field = desc->AddField("", "char", "method", "");
	field->offset = int((char*)&(pGRVertexElement->method) - (char*)pGRVertexElement);
	field = desc->AddField("", "char", "usage", "");
	field->offset = int((char*)&(pGRVertexElement->usage) - (char*)pGRVertexElement);
	field = desc->AddField("", "char", "usageIndex", "");
	field->offset = int((char*)&(pGRVertexElement->usageIndex) - (char*)pGRVertexElement);
	db->RegisterDesc(desc);
	
	GRVertexArray* pGRVertexArray = NULL;
	desc = DBG_NEW UTTypeDesc("GRVertexArray");
	desc->size = sizeof(GRVertexArray);
	desc->access = DBG_NEW UTAccess<GRVertexArray>;
	field = desc->AddField("", "GRVertexElement", "format", "");
	field->offset = int((char*)&(pGRVertexArray->format) - (char*)pGRVertexArray);
	field = desc->AddField("pointer", "void", "buffer", "");
	field->offset = int((char*)&(pGRVertexArray->buffer) - (char*)pGRVertexArray);
	db->RegisterDesc(desc);

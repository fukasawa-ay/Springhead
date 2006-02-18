#if 0	//	this file is not for compile but for bin/typedesc.exe
struct Vec2f{ float x; float y; };
struct Vec2d{ double x; double y; };
struct Vec3f{ float x; float y; float z; };
struct Vec3d{ double x; double y; double z; };
struct Vec4f{ float x; float y; float z; float w;};
struct Vec4d{ double x; double y; double z; double w;};
struct Quaternionf{ float x; float y; float z; float w;};
struct Quaterniond{ double x; double y; double z; double w;};
//struct Posef{ Quaternionf Ori(); Vec3f Pos(); }
//struct Posed{ Quaterniond Ori(); Vec3d Pos(); }
struct Posef{ float w; float x; float y; float z; float px; float py; float pz;}
struct Posed{ double w; double x; double y; double z; double px; double py; double pz;}
struct Matrix3f{ float data[9]; }
struct Matrix3d{ double data[9]; }
struct Affinef{ float data[16]; }
struct Affined{ double data[16]; }
#endif

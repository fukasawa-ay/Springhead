@echo off
if "%1"=="" goto end
if "%1"=="LISTSRC" goto ListSrc
cmd /v:on /c%0 LISTSRC %1 %2 %3 %4 %5 %6 %7 %8 %9
goto end

:ListSrc
set TARGET=%9 %8 %7 %6 %5 %4 %3 %2
set MODULE=%2
set CPP=EP%MODULE%.cpp
set HPP=EP%MODULE%.h
set SPRH=SprEP%MODULE%.h
set API_INCLUDE=../../include

rem makefileの作成とMakeの実行
set PATHORG=%PATH%;

set PATH=..\..\..\bin;..\..\..\bin\swig
set SRCINTF=
for %%f in (%API_INCLUDE%/%MODULE%/*.h) do set SRCINTF=!SRCINTF! %API_INCLUDE%/%MODULE%/%%f

echo ========================= MODULE %MODULE% =============================

rem .iファイルを手書きするときはここにモジュール名を追加
if "%MODULE%" == "Base" goto swig
if "%MODULE%" == "Framework" goto swig
if "%MODULE%" == "Physics" goto swig

:makeif
echo Create .i file part
echo #	Do not edit. RunSwig.bat will update this file.> %MODULE%.i
echo #pragma SWIG nowarn=-401-325>> %MODULE%.i
echo %%module %MODULE%>> %MODULE%.i

echo //-- >> %MODULE%.i
echo %include "../../include/base/Env.h">> %MODULE%.i
echo #define PyAPI_FUNC(r)	r			>> %MODULE%.i
echo #define LONG_MAX		0X7FFFFFFFL	>> %MODULE%.i
echo #define UCHAR_MAX		0xFF		>> %MODULE%.i
echo #define NULL			0			>> %MODULE%.i
echo #undef __PYDECL					>> %MODULE%.i
echo #define __PYDECL					>> %MODULE%.i
echo #undef SPR_CDECL					>> %MODULE%.i
echo #define SPR_CDECL					>> %MODULE%.i
echo #undef SPR_DLL						>> %MODULE%.i
echo #define SPR_DLL					>> %MODULE%.i

call :Extend_%MODULE%

echo %%begin%%{>> %MODULE%.i
echo #include "%API_INCLUDE%/Springhead.h">> %MODULE%.i
echo #include "%API_INCLUDE%/Python/Python.h">> %MODULE%.i
echo #include "%API_INCLUDE%/EmbPython/SprEPUtility.h">> %MODULE%.i
echo #include "%API_INCLUDE%/EmbPython/SprEPBase.h">> %MODULE%.i
if not "%9"=="" echo #include "%API_INCLUDE%/EmbPython/SprEP%9.h">> %MODULE%.i
if not "%8"=="" echo #include "%API_INCLUDE%/EmbPython/SprEP%8.h">> %MODULE%.i
if not "%7"=="" echo #include "%API_INCLUDE%/EmbPython/SprEP%7.h">> %MODULE%.i
if not "%6"=="" echo #include "%API_INCLUDE%/EmbPython/SprEP%6.h">> %MODULE%.i
if not "%5"=="" echo #include "%API_INCLUDE%/EmbPython/SprEP%5.h">> %MODULE%.i
if not "%4"=="" echo #include "%API_INCLUDE%/EmbPython/SprEP%4.h">> %MODULE%.i
if not "%3"=="" echo #include "%API_INCLUDE%/EmbPython/SprEP%3.h">> %MODULE%.i
echo %%}>> %MODULE%.i
echo //--  >> %MODULE%.i

for %%p in (%SRCINTF%) do echo %%include "%%p">> %MODULE%.i


:swig
echo Swig Part
call swig.exe -cpperraswarn -sprpy -c++ %MODULE%.i & if errorlevel 1 echo !!!!SWIG FAILED!!!!! & @pause

:astyle
echo AStyle Part
call ..\..\..\bin\AStyle.exe  --style=allman --indent=tab "%CPP%" "%HPP%" "%SPRH%" & if errorlevel 1 @pause
del /Q .\SprEP%MODULE%.*.orig
del /Q .\EP%MODULE%.*.orig

:arrange
echo Arrange Part
set APIHEADER=%API_INCLUDE%\EmbPython\SprEP%MODULE%.h
set HEADER=EP%MODULE%.h

move /Y %SPRH% %APIHEADER%
echo #include "%APIHEADER%" > %MODULE%.tmp
echo #include "%HEADER%" >> %MODULE%.tmp
echo #pragma warning(disable:4244) >> %MODULE%.tmp
type %CPP% >> %MODULE%.tmp

move /Y %MODULE%.tmp %CPP%

nkf -w8 < EP%MODULE%.h   > ..\..\..\src\EmbPython\EP%MODULE%.h
nkf -w8 < EP%MODULE%.cpp > ..\..\..\src\EmbPython\EP%MODULE%.cpp
nkf -w8 < ..\..\include\EmbPython\SprEP%MODULE%.h > ..\..\..\include\EmbPython\SprEP%MODULE%.h

:end
exit /b




rem ================================================================

:Extend_Foundation
echo //--->>%MODULE%.i
echo %%ignore Spr::UTPadding;>> %MODULE%.i
echo %%ignore Spr::UTStringLess;>> %MODULE%.i
echo %%ignore Spr::UTEatWhite;>> %MODULE%.i
echo %%ignore Spr::UTTypeInfo;>> %MODULE%.i
echo %%ignore Spr::UTTypeInfoObjectBase;>> %MODULE%.i
echo %%ignore Spr::UTTimerProvider;>> %MODULE%.i
echo %%ignore Spr::UTTimerIf;>> %MODULE%.i
echo %%ignore Spr::DebugPrintf;>> %MODULE%.i
echo %%ignore Spr::DebugCSV;>> %MODULE%.i
echo //--->>%MODULE%.i
exit /b

:Extend_FileIO
echo //--->>%MODULE%.i
exit /b

:Extend_Collision
echo //--->>%MODULE%.i
echo %%include "Utility/EPObject.i">>%MODULE%.i
echo EXTEND_NEW(PHMaterial)>>%MODULE%.i
echo //--->>%MODULE%.i
exit /b

:Extend_Graphics
echo //--->>%MODULE%.i
echo %%ignore Spr::GRRenderBaseIf::DrawIndexed;>> %MODULE%.i
echo %%ignore Spr::GRRenderBaseIf::DrawArrays;>> %MODULE%.i
echo %%ignore Spr::GRRenderBaseIf::DrawCurve;>> %MODULE%.i
echo %%ignore Spr::GRRenderBaseIf::CreateShader;>> %MODULE%.i
echo %%ignore Spr::GRFrameIf::GetChildren;>> %MODULE%.i
rem ポインタの配列、ポインタのポインタなど未対応なため
echo %%ignore Spr::GRVertexElement::vfP2f;>> %MODULE%.i
echo %%ignore Spr::GRVertexElement::vfP3f;>> %MODULE%.i
echo %%ignore Spr::GRVertexElement::vfC4bP3f;>> %MODULE%.i
echo %%ignore Spr::GRVertexElement::vfN3fP3f;>> %MODULE%.i
echo %%ignore Spr::GRVertexElement::vfC4fN3fP3f;>> %MODULE%.i
echo %%ignore Spr::GRVertexElement::vfT2fP3f;>> %MODULE%.i
echo %%ignore Spr::GRVertexElement::vfT4fP4f;>> %MODULE%.i
echo %%ignore Spr::GRVertexElement::vfT2fC4bP3f;>> %MODULE%.i
echo %%ignore Spr::GRVertexElement::vfT2fN3fP3f;>> %MODULE%.i
echo %%ignore Spr::GRVertexElement::vfT2fC4fN3fP3f;>> %MODULE%.i
echo %%ignore Spr::GRVertexElement::vfT4fC4fN3fP4f;>> %MODULE%.i
echo %%ignore Spr::GRVertexElement::typicalFormats;>> %MODULE%.i
echo %%ignore Spr::GRVertexElement::vfP3fB4f;>> %MODULE%.i
echo %%ignore Spr::GRVertexElement::vfC4bP3fB4f;>> %MODULE%.i
echo %%ignore Spr::GRVertexElement::vfC3fP3fB4f;>> %MODULE%.i
echo %%ignore Spr::GRVertexElement::vfN3fP3fB4f;>> %MODULE%.i
echo %%ignore Spr::GRVertexElement::vfC4fN3fP3fB4f;>> %MODULE%.i
echo %%ignore Spr::GRVertexElement::vfT2fP3fB4f;>> %MODULE%.i
echo %%ignore Spr::GRVertexElement::vfT2fC4bP3fB4f;>> %MODULE%.i
echo %%ignore Spr::GRVertexElement::vfT2fN3fP3fB4f;>> %MODULE%.i
echo %%ignore Spr::GRVertexElement::vfT2fC4fN3fP3fB4f;>> %MODULE%.i
echo %%ignore Spr::GRVertexElement::typicalBlendFormats;>> %MODULE%.i
echo %%ignore Spr::GRMeshFace::indices;>>%MODULE%.i
echo //--->>%MODULE%.i

echo //--->>%MODULE%.i
echo %%include "Utility/EPObject.i">>%MODULE%.i
echo EXTEND_NEW(GRAnimationKey)>>%MODULE%.i
echo EXTEND_NEW(GRCameraDesc)>>%MODULE%.i
echo EXTEND_NEW(GRVertexElement)>>%MODULE%.i
echo EXTEND_NEW(GRKey)>>%MODULE%.i
echo EXTEND_NEW(GRMeshFace)>>%MODULE%.i
echo EXTEND_N_GETS_TO_LIST(Spr::GRFrameIf,NChildren,GetChildren,GRVisualIf)>>%MODULE%.i
echo //--->>%MODULE%.i

exit /b

:Extend_Creature
echo //--->>%MODULE%.i
echo %%include "Utility/EPObject.i">>%MODULE%.i
echo EXTEND_NEW(CRContactInfo)>>%MODULE%.i
echo EXTEND_N_GET_TO_LIST(Spr::CRSdkIf		,NCreatures		,GetCreature	,CRCreatureIf	)>>%MODULE%.i
echo EXTEND_N_GET_TO_LIST(Spr::CRCreatureIf	,NEngines		,GetEngine			,CREngineIf		)>>%MODULE%.i
rem echo EXTEND_N_GET_TO_LIST(Spr::CRCreatureIf	,NBodies		,GetBody		,CRBodyIf		)>>%MODULE%.i  Bodysになってしまう
rem echo EXTEND_N_GET_TO_LIST(Spr::CRIKSolidIf	,NContacts		,GetContactForce	,Vec3f			)>>%MODULE%.i  RuntimeDownCastができない
rem echo EXTEND_N_GET_TO_LIST(Spr::CRIKSolidIf	,NContacts		,GetContactArea		,double			)>>%MODULE%.i newEPdoubleになってしまう
rem echo EXTEND_N_GET_TO_LIST(Spr::CRIKSolidIf	,NContacts		,GetContactPosition	,Vec3f			)>>%MODULE%.i  RuntimeDownCastができない
rem echo EXTEND_N_GET_TO_LIST(Spr::CRIKSolidIf	,NContacts		,GetContactSolid	,PHSolidIf		)>>%MODULE%.i
rem echo EXTEND_N_GET_TO_LIST(Spr::CRIKSolidIf	,NVisibleSolids	,GetVisibleSolid	,PHSolidIf		)>>%MODULE%.i
echo EXTEND_N_GET_TO_LIST(Spr::CRBodyIf		,NBones		,GetBone			,CRBoneIf		)>>%MODULE%.i
echo //--->>%MODULE%.i
exit /b

:Extend_HumanInterface
echo //--->>%MODULE%.i
echo %%ignore Spr::DVKeyMouseIf::GetMousePosition; //参照渡しして値を入れてもらうような関数をどうにかする>> %MODULE%.i
echo %%ignore Spr::HITrackballIf::GetAngle; //同上>> %MODULE%.i
echo %%ignore Spr::HITrackballIf::GetDistanceRange; //同上>> %MODULE%.i
echo %%ignore Spr::HITrackballIf::GetLatitudeRange; //同上>> %MODULE%.i
echo %%ignore Spr::HITrackballIf::GetLongitudeRange; //同上>> %MODULE%.i

echo //--->>%MODULE%.i
echo %%include "Utility/EPObject.i">>%MODULE%.i
echo EXTEND_NEW(HISpidarMotorDesc)>>%MODULE%.i
echo //--->>%MODULE%.i

exit /b

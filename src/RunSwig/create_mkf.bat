@echo off
:: ***********************************************************************************
::  File:
::	create_mkf.bat makefile
::
::	arguments:
::		makefile	makefile名 (=%MAKEFILE%)
::		project		プロジェクト名 (=%PROJECT%)
::		dependencies	依存するプロジェクト名のリスト (カンマ区切り)
::				(=%DEPENDENCIES%)
::
::  Description:
::	RunSwig 実行のための makefile を作成するスクリプト.
::	プロジェクトの依存関係は, 引数 dependencies で指定する.
::	このスクリプトは, 各プロジェクトのビルドを実行するディレクトリで実行され,
::	makefile もそこに作成される.
::
::	関連ファイル
::	  output: %MAKEFILE% (=%1)
::
:: ***********************************************************************************
::  Version:
::	Ver 1.0	  2012/12/27	F.Kanehori
:: ***********************************************************************************
setlocal enabledelayedexpansion
set CWD=%cd%
set DEBUG=0

:: ----------
::  各種定義
:: ----------
:: 引数
::
set ARGC=0
for %%a in ( %* ) do set /a ARGC+=1
if not %ARGC% == 3 (
    echo create_mkf: bad number of arguments.
    exit /b
)
set MAKEFILE=%1
set PROJECT=%2
set DEPENDENCIES=%3
if %DEBUG% == 1 (
    echo -- create_mkf --
    echo.  arg1: MAKEFILE:      [%MAKEFILE%]
    echo.  arg2: PROJECT:       [%PROJECT%]
    echo.  arg3: DEPENDENCIES:  [%DEPENDENCIES%]
)
call :leaf_name %MAKEFILE%
set DISP_MAKEFILE=%LEAF:.tmp=%
if %DEBUG% == 1 (
    echo.  DISP_MAKEFILE:       [%DISP_MAKEFILE%]
)

:: 関連するヘッダファイルがあるディレクトリ
::
:: (build を実行するディレクトリからみた相対パス)
set INCDIR=..\..\include
set SRCDIR=..\..\src
::
:: (makefile に出力するときのパス： '\' → '/' の変換は後でまとめて行なう)
set INCDIROUT=..\..\include
set SRCDIROUT=.

:: 使用するファイル名
::
set MAKEFILE=.\%MAKEFILE%
set STUBFILE=Stub.cpp
set TMPFILE=create_mkf.tmp
if %DEBUG% == 1 echo TMPFILE=%TMPFILE%

::
:: 使用するプログラム
::
set MAKEMGR=make_manager.bat
set SWIG=RunSwig.bat

:: パスの設定
::
set BINDIR=..\..\src\RunSwig
set SWIGPATH=%SRCDIR%\Foundation

:: 常に依存関係にあるファイルの一覧
::
set FIXDEPSINC=%INCDIROUT%\Springhead.h %INCDIROUT%\Base\Env.h %INCDIROUT%\Base\BaseDebug.h
set FIXDEPSSRC=%SRCDIROUT%\..\Foundation\UTTypeDesc.h
set FIXHDRS=%FIXDEPSINC% %FIXDEPSSRC%

:: 依存関係にはないと見做すファイルの一覧
::
set EXCLUDES=

:: -----------------------------------------------------------------------------------
::  ヘッダ情報の収集
:: ------------------
::	依存関係にあるファイルの一覧を、変数 {FIX|INC|SRC}HDRS に集める.
::	毎回ディレクトリのリストをとるので、新規のファイルが追加されても大丈夫.
::	サブルーチン parse は、依存関係にあるプロジェクトについて処理を行なう.
::	※ サブルーチン :add_headers の結果は環境変数 STR1 に、:add_prefix の結果は
::	   環境変数 STR2 に設定される.
:: -----------------------------------------------------------------------------------
if %DEBUG% == 1 echo FIXHDRS [%FIXHDRS%]

set INCHDRS=
for %%d in (%INCDIR%\%PROJECT%\*.h) do (
    call :add_headers !INCHDRS! %%d
    set INCHDRS=!STR1!
)
call :add_prefix %INCDIROUT%\%PROJECT%
set INCHDRS=%STR2%
if %DEPENDENCIES% neq "" (
    call :parse %INCDIR% %DEPENDENCIES% %TMPFILE% "init"
    for /f %%e in (%TMPFILE%) do (
        set INCHDRS=!INCHDRS! %%e
    )
    del %TMPFILE% 2> nul
)
if %DEBUG% == 1 echo INCHDRS [%INCHDRS%]

set SRCHDRS=
for %%d in (%SRCDIR%\%PROJECT%\*.h) do (
    call :add_headers !SRCHDRS! %%d
    set SRCHDRS=!STR1!
)
call :add_prefix %SRCDIROUT%
set SRCHDRS=%STR2%
if %DEPENDENCIES% neq "" (
    call :parse %SRCDIR% %DEPENDENCIES% %TMPFILE% "init"
    for /f %%e in (%TMPFILE%) do (
        set SRCHDRS=!SRCHDRS! %%e
    )
    del %TMPFILE% 2> nul
)
if %DEBUG% == 1 echo SRCHDRS [%SRCHDRS%]

:: ------------------
::  makefile の作成
:: ------------------
echo     making %DISP_MAKEFILE%
echo # THIS FILE IS AUTO-GENERATED. ** DO NOT EDIT THIS FILE ** > %MAKEFILE%
echo # File: %DISP_MAKEFILE%	>> %MAKEFILE%
echo.				>> %MAKEFILE%
echo FIXHDRS=\>> %MAKEFILE%
for %%h in (%FIXHDRS%) do (
    set MSPATH=%%h
    echo !MSPATH:\=/! \>> %MAKEFILE%
)
echo.  			>> %MAKEFILE%
echo INCHDRS=\>> %MAKEFILE%
for %%h in (%INCHDRS%) do (
    set MSPATH=%%h
    echo !MSPATH:\=/! \>> %MAKEFILE%
)
echo.  			>> %MAKEFILE%
echo SRCHDRS=\>> %MAKEFILE%
for %%h in (%SRCHDRS%) do (
    set MSPATH=%%h
    echo !MSPATH:\=/! \>> %MAKEFILE%
)
echo.  			>> %MAKEFILE%

echo all:	%PROJECT%%STUBFILE%	>> %MAKEFILE%
echo.  			>> %MAKEFILE%

echo %PROJECT%%STUBFILE%:	$^(FIXHDRS^) $^(INCHDRS^) $^(SRCHDRS^)	>> %MAKEFILE%
set SWIGARGS=%PROJECT%
if %DEPENDENCIES% neq "" (
    set SWIGOPTS=%DEPENDENCIES:"=%
    set SWIGARGS=!SWIGARGS! !SWIGOPTS:,= !
)
echo.	%BINDIR%\%MAKEMGR% -t	>> %MAKEFILE%
echo.	%SWIGPATH%\%SWIG% %SWIGARGS%	>> %MAKEFILE%
echo.  			>> %MAKEFILE%

echo $^(FIXHDRS^):		>> %MAKEFILE%
echo.  			>> %MAKEFILE%
echo $^(INCHDRS^):		>> %MAKEFILE%
echo.  			>> %MAKEFILE%
echo $^(SRCHDRS^):		>> %MAKEFILE%

:: ----------
::  処理終了
:: ----------
endlocal
exit /b

:: -----------------------------------------------------------------------------------
::  変数 *HDRS にファイル名を追加する (デリミタは空白文字).
::
::  引数 %1	変数 *HDRS の現在値
::	 %2	*HDRS に追加するファイル名
:: -----------------------------------------------------------------------------------
:add_headers
set STR1=
:loop
    if "%1" equ "" goto :endloop
    for %%f in (%EXCLUDES%) do if %1 equ %%f goto :next
    if "!STR1!" equ "" (
	set STR1=%1
    ) else (
	set STR1=!STR1! %1
    )
:next
    shift /1
    goto :loop
:endloop
exit /b

:: -----------------------------------------------------------------------------------
::  引数で与えられたカンマ区切りの文字列を解析し、要素(＝依存するプロジェクト名)毎に
::  サブルーチン dirlist を呼び出してヘッダ情報の収集を行なう.
::
::  引数 %1	走査するディレクトリのベースパス
::	 %2	依存するプロジェクトのリスト (カンマ区切りのリスト)
::	 %3	作業用ファイル名
::	 %4	作業用ファイル初期化フラグ (何かが設定されていたら初期化を行なう)
:: -----------------------------------------------------------------------------------
:parse
setlocal
set LIST=%2
set LIST=%LIST:"=%
for /f "delims=, tokens=1,*" %%a in ("%LIST%") do (
    if "%4" neq "" del %3 2> nul
    if "%%a" neq "" call :dirlist %1 %%a %3
    if "%%b" neq "" call :parse %1 "%%b" %3
)
endlocal
exit /b

:: -----------------------------------------------------------------------------------
::  引数で与えられたプロジェクト<p>について、include/<p> ディレクトリに存在するヘッダ
::  ファイルをリストアップして作業ファイルに書き出す (1ファイル/1行). 
::
::  引数 %1	走査するディレクトリのベースパス
::	 %2	プロジェクト名
::	 %3	作業用ファイル名
:: -----------------------------------------------------------------------------------
:dirlist
set DIR=%2
set DIR=%DIR:"=%
call :leaf_name %1
echo     dependent module: [%LEAF%/%DIR%]
for %%d in (%1\%DIR%\*.h) do (
    call :leaf_name %%d
    rem echo output: [%1\%DIR%\!LEAF!]
    echo %1\%DIR%\!LEAF! >> %3
)
exit /b

:: -----------------------------------------------------------------------------------
::  引数で与えられたパス名からリーフ名を取り出す.
::  結果は、環境変数 LEAF に設定される.
::
::  引数 %1	パス名
:: -----------------------------------------------------------------------------------
:leaf_name
    set LEAF=%~nx1
exit /b

:: -----------------------------------------------------------------------------------
::  変数 STR1 に設定されている各ファイル名に、引数で指定された prefix を追加する.
::  結果は、環境変数 STR2 に設定される.
::
::  引数 %1	追加するプリフィックス
:: -----------------------------------------------------------------------------------
:add_prefix
set STR2=
for %%f in (!STR1!) do (
    call :leaf_name %%f
    if "!STR2!" equ "" (
	set STR2=%1\!LEAF!
    ) else (
	set STR2=!STR2! %1\!LEAF!
    )
)
exit /b

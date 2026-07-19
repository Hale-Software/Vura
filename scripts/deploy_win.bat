@echo off
REM Set paths matching your local environment installation
SET QT_BIN=C:\Qt\6.11.1\mingw_64\bin
SET MINGW_BIN=C:\Qt\Tools\mingw1120_64\bin
SET FFMPEG_BIN=C:\Users\halea\bin\ffmpeg-8.1.1-full_build-shared\bin
SET FFMPEG_DIR=C:\Users\halea\bin\ffmpeg-8.1.1-full_build-shared

SET OUTPUT_DIR=..\build\deploy
SET DEPLOY_DIR=..\build\deploy\dist_windows

mkdir %DEPLOY_DIR%
copy ..\build\release\src\vura.exe %DEPLOY_DIR%\

copy ..\assets\icons\vura.ico %OUTPUT_DIR%\
copy ..\assets\data\license.txt %OUTPUT_DIR%\

xcopy ..\assets %DEPLOY_DIR%\assets /E /I

REM 1. Run Qt deployment tool to fetch plugins, platforms, and Qt DLLs
%QT_BIN%\windeployqt.exe --dir %DEPLOY_DIR% %DEPLOY_DIR%\vura.exe

REM 2. Manually copy required MinGW runtime compiler DLLs
copy %MINGW_BIN%\libgcc_s_seh-1.dll %DEPLOY_DIR%\
copy %MINGW_BIN%\libstdc++-6.dll %DEPLOY_DIR%\
copy %MINGW_BIN%\libwinpthread-1.dll %DEPLOY_DIR%\

REM 3. Copy your FFmpeg shared library components
copy %FFMPEG_BIN%\avcodec-*.dll %DEPLOY_DIR%\
copy %FFMPEG_BIN%\avformat-*.dll %DEPLOY_DIR%\
copy %FFMPEG_BIN%\avutil-*.dll %DEPLOY_DIR%\
copy %FFMPEG_BIN%\swscale-*.dll %DEPLOY_DIR%\

echo Deployment package built in %DEPLOY_DIR%

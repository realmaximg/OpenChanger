@echo off
setlocal
if "%~1"=="" (
    echo usage: tools\build.cmd ^<dev^|release^|lint^>
    exit /b 2
)

set "VSWHERE=%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe"
if not exist "%VSWHERE%" (
    echo vswhere.exe not found: install Visual Studio Build Tools 2022
    exit /b 1
)
set "VSDIR="
for /f "usebackq delims=" %%i in (`"%VSWHERE%" -version [17.0^,18.0^) -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath`) do set "VSDIR=%%i"
if not defined VSDIR (
    echo Visual Studio Build Tools 2022 with the C++ x64 tools not found
    exit /b 1
)

call "%VSDIR%\VC\Auxiliary\Build\vcvars64.bat" >nul || exit /b 1
set "PATH=C:\Program Files\LLVM\bin;%PATH%"

cd /d "%~dp0.."
if /i "%~1"=="lint" goto lint
if /i "%~1"=="release" call :frontend || exit /b 1
call :protobuf %1 || exit /b 1
cmake --preset %1 "-DOC_PROTOBUF_PREFIX=%OC_PB%" || exit /b 1
cmake --build --preset %1 || exit /b 1
exit /b 0

:lint
call :protobuf dev || exit /b 1
cmake --preset dev "-DOC_PROTOBUF_PREFIX=%OC_PB%" >nul || exit /b 1
cmake --build --preset dev --target oc_proto_generate || exit /b 1
set "FINDINGS="
for /f "delims=" %%f in ('dir /s /b src\*.cpp src\*.h') do (
    clang-format --dry-run --Werror "%%f" || set "FINDINGS=1"
)
for /f "delims=" %%f in ('dir /s /b src\*.cpp') do (
    clang-tidy -p build\dev --quiet "%%f" || set "FINDINGS=1"
)
if defined FINDINGS (
    echo lint: findings above
    exit /b 1
)
echo lint: 0 findings
exit /b 0

:frontend
call :protobuf dev || exit /b 1
cmake --preset dev "-DOC_PROTOBUF_PREFIX=%OC_PB%" >nul || exit /b 1
cmake --build --preset dev --target oc_api_types_file || exit /b 1
pushd src\ui\panorama
if not exist node_modules call npm ci || (popd & exit /b 1)
call npm run build:release || (popd & exit /b 1)
popd
exit /b 0

:protobuf
set "OC_DEV=OFF"
if /i "%~1"=="dev" set "OC_DEV=ON"
set "OC_PB=%LOCALAPPDATA%\openchanger\protobuf-3.21.8-%~1"
if exist "%OC_PB%\bin\protoc.exe" exit /b 0
cmake -S cmake\protobuf -B "%OC_PB%-build" -G Ninja -DCMAKE_BUILD_TYPE=Release -DCMAKE_C_COMPILER=clang-cl -DCMAKE_CXX_COMPILER=clang-cl -DCMAKE_LINKER=lld-link -DCMAKE_AR=llvm-lib -DOPENCHANGER_DEV=%OC_DEV% "-DCMAKE_INSTALL_PREFIX=%OC_PB%" || exit /b 1
cmake --build "%OC_PB%-build" --target install || exit /b 1
exit /b 0

@echo OFF

:: name of zip archive on local hard disk
set archive=mingw-11.3-win32-64.zip

:: url of zip archive to download
set url="https://artifactory.intra.infineon.com/artifactory/gen-pmm-hmi-local/mingw/%archive%"

:: directory of this script
set wd=%~dp0

:: path to 7zip
set zip="%wd%\7-Zip\7z.exe"

:: flag indicating if a symlink should be created (default: false)
set create_symlink=n

:: print usage
if "%1"=="-h" (
    echo install_mingw.bat [-d DIRECTORY]
    echo.
    echo Download and extract MinGW distribution
    echo.
    echo Options:
    echo   -d DIRECTORY: install MinGW distribution to DIRECTORY\mingw64
)

:: parsing arguments
if "%1"=="-d" (
    :: install MinGW distribution to directory and create symlink
    set create_symlink=y
    set directory=%2
) ELSE (
    set directory=%~dp0
)

if exist %wd%\mingw64 (
    echo MinGW already installed.
    exit /B 0
)

if not exist %directory%\mingw64 (
    mkdir %directory%

    if not exist %directory%\%archive% (
        :: download zip archive
        echo Downloading %archive%.
        powershell -Command "(New-Object Net.WebClient).DownloadFile('%url%', '%directory%\%archive%')"
    )

    :: extract zip archive
    echo Extracting %archive%.
    cd %directory%
    %zip% -bd -bse1 -y x %archive% > nul
)

:: if needed create symlink
if %create_symlink%==y (
    mklink /J %wd%\mingw64 %directory%\mingw64
)
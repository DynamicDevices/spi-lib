@echo OFF
cd %~dp0

set archive=python-3.7.7-embed-amd64.zip
set url="https://artifactory.intra.infineon.com/artifactory/gen-pmm-hmi-local/tools/python/%archive%"

if exist python ( 
	echo python already installed. Aborting.
) ELSE (
	if not exist %archive% (
        echo Downloading %archive%.
        powershell -Command "(New-Object Net.WebClient).DownloadFile('%url%', '%archive%')"
	)
    echo Extracting %archive%.
    7-Zip\7z.exe -bd -bse1 -y x %archive% > nul
)

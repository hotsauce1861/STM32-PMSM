echo %cd%
set ROOT_PATH=.\..\..
set OUT_NAME=firmware.bin

set SRC=%ROOT_PATH%\..\Project\MDK\ARMCC\Out\obj\*.hex
set TOOL=%ROOT_PATH%\..\Bin\hex2bin.exe
set DST=%ROOT_PATH%\..\Firmware\%OUT_NAME%

if exist %SRC% (%TOOL% %SRC%)
exit
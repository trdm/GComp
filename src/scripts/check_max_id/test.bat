@echo off

gcomp -d --filter ИдентификаторыКонфигурации
perl check_max_id.pl
if errorlevel 1 goto Error

echo Можно коммитить
gcomp -d -v| perl c:\scripts\auto_add.pl
goto End


:Error
echo Нельзя коммитить

:End

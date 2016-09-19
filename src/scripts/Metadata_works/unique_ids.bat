@echo off

perl unique_ids.pl
if errorlevel 1 goto err
echo Всё зашибись
goto end

:err
echo Есть неуникальные идентификаторы!

:end
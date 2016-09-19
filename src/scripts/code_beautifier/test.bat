
rem напечатаем help, и выведем его в файл correct_dlg.txt, на случай нечитабельности.
perl code_beautifier.pl -h 2>code_beautifier.txt
perl code_beautifier.pl -h


rem Отформатируем все модули в каталоге SRC. Отформатированые модули положатся в файлы *.new.1S
rem вместо SRC подставьте свой каталог.
perl code_beautifier.pl -d SRC -k
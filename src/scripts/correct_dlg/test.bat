
rem напечатаем help, и выведем его в файл correct_dlg.txt, на случай нечитабельности.
perl correct_dlg.pl -h >correct_dlg.txt
perl correct_dlg.pl -h


rem Исправим все диалоги в каталоге SRC. Исправления положатся в файлы *.new.frm
rem вместо SRC подставьте свой каталог.
perl correct_dlg.pl -d SRC -k
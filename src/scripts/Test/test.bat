gcomp.old.exe -d -F 1cv7.md -D OLD
gcomp.exe -d -F 1cv7.md -D TMP
gcomp.exe -c -D TMP -F 1cv7.new.md
gcomp.old.exe -d -F 1cv7.new.md -D NEW
"D:\Program Files\Kdiff3\kdiff3.exe" OLD NEW

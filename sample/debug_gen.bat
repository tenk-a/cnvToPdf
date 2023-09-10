chcp 65001
..\build\debug\cnvToPdf.exe -otmp_テスト0.pdf -t="タイトル♠" -a="著者" data/*.jpg
..\build\debug\cnvToPdf.exe -t="タイトル1♥" -a="著者" data/*.jpg
..\build\debug\cnvToPdf.exe -T="[著者] タイトル2" data/*.jpg
..\build\debug\cnvToPdf.exe -T="著者 - タイトル3" data/*.jpg
..\build\debug\cnvToPdf.exe -D- -a="著者" -t="タイトル1-D-" data/*.jpg

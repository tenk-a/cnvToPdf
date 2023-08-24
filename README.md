# cnvToPdf v1.0

複数 jpg 画像をpdf化


## 機能

- 複数 jpg 画像をpdf化
- 左右綴じ指定
- 見開き指定
- タイトルと著者名の指定

## 使い方

usage > jpg2pdf.exe [オプション] jpgファイル...  
  1枚以上の指定されたjpg画像をファイル名でソートして、１つのpdfファイルを生成.  
  ソートはファイル名の数値部分は数値同士で比較（例えば img_2.jpg, img_10.jpg 順になる）

オプション:
  -o  --output="FILE.pdf"               出力するpdfファイル名  
                                        指定無なら最初のjpgのディレクトリ名を -T で採用  
  -d  --out-dir="DIR"                   -o 指定のない場合の出力ディレクトリ名  
  -D  --digit[-]                        ファイル名の数字列を数値として比較(*)  [- しない]  
  -r  --r2l                             右綴じ指定  
  -r- --l2r                             左綴じ指定(*)  
      --outline                         アウトライン(頁番号一覧)を付加  
  -t  --title="タイトル"                タイトル指定  
  -a  --author="著者"        著者指定  
  -T  --author-title="[著者] タイトル"  著者を[]で囲んで著者とタイトルを指定  
  -T  --author-title="著者 - タイトル"  ' - 'を挟んで著者とタイトルを指定  
  -mN                                  頁の開き方指定(0～5).下記6つに同じ  
      --page-layout-single             0:単頁(*)  
      --page-layout-one-column         1:単頁スクロール  
      --page-layout-two-column-left    2:見開きスクロール両頁開始  
      --page-layout-two-column-right   3:見開きスクロール片頁開始  
      --page-layout-two-page-left      4:見開き両頁開始  
      --page-layout-two-page-right     5:見開き片頁開始  

  ※ (*)付はデフォルトで指定済  
  ※ オプションとオプション引数の間に空白は入れられない。-dHOGE のように空白無で指定のこと.  
  

## 著者名、タイトル名 について

著者名、タイトル名 指定は pdf 内に設定するが出力ファイル名にも用いる。(-o指定無時)  

もともと個人的には "著者名 - タイトル名" というファイル名の付け方をしていたのだが、
iPad,iPhoneでよく使ってる i文庫(S,HD) が、pdf内の情報は見ずにファイル名が  
  "[著者名] タイトル名" (や "タイトル名[著者名]")  
のような場合、[]内を著者名、それ以外をタイトル名として扱っているようなので
それに倣い、先頭に [著者名] があればそれを著者名とした。

-o 指定無で -a -t で指定した場合の出力ファイル名は "[著者名] タイトル名" となる。
-T の場合は、-T の指定のままがファイル名となる。

タイトルや著者名の文字コードは、UNICODE を用いている。

mac,linux の os 環境の文字コードは UTF-8 であることを前提に動作する。
windows 環境も UNICODE だが、出力ファイル名については OSの文字コード(SJIS)
の制限を受ける。


## ソースコンパイル

vcpkg ＆ cmake を用いてビルドする。
vcpkg は 環境変数 VCPKG_ROOT (と VCPKG_DEFAULT_TRIPLET) を設定のこと。

プロジェクト・ディレクトリ下に build ディレクトリを作る
 mkdir build

build/ ディレクトリをカレントにして  
 cmake ..  
 make  
や
 cmake ..  
 msbuild cnvToPdf.sln  
等を行いビルド。


## 履歴
2016/02/28 v0.1  公開  
2016/02/29 v0.1b  libharuのビルドで確認も兼ねて demo もビルドしていたが、
その中の jpfont_demo.exe の実行でエラー終了してしまう。jpfont_demo.c 中のフォント名
MS-Mincyo が原因で、全てMS-Minchoに置換すれば直るが、主目的でないので demo を
ビルドしないようにバッチ修正.  
2016/08/25 v0.1c  ファイル名比較が大文字小文字別になっていたのを同一視するように修正  

2023/08/20 v1.0   自前のライブラリビルドをやめ vcpkg ＆ cmake でビルドするように変更。
                  タイトルや著者の文字コードを UNICODE(utf-8)に変更。

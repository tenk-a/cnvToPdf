# cnvToPdf v0.1c

複数 jpg 画像をpdf化


## 機能

- 複数 jpg 画像をpdf化.
- 右綴じ指定可能
- 見開き指定可能
- タイトルと著者名の指定可能

## 使い方

usage > jpg2pdf.exe [オプション] jpgファイル...  
  1枚以上の指定されたjpg画像をファイル名でソートして、１つのpdfファイルを生成.  
  ソートはファイル名の数値部分は数値同士で比較（例えば img_2.jpg, img_10.jpg 順になる）

オプション:
  -o  --output"FILE.pdf"               出力するpdfファイル名  
                                       指定無なら最初のjpgのディレクトリ名を-Tで採用  
  -d  --out-dir"DIR"                   --out指定のない場合の出力ディレクトリ名  
      --digit[-]                       ファイル名の数字列を数値として比較(*)  [- しない]  
  -r  --r2l                            右綴じ指定  
  -r- --l2r                            左綴じ指定(*)  
      --outline                        アウトライン(頁番号一覧)を付加  
  -t  --title"タイトル"                タイトル指定  
  -a  --author"著者"        著者指定  
  -T  --author-title"[著者] タイトル"  著者を[]で囲んで著者とタイトルを指定  
  -T  --author-title"著者 - タイトル"  ' - 'を挟んで著者とタイトルを指定  
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
(後ろ[]は個人的に著者名以外で使うことがあるので未サポート)

-o指定無で -a -t で指定した場合の出力ファイル名は "[著者名] タイトル名" となる。
-T の場合は、-T の指定のままがファイル名となる。

※ タイトルや著者名の文字は、SJIS範囲に限られる. UNICODE未対応.


## ソースコンパイル

コンパイルするには、このソースの他に libharu, zlib, libpng のソースが必要.  
それらのソースをダウンロードしてきてソース環境の  
    libs_vc/  
フォルダ内に  
  libharu-RELEASE_2_3_0/  
  lpng1621/  
  zlib-1.2.8/  
のように配置。(上記は実際に使用したバージョン。多少前後のバージョンでも大丈夫なはず)  

libs_vc/bld_lib_bat/libs_config.bat をテキストエディタで編集、  
  set CcName=vc90  
のように使用するvcのバージョンを指定。(vc80,vc90,vc100,vc120,vc140) 
libs_vc/bld_lib_bat フォルダをカレントにして  
  bld.bat  
を実行。misc_inc, misc_lib にヘッダとlibが作られる。  

使用するvcに合わせて いずれかの lib_vc???/ の sln を開いてコンパイル。  
該当外のバージョンなら適当にコンバートのこと。  

基本的にstaticランタイムでx64設定あり。ただし vc8exp は express版を考慮して
dllランタイム使用でx64無にしてある。  
express版の場合は bld.bat 実行前に libs_config.bat を  
  set CcHasX64=0  
  set CcNoRtStatic=1  
に書き換える必要あり。  
(が実際にexpress版でコンパイルしたわけでないので注意.ダメならゴメン)  

※ VCのバージョン分けに、Visual Studio のマクロ変数 PlatformToolsetVersion を
用いている。これは vc10(2010)以降にはあるが、vc9(2008)以前には無いので
それらでは macro.vsprops というのを追加し、その中で PlatformToolsetVersion を
設定するようにしている。

## 履歴
2016/02/28 v0.1  公開
2016/02/29 v0.1b  libharuのビルドで確認も兼ねて demo もビルドしていたが、
その中の jpfont_demo.exe の実行でエラー終了してしまう。jpfont_demo.c 中のフォント名
MS-Mincyo が原因で、全てMS-Minchoに置換すれば直るが、主目的でないので demo を
ビルドしないようにバッチ修正.
2016/08/25 v0.1c  ファイル名比較が大文字小文字別になっていたのを同一視するように修正

# cnvToPdf
複数 jpg 画像をpdf化

## 機能
- 複数 jpg 画像をpdf化.
- 右綴じ指定可能
- 見開き指定可能
- タイトルと著者名の指定可能

## 使い方

> jpg2pdf.exe [オプション] jpgファイル...

>>>
  -o  --output"FILE.pdf"               出力するpdfファイル名
                                       指定無なら最初のjpgのディレクトリ名を-Tで採用
  -d  --out-dir"DIR"                   --out指定のない場合の出力ディレクトリ名
      --digit[-]                       ファイル名の数字列を数値として比較(*)  [- しない]
  -r  --r2l                            右綴じ指定
  -r- --l2r                            左綴じ指定(*)
      --outline                        アウトライン(頁番号一覧)を付加
  -t  --title"タイトル"                タイトル指定
  -a  --author"著者"        著者指定
  -T  --author-title"著者 - タイトル"  ' - 'を挟んで著者とタイトルを指定
  -T  --author-title"[著者] タイトル"  著者を[]で囲んで著者とタイトルを指定
  -mN                                  頁の開き方指定(0～5).下記6つに同じ
      --page-layout-single             0:単頁(*)
      --page-layout-one-column         1:単頁スクロール
      --page-layout-two-column-left    2:見開きスクロール両頁開始
      --page-layout-two-column-right   3:見開きスクロール片頁開始
      --page-layout-two-page-left      4:見開き両頁開始
      --page-layout-two-page-right     5:見開き片頁開始
  (*)付はデフォルト指定済
<<<




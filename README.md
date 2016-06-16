# 質問は  
 J1618 鈴木豪 までSlackでDMで教えてください．  
# 問題提起について  
 Issuesから議論しましょう．  
# リポジトリの構成
 とりあえず，一人一つブランチを作り，そこに個々の成果をコミットすることにします．
# ソリューションを置く前に
 wikiにそのプロジェクト・ソリューションについて以下のことを明記して概要を書いておくと良いです．（強制ではない） 
 * VisualStudio/コンパイラのバージョン
 * 必要ライブラリ(OpenCV, OpenMP, OpenGL, Vulkan, DirectX, WindowsSDK, CUDA, Mantle, AMPなどの特殊なもの)
 * 説明 
 
# 大まかな流れ
### はじめ
#### git clone https://github.com/mct-procon/procon2016
 これでリポジトリをクローン．
#### git config --local user.name "自分の名前"
#### git config --local user.email "自分のE-mail"
 自分のユーザー設定を設定  
#### git branch ブランチの名前
 これで，ローカルにブランチつくり，
#### git checkout ブランチの名前
 ブランチ変更  
#### git push origin ブランチの名前
 リモートリポジトリにブランチ作成  
### アップロード
#### git add *
 変更内容をまとめて
#### git commit -a -m "コメント"
 変更内容をコミットし，
#### git push origin ブランチ名
 リモートリポジトリに反映する．
# 基本的なコマンド
### [最初のみ] git clone https://github.com/mct-procon/procon2016
 リポジトリをローカルにダウンロードする． 
## BRANCH
### git branch ブランチの名前
 ブランチを作成します． 
### git checkout ブランチの名前
 ブランチを移動します． 
### git branch
 ローカルのブランチの一覧を表示します． 
### git branch -d ブランチの名前
 ブランチを削除します． 
### git branch -r
 リモートのブランチの一覧を表示します．
## COMMIT
### git add *
 編集・追加内容をステージングにあげます．
### git commit -a -m "コメント"
 コミットします．
### git push　(たいていはあとのように) origin ブランチ名
 リモートリポジトリに変更内容(コミットたち)を送ります． 
## UPDATE
### git pull　(たいていはあとのように) origin ブランチ名
 リモートリポジトリから変更内容(コミットたち)を受け取り，ローカルリポジトリに反映します．

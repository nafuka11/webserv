# \*Configクラスの実装をテスト
```
$ make
$ ./a.out
```

## このディレクトリの扱い
必要なくなり次第、このディレクトリは消去する。

## conf_main.cpp
３つのクラス
- ``Config``クラス
- ``ServerConfig``クラス
- ``LocationConfig``クラス
このメンバ関数が動作しているかを確認する。

確認は、各クラスのメンバ変数（各ディレクティブのデータ）を出力することで行っています。

## Makefile
- ``conf_main.cpp``
- ``Config.cpp``
- ``ServerConfig.cpp``
- ``LocationConfig.cpp``
をコンパイルする。

# webserv

## 課題概要

NginxのようなHTTP/1.1に準拠したサーバをC++98で作る課題です。

## 使い方

### ダウンロード & ビルド

```bash
git clone https://github.com/nafuka11/webserv.git
cd webserv
make
```

### プログラムの起動

```bash
./webserv conf/default.conf
```

## テスト

### テストに必要なパッケージのインストール

```bash
cd tests
make install
```

### すべてのテスト

```bash
make
```

### 設定ファイルのテスト

```bash
make conf
```

### サーバのテスト

```bash
make http
```

## その他

docs/cgi/tohoho.pl は、[とほほのCGI入門](https://www.tohoho-web.com/wwwcgi.htm) の [CGIスクリプトのテンプレート](https://www.tohoho-web.com/wwwcgi4.htm) よりお借りしました。

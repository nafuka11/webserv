#!/usr/local/bin/perl
# 上の１行の前には空行も空白文字もはいらないようにしてください。
# perlのパス名はプロバイダや環境に合わせて変更してください。

#
# HTMLのメタ文字をエスケープする関数を用意します
#
sub html {
    $str = $_[0];
    $str =~ s/&/&amp;/g;
    $str =~ s/</&lt;/g;
    $str =~ s/>/&gt;/g;
    $str =~ s/"/&quot;/g;
    $str =~ s/'/&#x27;/g;
    return $str;
}

#
# ヘッダ部を書き出します
#
print <<EOF;
Content-Type: text/html

<!doctype html>
<html>
<head>
<meta charset="utf-8">
<title>CGI TEST</title>
</head>
<body>
<h1>CGI TEST</h1>
<pre>
EOF

#
# コマンド引数を書き出します
#
print "=================================\n";
print "コマンド引数\n";
print "=================================\n";
for ($i = 0; $i <= $#ARGV; $i++) {
	print "ARGV[$i] = [ " . html($ARGV[$i]) . " ]\n";
}
print "\n";

#
# CGIスクリプトが参照可能な環境変数を書き出します。
#
print "=================================\n";
print "環境変数\n";
print "=================================\n";
print "AUTH_TYPE = [ $ENV{'AUTH_TYPE'} ]\n";
print "CONTENT_LENGTH = [ $ENV{'CONTENT_LENGTH'} ]\n";
print "CONTENT_TYPE = [ $ENV{'CONTENT_TYPE'} ]\n";
print "GATEWAY_INTERFACE = [ $ENV{'GATEWAY_INTERFACE'} ]\n";
print "PATH_INFO = [ $ENV{'PATH_INFO'} ]\n";
print "PATH_TRANSLATED = [ $ENV{'PATH_TRANSLATED'} ]\n";
print "QUERY_STRING = [ $ENV{'QUERY_STRING'} ]\n";
print "REMOTE_ADDR = [ $ENV{'REMOTE_ADDR'} ]\n";
print "REMOTE_HOST = [ $ENV{'REMOTE_HOST'} ]\n";
print "REMOTE_IDENT = [ $ENV{'REMOTE_IDENT'} ]\n";
print "REMOTE_USER = [ $ENV{'REMOTE_USER'} ]\n";
print "REQUEST_METHOD = [ $ENV{'REQUEST_METHOD'} ]\n";
print "SCRIPT_NAME = [ $ENV{'SCRIPT_NAME'} ]\n";
print "SERVER_NAME = [ $ENV{'SERVER_NAME'} ]\n";
print "SERVER_PORT = [ $ENV{'SERVER_PORT'} ]\n";
print "SERVER_PROTOCOL = [ $ENV{'SERVER_PROTOCOL'} ]\n";
print "SERVER_SOFTWARE = [ $ENV{'SERVER_SOFTWARE'} ]\n";
print "\n";

#
# フォームに指定した値を書き出します
#
print "=================================\n";
print "フォーム変数\n";
print "=================================\n";
if ($ENV{'REQUEST_METHOD'} eq "POST") {
	# POSTであれば標準入力から読込みます
	read(STDIN, $query_string, $ENV{'CONTENT_LENGTH'});
} else {
	# GETであれば環境変数から読込みます
	$query_string = $ENV{'QUERY_STRING'};
}
# 「変数名1=値1&変数名2=値2」の形式をアンパサンド( & )で分解します
@a = split(/&/, $query_string);
# それぞれの「変数名=値」について
foreach $a (@a) {
	# イコール( = )で分解します
	($name, $value) = split(/=/, $a);
	# + や %8A などをデコードします
	$value =~ tr/+/ /;
	$value =~ s/%([0-9a-fA-F][0-9a-fA-F])/pack("C", hex($1))/eg;
	# 変数名と値を書き出します
	print "$name = [ " . html($value) . " ]\n";
	# 後で使用する場合は、$FORM{'変数名'} に代入しておきます
	$FORM{$name} = $value;
}

#
# フッター部を書き出します
#
print <<EOF;
</pre>
</body>
</html>
EOF
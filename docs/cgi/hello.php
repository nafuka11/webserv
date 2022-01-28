<?php
echo "Content-type: text/html\n";
echo "\n";
echo "Hello, World!\n";
echo 'AUTH_TYPE = ' .$_ENV["AUTH_TYPE"]."\n";
echo 'CONTENT_LENGTH = ' .$_ENV["CONTENT_LENGTH"]."\n";
echo 'CONTENT_TYPE = ' .$_ENV["CONTENT_TYPE"]."\n";
echo 'GATEWAY_INTERFACE = ' .$_ENV["GATEWAY_INTERFACE"]."\n";
echo 'PATH_INFO = ' .$_ENV["PATH_INFO"]."\n";
echo 'PATH_TRANSLATED = ' .$_ENV["PATH_TRANSLATED"]."\n";
echo 'QUERY_STRING = ' .$_ENV["QUERY_STRING"]."\n";
echo 'REMOTE_ADDR = ' .$_ENV["REMOTE_ADDR"]."\n";
echo 'REMOTE_HOST = ' .$_ENV["REMOTE_HOST"]."\n";
echo 'REMOTE_IDENT = ' .$_ENV["REMOTE_IDENT"]."\n";
echo 'REMOTE_USER = ' .$_ENV["REMOTE_USER"]."\n";
echo 'REQUEST_METHOD = ' .$_ENV["REQUEST_METHOD"]."\n";
echo 'REQUEST_URI = ' .$_ENV["REQUEST_URI"]."\n";
echo 'SCRIPT_NAME = ' .$_ENV["SCRIPT_NAME"]."\n";
echo 'SERVER_NAME = ' .$_ENV["SERVER_NAME"]."\n";
echo 'SERVER_PORT = ' .$_ENV["SERVER_PORT"]."\n";
echo 'SERVER_PROTOCOL = ' .$_ENV["SERVER_PROTOCOL"]."\n";
echo 'SERVER_SOFTWARE = ' .$_ENV["SERVER_SOFTWARE"]."\n";
?>

import re
from http import HTTPStatus
from http.client import HTTPConnection
from pathlib import Path

from helper import HTML_PATH_OK, assert_response


class TestGet:
    def test_valid(self, http_connection: HTTPConnection):
        """正常なGETリクエストで200を返すこと"""
        http_connection.request("GET", "/")
        response = http_connection.getresponse()
        assert_response(HTTPStatus.OK, response, HTML_PATH_OK)

    def test_404_no_such_file(self, http_connection: HTTPConnection):
        """URIのファイルが存在しない時、404を返すこと"""
        http_connection.request("GET", "/nosuchfile")
        response = http_connection.getresponse()
        assert_response(HTTPStatus.NOT_FOUND, response)

    def test_autoindex_on(self, http_connection: HTTPConnection):
        """autoindex onのディレクトリで各ファイル情報のHTMLが返されること"""
        http_connection.request("GET", "/autoindex/")
        response = http_connection.getresponse()
        actual_body = response.read().decode("utf-8")
        expected_files = [
            r'<a href="subdir/">subdir/</a>                                            \d{2}-\w+-\d{4} \d{2}:\d{2} +-',
            r'<a href="50_name_len_directory_2345678901234567890123456789/">50_name_len_directory_2345678901234567890123456\.\.&gt;</a> \d{2}-\w+-\d{4} \d{2}:\d{2} +-',
            r'<a href="49_name_len_directory_345678901234567890123456789/">49_name_len_directory_345678901234567890123456789/</a> \d{2}-\w+-\d{4} \d{2}:\d{2} +-',
            r'<a href="50_name_len_file_2345678901234567890123456789\.html">50_name_len_file_2345678901234567890123456789\.html</a> \d{2}-\w+-\d{4} \d{2}:\d{2} +\d+',
            r'<a href="51_name_len_file_12345678901234567890123456789\.html">51_name_len_file_12345678901234567890123456789\.\.\.&gt;</a> \d{2}-\w+-\d{4} \d{2}:\d{2} +\d+',
            r'<a href="escape%26%3c%22char%22%3e\.html">escape&amp;&lt;&quot;char&quot;&gt;\.html</a>                               \d{2}-\w+-\d{4} \d{2}:\d{2} +\d+',
            r'<a href="%3chtml%3e\.html">&lt;html&gt;\.html</a>                                        \d{2}-\w+-\d{4} \d{2}:\d{2} +\d+',
        ]
        for pattern in expected_files:
            assert re.search(pattern, actual_body) is not None
        assert response.status == 200

    def test_autoindex_off(self, http_connection: HTTPConnection):
        """autoindex offのディレクトリで404を返すこと"""
        http_connection.request("GET", "/subdir/")
        response = http_connection.getresponse()
        assert_response(HTTPStatus.NOT_FOUND, response)


class TestInvalid:
    def test_invalid_method(self, http_connection: HTTPConnection):
        """サポートしていないmethodなら501を返すこと"""
        http_connection.request("INVALID", "/")
        response = http_connection.getresponse()
        assert_response(HTTPStatus.NOT_IMPLEMENTED, response)

    def test_not_allowed_method(self, http_connection: HTTPConnection):
        """allow_methodで許可されていないmethodなら405を返すこと"""
        http_connection.request("GET", "/allow_method_one/")
        response = http_connection.getresponse()
        assert_response(HTTPStatus.METHOD_NOT_ALLOWED, response)

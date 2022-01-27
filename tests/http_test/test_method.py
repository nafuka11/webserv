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


class TestDelete:
    def test_file_found(self, http_connection: HTTPConnection, tmp_path: Path):
        """DELETEでファイルが存在し削除可能なとき204を返すこと"""
        file_name = "new_file"
        file_path = tmp_path / file_name
        file_path.write_text("This file should be deleted")
        http_connection.request("DELETE", f"/test_delete/{tmp_path.name}/{file_name}")
        response = http_connection.getresponse()
        assert_response(HTTPStatus.NO_CONTENT, response, empty_body=True)

    def test_file_not_found(self, http_connection: HTTPConnection):
        """DELETEでファイルが存在しないとき404を返すこと"""
        http_connection.request("DELETE", "/test_delete/no_such_file")
        response = http_connection.getresponse()
        assert_response(HTTPStatus.NOT_FOUND, response)

    def test_directory(self, http_connection: HTTPConnection, tmp_path: Path):
        """DELETEで指定パスがディレクトリのとき403を返すこと"""
        dir_name = "new_dir"
        dir_path = tmp_path / dir_name
        dir_path.mkdir()
        http_connection.request("DELETE", f"/test_delete/{tmp_path.name}/{dir_name}")
        response = http_connection.getresponse()
        assert_response(HTTPStatus.FORBIDDEN, response)

    def test_file_no_permission(self, http_connection: HTTPConnection, tmp_path: Path):
        """DELETEでファイルに書き込み権限が無いとき403を返すこと"""
        file_name = "new_file"
        file_path = tmp_path / file_name
        file_path.write_text("This file should not be deleted")
        file_path.chmod(0o444)  # r--r--r--
        http_connection.request("DELETE", f"/test_delete/{tmp_path.name}/{file_name}")
        response = http_connection.getresponse()
        assert_response(HTTPStatus.FORBIDDEN, response)

    def test_index(self, http_connection: HTTPConnection, tmp_path: Path):
        """DELETEでパスにindexが存在するディレクトリが指定されたとき403を返すこと"""
        file_name = "index.html"
        file_path = tmp_path / file_name
        file_path.write_text("This file should not be deleted")
        http_connection.request("DELETE", f"/test_delete/{tmp_path.name}/")
        response = http_connection.getresponse()
        assert_response(HTTPStatus.FORBIDDEN, response)

    def test_autoindex(self, http_connection: HTTPConnection):
        """DELETEでパスがautoindex onのディレクトリのとき403を返すこと"""
        http_connection.request("DELETE", "/autoindex/")
        response = http_connection.getresponse()
        assert_response(HTTPStatus.FORBIDDEN, response)


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

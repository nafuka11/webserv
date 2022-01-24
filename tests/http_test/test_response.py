import re
from http import HTTPStatus
from http.client import HTTPConnection

from helper import HTML_PATH_404, HTML_PATH_OK, assert_response


def test_valid(http_connection: HTTPConnection):
    """正常なGETリクエストで200を返すこと"""
    http_connection.request("GET", "/")
    response = http_connection.getresponse()
    assert_response(HTTPStatus.OK, response, HTML_PATH_OK)


def test_invalid_method(http_connection: HTTPConnection):
    """サポートしていないmethodなら501を返すこと"""
    http_connection.request("INVALID", "/")
    response = http_connection.getresponse()
    assert_response(HTTPStatus.NOT_IMPLEMENTED, response)


def test_not_allowed_method(http_connection: HTTPConnection):
    """allow_methodで許可されていないmethodなら405を返すこと"""
    http_connection.request("GET", "/allow_method_one/")
    response = http_connection.getresponse()
    assert_response(HTTPStatus.METHOD_NOT_ALLOWED, response)


def test_invalid_http_version(http_connection: HTTPConnection):
    """HTTP/1.0なら400を返すこと"""
    http_connection._http_vsn_str = "HTTP/1.0"
    http_connection.request("GET", "/")
    response = http_connection.getresponse()
    assert_response(HTTPStatus.BAD_REQUEST, response)


def test_404_no_such_file(http_connection: HTTPConnection):
    """URIのファイルが存在しない時、404を返すこと"""
    http_connection.request("GET", "/nosuchfile")
    response = http_connection.getresponse()
    assert_response(HTTPStatus.NOT_FOUND, response)


def test_autoindex_on(http_connection: HTTPConnection):
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


def test_autoindex_off(http_connection: HTTPConnection):
    """autoindex offのディレクトリで404を返すこと"""
    http_connection.request("GET", "/subdir/")
    response = http_connection.getresponse()
    assert_response(HTTPStatus.NOT_FOUND, response)


def test_error_page_found(http_connection: HTTPConnection):
    """error_pageで設定したファイルが存在する時、そのファイルを返すこと"""
    http_connection.request("GET", "/error_page/no_such_file.html")
    response = http_connection.getresponse()
    assert_response(HTTPStatus.NOT_FOUND, response, HTML_PATH_404)


def test_error_page_not_found(http_connection: HTTPConnection):
    """error_pageで設定したファイルが存在しない時、StatusCodeに応じたレスポンスを返すこと"""
    http_connection.request("GET", "/error_page_not_found/no_such_file.html")
    response = http_connection.getresponse()
    assert_response(HTTPStatus.NOT_FOUND, response)


def test_chunk_size_is_not_hex(http_connection: HTTPConnection):
    """chunk-sizeが16進数でない場合、400を返すこと"""
    headers = {"Transfer-Encoding": "chunked"}
    body = "1g\r\n0"
    http_connection.request("POST", "/test_post/", headers=headers, body=body)
    response = http_connection.getresponse()
    assert_response(HTTPStatus.BAD_REQUEST, response)


def test_chunk_size_overflow(http_connection: HTTPConnection):
    """chunk-sizeがオーバーフローする場合、413を返すこと"""
    headers = {"Transfer-Encoding": "chunked"}
    body = "9999999999999999999\r\n0"
    http_connection.request("POST", "/test_post/", headers=headers, body=body)
    response = http_connection.getresponse()
    assert_response(HTTPStatus.REQUEST_ENTITY_TOO_LARGE, response)


def test_chunk_data_not_end_with_newline(http_connection: HTTPConnection):
    """chunk-dataが\r\nで終わらない場合、400を返すこと"""
    headers = {"Transfer-Encoding": "chunked"}
    body = "5\r\n123456\r\n0"
    http_connection.request("POST", "/test_post/", headers=headers, body=body)
    response = http_connection.getresponse()
    assert_response(HTTPStatus.BAD_REQUEST, response)

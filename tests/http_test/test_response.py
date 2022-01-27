from http import HTTPStatus
from http.client import HTTPConnection

from helper import HTML_PATH_404, assert_response


def test_invalid_http_version(http_connection: HTTPConnection):
    """HTTP/1.0なら400を返すこと"""
    http_connection._http_vsn_str = "HTTP/1.0"
    http_connection.request("GET", "/")
    response = http_connection.getresponse()
    assert_response(HTTPStatus.BAD_REQUEST, response)


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
    body = "\r\n".join(["1g", "0", ""])
    http_connection.request("POST", "/test_post/", headers=headers, body=body)
    response = http_connection.getresponse()
    assert_response(HTTPStatus.BAD_REQUEST, response)


def test_chunk_size_overflow(http_connection: HTTPConnection):
    """chunk-sizeがオーバーフローする場合、413を返すこと"""
    headers = {"Transfer-Encoding": "chunked"}
    body = "\r\n".join([hex(9999999999999999999), "0", ""])
    http_connection.request("POST", "/test_post/", headers=headers, body=body)
    response = http_connection.getresponse()
    assert_response(HTTPStatus.REQUEST_ENTITY_TOO_LARGE, response)


def test_chunk_size_over_client_max_body_size(http_connection: HTTPConnection):
    """chunk-sizeがclient_max_body_sizeを超える場合、413を返すこと"""
    headers = {"Transfer-Encoding": "chunked"}
    body = "\r\n".join([hex(99), "a" * 99, hex(2), "a" * 2, "0", ""])
    http_connection.request("POST", "/test_post/", headers=headers, body=body)
    response = http_connection.getresponse()
    assert_response(HTTPStatus.REQUEST_ENTITY_TOO_LARGE, response)


def test_chunk_data_not_end_with_newline(http_connection: HTTPConnection):
    """chunk-dataが\r\nで終わらない場合、400を返すこと"""
    headers = {"Transfer-Encoding": "chunked"}
    body = "\r\n".join([hex(5), "123456", "0", ""])
    http_connection.request("POST", "/test_post/", headers=headers, body=body)
    response = http_connection.getresponse()
    assert_response(HTTPStatus.BAD_REQUEST, response)

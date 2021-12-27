from http import HTTPStatus
from http.client import HTTPConnection
from tests.helper import assert_response


def test_valid(http_connection: HTTPConnection):
    """正常なGETリクエストで200を返すこと"""
    http_connection.request("GET", "/")
    response = http_connection.getresponse()
    assert_response(HTTPStatus.OK, response)


def test_invalid_method(http_connection: HTTPConnection):
    """サポートしていないmethodなら501を返すこと"""
    http_connection.request("INVALID", "/")
    response = http_connection.getresponse()
    assert_response(HTTPStatus.NOT_IMPLEMENTED, response)


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

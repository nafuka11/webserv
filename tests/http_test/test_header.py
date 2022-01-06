from datetime import datetime
from http import HTTPStatus
from http.client import HTTPConnection
from typing import Callable

from tests.helper import assert_response


def test_header_server(http_connection: HTTPConnection):
    """Serverヘッダがあること"""
    http_connection.request("GET", "/")
    response = http_connection.getresponse()
    assert response.getheader("Server") == "webserv/1.0.0"


def test_header_date(http_connection: HTTPConnection):
    """DateヘッダがIMF-fixdate形式であること"""
    http_connection.request("GET", "/")
    response = http_connection.getresponse()
    date_value = response.getheader("Date")
    datetime.strptime(date_value, "%a, %d %b %Y %H:%M:%S GMT")


def test_empty_header_value(http_connection: HTTPConnection):
    """ヘッダの値が空の時、400を返すこと"""
    headers = {"Empty": ""}
    http_connection.request("GET", "/", headers=headers)
    response = http_connection.getresponse()
    assert_response(HTTPStatus.BAD_REQUEST, response)


def test_duplicate_host_header(http_connection: HTTPConnection):
    """Hostヘッダが重複した時、400を返すこと"""
    http_connection.putrequest("GET", "/", skip_host=True)
    http_connection.putheader("host", "example.com")
    http_connection.putheader("host", "example.com")
    http_connection.endheaders()
    response = http_connection.getresponse()
    assert_response(HTTPStatus.BAD_REQUEST, response)


def test_missing_host_header(http_connection: HTTPConnection):
    """Hostヘッダが存在しない時、400を返すこと"""
    http_connection.putrequest("GET", "/", skip_host=True)
    http_connection.endheaders()
    response = http_connection.getresponse()
    assert_response(HTTPStatus.BAD_REQUEST, response)


def test_negative_content_length_header(http_connection: HTTPConnection):
    """Content-Lengthヘッダの値が負数の時、400を返すこと"""
    headers = {"Content-Length": "-1"}
    http_connection.request("GET", "/", headers=headers)
    response = http_connection.getresponse()
    assert_response(HTTPStatus.BAD_REQUEST, response)


def test_non_digit_content_length_header(http_connection: HTTPConnection):
    """Content-Lengthヘッダの値に不正な文字が含まれる時、400を返すこと"""
    headers = {"Content-Length": "123a"}
    http_connection.request("GET", "/", headers=headers)
    response = http_connection.getresponse()
    assert_response(HTTPStatus.BAD_REQUEST, response)


def test_client_max_body_size_0(
    http_connection_factory: Callable[[int], HTTPConnection]
):
    """client_max_body_sizeが0の時、Content-Lengthの値が制限されないこと"""
    headers = {"Content-Length": "100000"}
    http_connection = http_connection_factory(8080)
    http_connection.request("GET", "/", headers=headers)
    response = http_connection.getresponse()
    assert_response(HTTPStatus.OK, response)


def test_over_client_max_body_size(http_connection: HTTPConnection):
    """Content-Lengthの値がclient_max_body_sizeを超える時、413を返すこと"""
    headers = {"Content-Length": "100000"}
    http_connection.request("GET", "/", headers=headers)
    response = http_connection.getresponse()
    assert_response(HTTPStatus.REQUEST_ENTITY_TOO_LARGE, response)


def test_equal_client_max_body_size(http_connection: HTTPConnection):
    """Content-Lengthの値がclient_max_body_sizeと等しい時、200を返すこと"""
    headers = {"Content-Length": "100"}
    http_connection.request("GET", "/", headers=headers)
    response = http_connection.getresponse()
    assert_response(HTTPStatus.OK, response)

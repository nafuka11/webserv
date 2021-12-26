from http import HTTPStatus
from http.client import HTTPConnection
from tests.helper import assert_response


def test_valid(http_connection: HTTPConnection):
    http_connection.request("GET", "/")
    response = http_connection.getresponse()
    actual_body = response.read()
    expected_body = RESPONSE_BODY_200
    assert response.status == 200
    assert response.reason == "OK"
    assert response.version == 11
    assert response.getheader("Content-Length") == str(len(expected_body))
    assert actual_body == expected_body


def test_header_server(http_connection: HTTPConnection):
    http_connection.request("GET", "/")
    response = http_connection.getresponse()
    assert response.getheader("Server") == "webserv/1.0.0"


def test_header_date(http_connection: HTTPConnection):
    http_connection.request("GET", "/")
    response = http_connection.getresponse()
    date_value = response.getheader("Date")
    datetime.strptime(date_value, "%a, %d %b %Y %H:%M:%S GMT")


def test_invalid_method(http_connection: HTTPConnection):
    http_connection.request("INVALID", "/")
    response = http_connection.getresponse()
    actual_body = response.read()
    expected_body = RESPONSE_BODY_501
    assert response.status == 501
    assert response.reason == "Not Implemented"
    assert response.version == 11
    assert response.getheader("Content-Length") == str(len(expected_body))
    assert actual_body == expected_body


def test_invalid_http_version(http_connection: HTTPConnection):
    http_connection._http_vsn_str = "HTTP/1.0"
    http_connection.request("GET", "/")
    response = http_connection.getresponse()
    actual_body = response.read()
    expected_body = RESPONSE_BODY_400
    assert response.status == 400
    assert response.reason == "Bad Request"
    assert response.version == 11
    assert response.getheader("Content-Length") == str(len(expected_body))
    assert actual_body == RESPONSE_BODY_400

import pytest
from datetime import datetime
from http.client import HTTPConnection
import subprocess
import time

HOST = "localhost"
PORT = 80
WEBSERV_PATH = "../webserv"
SLEEP_TIME = 0.1

RESPONSE_BODY_200 = (
    b"<html>\r\n"
    b"<head><title>200 OK</title></head>\r\n"
    b"<body>\r\n"
    b"<center><h1>200 OK</h1></center><hr><center>webserv/1.0.0</center>\r\n"
    b"</body>\r\n"
    b"</html>\r\n"
)
RESPONSE_BODY_501 = (
    b"<html>\r\n"
    b"<head><title>501 Not Implemented</title></head>\r\n"
    b"<body>\r\n"
    b"<center><h1>501 Not Implemented</h1></center><hr><center>webserv/1.0.0</center>\r\n"
    b"</body>\r\n"
    b"</html>\r\n"
)
RESPONSE_BODY_400 = (
    b"<html>\r\n"
    b"<head><title>400 Bad Request</title></head>\r\n"
    b"<body>\r\n"
    b"<center><h1>400 Bad Request</h1></center><hr><center>webserv/1.0.0</center>\r\n"
    b"</body>\r\n"
    b"</html>\r\n"
)


@pytest.fixture
def http_connection() -> HTTPConnection:
    connection = HTTPConnection(HOST, PORT)
    yield connection
    connection.close()


@pytest.fixture(scope="session", autouse=True)
def server():
    process = subprocess.Popen(WEBSERV_PATH)
    time.sleep(SLEEP_TIME)
    yield
    process.terminate()


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


def test_empty_header_value(http_connection: HTTPConnection):
    headers = {"Empty": ""}
    http_connection.request("GET", "/", headers=headers)
    response = http_connection.getresponse()
    actual_body = response.read()
    expected_body = RESPONSE_BODY_400
    assert response.status == 400
    assert response.reason == "Bad Request"
    assert response.version == 11
    assert response.getheader("Content-Length") == str(len(expected_body))
    assert actual_body == expected_body


def test_duplicate_host_header(http_connection: HTTPConnection):
    http_connection.putrequest("GET", "/", skip_host=True)
    http_connection.putheader("host", "example.com")
    http_connection.putheader("host", "example.com")
    http_connection.endheaders()
    response = http_connection.getresponse()
    actual_body = response.read()
    expected_body = RESPONSE_BODY_400
    assert response.status == 400
    assert response.reason == "Bad Request"
    assert response.version == 11
    assert response.getheader("Content-Length") == str(len(expected_body))
    assert actual_body == expected_body


def test_missing_host_header(http_connection: HTTPConnection):
    http_connection.putrequest("GET", "/", skip_host=True)
    http_connection.endheaders()
    response = http_connection.getresponse()
    actual_body = response.read()
    expected_body = RESPONSE_BODY_400
    assert response.status == 400
    assert response.reason == "Bad Request"
    assert response.version == 11
    assert response.getheader("Content-Length") == str(len(expected_body))
    assert actual_body == expected_body


def test_negative_content_length_header(http_connection: HTTPConnection):
    headers = {"Content-Length": "-1"}
    http_connection.request("GET", "/", headers=headers)
    response = http_connection.getresponse()
    actual_body = response.read()
    expected_body = RESPONSE_BODY_400
    assert response.status == 400
    assert response.reason == "Bad Request"
    assert response.version == 11
    assert response.getheader("Content-Length") == str(len(expected_body))
    assert actual_body == expected_body


def test_non_digit_content_length_header(http_connection: HTTPConnection):
    headers = {"Content-Length": "123a"}
    http_connection.request("GET", "/", headers=headers)
    response = http_connection.getresponse()
    actual_body = response.read()
    expected_body = RESPONSE_BODY_400
    assert response.status == 400
    assert response.reason == "Bad Request"
    assert response.version == 11
    assert response.getheader("Content-Length") == str(len(expected_body))
    assert actual_body == expected_body

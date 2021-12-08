import pytest
from datetime import datetime
from http.client import HTTPConnection
import subprocess
import time

HOST = "localhost"
PORT = 80
WEBSERV_PATH = "../webserv"
SLEEP_TIME = 0.1


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
    expected_body = (
        b"<html>\r\n"
        b"<head><title>200 OK</title></head>\r\n"
        b"<body>\r\n"
        b"<center><h1>200 OK</h1></center><hr><center>webserv/1.0.0</center>\r\n"
        b"</body>\r\n"
        b"</html>\r\n"
    )
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
    expected_body = (
        b"<html>\r\n"
        b"<head><title>501 Not Implemented</title></head>\r\n"
        b"<body>\r\n"
        b"<center><h1>501 Not Implemented</h1></center><hr><center>webserv/1.0.0</center>\r\n"
        b"</body>\r\n"
        b"</html>\r\n"
    )
    assert response.status == 501
    assert response.reason == "Not Implemented"
    assert response.version == 11
    assert response.getheader("Content-Length") == str(len(expected_body))
    assert actual_body == expected_body

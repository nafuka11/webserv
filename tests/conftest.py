from typing import Callable
import pytest
import subprocess
import time
from http.client import HTTPConnection

HOST = "localhost"
PORT = 4242
CLIENT_MAX_BODY_SIZE_PORT = 8080
WEBSERV_PATH = "../webserv"
CONF_FILE = "./conf/default.conf"
SLEEP_TIME = 0.1


pytest.register_assert_rewrite("tests.helper")


@pytest.fixture
def http_connection(request: pytest.FixtureRequest) -> HTTPConnection:
    connection = HTTPConnection(HOST, PORT)
    request.addfinalizer(connection.close)
    yield connection


@pytest.fixture
def http_connection_factory() -> Callable[[int], HTTPConnection]:
    """portを指定してwebservに接続する関数を返す"""

    def _factory(port: int) -> HTTPConnection:
        connection = HTTPConnection(HOST, port)
        return connection

    return _factory


@pytest.fixture(scope="session", autouse=True)
def server(request: pytest.FixtureRequest) -> None:
    process = subprocess.Popen([WEBSERV_PATH, CONF_FILE])
    request.addfinalizer(process.terminate)
    time.sleep(SLEEP_TIME)
    yield

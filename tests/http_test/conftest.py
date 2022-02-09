import subprocess
import time
from http.client import HTTPConnection
from pathlib import Path
from typing import Callable
from urllib.error import URLError
from urllib.request import urlopen

import pytest
from pytest import TempPathFactory

HOST = "localhost"
PORT = 4242
CLIENT_MAX_BODY_SIZE_PORT = 8080
VIRTUAL_HOST_PORT = 4243
WEBSERV_PATH = "../../webserv"
CONF_FILE = "./conf/default.conf"
SLEEP_TIME = 0.1


pytest.register_assert_rewrite("helper")


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
    _wait_for_connection()
    yield


def _wait_for_connection() -> None:
    while True:
        try:
            urlopen(f"http://{HOST}:{PORT}", timeout=1)
            return
        except URLError:
            time.sleep(SLEEP_TIME)


@pytest.fixture(scope="session")
def upload_dir(tmp_path_factory: TempPathFactory) -> Path:
    dir = tmp_path_factory.getbasetemp() / "upload"
    dir.mkdir(exist_ok=True)
    return dir


@pytest.fixture(scope="session")
def upload_no_perm_dir(tmp_path_factory: TempPathFactory) -> Path:
    dir = tmp_path_factory.getbasetemp() / "upload_no_perm"
    dir.mkdir(0o444, exist_ok=True)
    return dir

from http import HTTPStatus
from http.client import HTTPConnection

import pytest

from http_test.helper import assert_response


class TestRedirect:
    @pytest.mark.parametrize(
        "status",
        [
            HTTPStatus.MOVED_PERMANENTLY,
            HTTPStatus.FOUND,
            HTTPStatus.SEE_OTHER,
            HTTPStatus.NOT_MODIFIED,
            HTTPStatus.TEMPORARY_REDIRECT,
            HTTPStatus.PERMANENT_REDIRECT,
        ],
    )
    def test_reason_phrase(self, http_connection: HTTPConnection, status: HTTPStatus):
        http_connection.request("GET", f"/redirect_{status.value}/")
        response = http_connection.getresponse()
        assert_response(status, response, empty_body=True)
        assert response.getheader("Location") == "http://example.com"

    @pytest.mark.parametrize("status", [104, 999])
    def test_no_reason_phrase(self, http_connection: HTTPConnection, status: int):
        http_connection.request("GET", f"/redirect_{status}/")
        response = http_connection.getresponse()
        assert response.status == status
        assert response.reason == ""
        assert response.getheader("Location") == "http://example.com"

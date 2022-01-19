from http import HTTPStatus
from http.client import HTTPResponse
from typing import Optional

HTML_PATH_OK = "./docs/index.html"
HTML_PATH_404 = "./docs/error_page/404.html"


def assert_response(
    code: HTTPStatus, response: HTTPResponse, html_path: Optional[str] = None
) -> None:
    actual_body = response.read()
    if html_path:
        with open(html_path, "r") as f:
            expected_body = f.read().encode("utf-8")
    else:
        expected_body = _generate_expected_body(code)
    assert response.status == code
    assert response.reason == _get_phrase(code)
    assert response.version == 11
    assert response.getheader("Content-Length") == str(len(expected_body))
    assert actual_body == expected_body


def _get_phrase(code: HTTPStatus) -> str:
    phrase = code.phrase
    # 古いHTTPの規格では "Request Entity Too Large" なのでphraseを変更する
    # https://en.wikipedia.org/wiki/List_of_HTTP_status_codes#4xx_client_errors
    if code == HTTPStatus.REQUEST_ENTITY_TOO_LARGE:
        phrase = "Payload Too Large"
    return phrase


def _generate_expected_body(code: HTTPStatus) -> bytes:
    phrase = _get_phrase(code)
    body = (
        "<html>\r\n"
        f"<head><title>{code} {phrase}</title></head>\r\n"
        "<body>\r\n"
        f"<center><h1>{code} {phrase}</h1></center><hr><center>webserv/1.0.0</center>\r\n"
        "</body>\r\n"
        "</html>\r\n"
    )
    return body.encode("utf-8")

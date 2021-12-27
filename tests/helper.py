from http import HTTPStatus
from http.client import HTTPResponse

RESPONSE_BODY_200 = (
    b"<!DOCTYPE html>\n"
    b'<html lang="en">\n'
    b"\n"
    b"<head>\n"
    b"  <title>Test</title>\n"
    b"</head>\n"
    b"\n"
    b"<body>\n"
    b"  <p>Test html</p>\n"
    b"</body>\n"
    b"\n"
    b"</html>"
)


def assert_response(code: HTTPStatus, response: HTTPResponse) -> None:
    actual_body = response.read()
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
    if code == HTTPStatus.OK:
        return RESPONSE_BODY_200

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

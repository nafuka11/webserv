from http import HTTPStatus
from http.client import HTTPResponse
from typing import Optional

HTML_PATH_OK = "./docs/index.html"
HTML_PATH_404 = "./docs/error_page/404.html"
HTML_PATH_VIRTUAL_HOST = "./docs/vhost/index.html"
HTML_PATH_NULL = "./docs/contains_null.html"
HTML_PATH_EMPTY = "./docs/empty.html"


def assert_response(
    code: HTTPStatus,
    response: HTTPResponse,
    html_path: Optional[str] = None,
    body: Optional[str] = None,
    empty_body: Optional[bool] = False,
) -> None:
    """HTTPResponseのassertをする(statusCode, reason, httpVersion, messageBody, Content-Length)

    Args:
        code (HTTPStatus): StatusCode
        response (HTTPResponse): HTTPResponse
        html_path (Optional[str]): 想定されるHTMLのファイルパス。未指定の場合はcodeに応じてHTMLを自動生成する
        body (Optional[str]): 想定されるmessageBodyの文字列
        empty_body (Optional[bool]): StatusCodeを元にHTMLを生成しない場合True
    """
    actual_body = response.read()
    expected_body = b""
    if html_path:
        with open(html_path, "r") as f:
            expected_body = f.read().encode("utf-8")
    elif not empty_body:
        expected_body = _generate_expected_body(code)
    elif body:
        expected_body = body
    assert response.status == code
    assert response.reason == _get_phrase(code)
    assert response.version == 11
    assert response.getheader("Content-Length") == str(len(expected_body))
    assert actual_body == expected_body


def _get_phrase(code: HTTPStatus) -> str:
    """StatusCodeに応じたPhraseを取得する"""
    phrase = code.phrase
    # 古いHTTPの規格では "Request Entity Too Large" なのでphraseを変更する
    # https://en.wikipedia.org/wiki/List_of_HTTP_status_codes#4xx_client_errors
    if code == HTTPStatus.REQUEST_ENTITY_TOO_LARGE:
        phrase = "Payload Too Large"
    return phrase


def _generate_expected_body(code: HTTPStatus) -> bytes:
    """StatusCodeに応じたHTMLを返す"""
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

#include "csapp.h"

int main(int argc, char **argv) //argc: 전달되는 정보의 갯수, argv: 전달되는 문자 배열
{
    // file descriptor
    int clientfd;
    // host가 저장된 주소, port 저장된 주소, buffer
    char *host, *port, buf[MAXLINE];
    // robust i/o
    rio_t rio;

    //==============================
    if (argc != 3) {
        fprintf(stderr, "usage: %s <host> <port>\n", argv[0]);
        exit(0);
    }

    host = argv[1]; // host 입력받음 localhost: 127.0.0.1
    port = argv[2]; // port 입력받음 port는 서버에서 설정한대로

    clientfd = Open_clientfd(host, port); // 그림11.18: 서버와 연결을 설정하는 도움 함수
    Rio_readinitb(&rio, clientfd); // 식별자 clientfd를 주소 rio에 위치한 rio_t타입 읽기 버퍼와 연결

    //==============================연결 수립

    while(Fgets(buf, MAXLINE, stdin) != NULL)
    //EOF 표준입력 만나면 종료, Ctrl+d 혹은 텍스트 소진의 경우임
    {
        // short count를 처리하기 위해 책에서 사용하는 함수들, 표준 함수도 아님...
        Rio_writen(clientfd, buf, strlen(buf));
        Rio_readlineb(&rio, buf, MAXLINE);
        Fputs(buf, stdout);
    }
    Close(clientfd);
    // 클라이언트 커널이 프로세스가 종료할 때 자동으로 모든 식별자를 닫기때문에 불필요하지만
    // 열었던 식별자를 명시적으로 닫아주는 것이 올바른 프로그래밍 습관
    exit(0);
}
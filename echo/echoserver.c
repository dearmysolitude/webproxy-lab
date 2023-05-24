#include "csapp.h"

void echo(int connfd);

int main(int argc, char **argv) //argv[0] 은 파일명
{
    int listenfd, connfd;
    socklen_t clientlen;
    struct sockaddr_storage clientaddr; /*enough space for any address*/
    //clientaddr: accept로 보내지는 소켓 주소 구조체
    char client_hostname[MAXLINE], client_port[MAXLINE];
    
    if (argc != 2) { /*오류 처리*/
        fprintf(stderr, "usage: %s <port>\n", argv[0]);
        exit(0);
    } 

    listenfd = Open_listenfd(argv[1]); // 듣기 descriptor 오픈

    while(1) { //각 반복실행은 클라이언트로부터 연결 요청을 기다림
        clientlen = sizeof(struct sockaddr_storage);
        /*socket 중 제일 커다란 자료를 가져옴으로써 어떤 자료든 받을 수 있음*/

        connfd = Accept(listenfd, (SA *)&clientaddr, &clientlen);
// Accept(int listenfd, struct sockaddr *addr, int *addrlen)의
// 하위함수 accept(int listenfd, struct sockaddr *addr, int *addrlne)-소켓 인터페이스
// listening descriptor를 전달받아 client socekt address를 clientadrr에 넣고,
// connection descriptor를 반환한다.
// -> clientaddr에는 요청하는 클라이언트의 소켓 주소가 채워진다.
// 시스템 콜을 사용하므로 서버가 실행되면 여기에서 정지

        Getnameinfo((SA *) &clientaddr, clientlen, client_hostname,
         MAXLINE, client_port, MAXLINE, 0);
// socket address structure를 corresponding host, service name string으로 되돌린다.
// 하위 함수인 getnameinfo(sa, salen, host, hostlen, serv, 
//                          servlen, flags)) != 0) 는
// sa 로부터 받은 socket address structure을 host와 serv 버퍼에 저장한다.

        printf("Connected to (%s, %s)\n", client_hostname, client_port);
        //도메인 이름, 연결 클라이언트 포트 출력
        
        echo(connfd); //클라이언트에 서비스하는 함수

        Close(connfd); //연결된 descriptor 종료
    }
    exit(0);
}

/* 한 번에 한 개씩의 클라이언트를 반복해서 실행하는 이런 서버를
 * 반복 서버iterative server라고 부른다.
 */
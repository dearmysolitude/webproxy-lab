/* $begin tinymain */
/*
 * tiny.c - A simple, iterative HTTP/1.0 Web server that uses the
 *     GET method to serve static and dynamic content.
 *
 * Updated 11/2019 droh
 *   - Fixed sprintf() aliasing issue in serve_static(), and clienterror().
 */
#include "csapp.h"

void doit(int fd);
void read_requesthdrs(rio_t *rp);
int parse_uri(char *uri, char *filename, char *cgiargs);
void serve_static(int fd, char *filename, int filesize);
void get_filetype(char *filename, char *filetype);
void serve_dynamic(int fd, char *filename, char *cgiargs);
void clienterror(int fd, char *cause, char *errnum, char *shortmsg,
                 char *longmsg);

/*
Rio_readinitb(rio_t *rp, int fd): 파일 식별자와 읽기 버퍼 연결
Rio_readlineb(): 입력 스트림에서 한 줄의 문자열을 읽어오는 함수.
fprintf(FILE *stream, const char *foramt, ...): formatted output을 stream에 쓰기 위해 사용됨
exit(status): status - 프로그램의 종료 상태를 나타내는 int형의 값
accept(): 듣기 식별자를 받음, 처리가 끝날 때 까지 대기(시스템 콜 사용),
듣기 식별자는 서버 살아있는 동안 계속 존재, accept는 연결 식별자를 리턴, 클라이언트-서버 사이의 연결 끝점
서버가 연결 요청을 수락시마다 생성, 서버가 클라이언트에 서비스하는 동안에만 존재
getnameinfo(): 호스트이름, 호스트주소, 서비스이름, 포트번호의 스트링 표시를 소켓 주소 구조체로 변환
getaddressinfo(): getaddrinfo와 반대

*/


int main(int argc, char **argv) {
  int listenfd, connfd;
  char hostname[MAXLINE], port[MAXLINE];
  socklen_t clientlen;
  struct sockaddr_storage clientaddr;

  /* Check command line args */
  if (argc != 2) {
    fprintf(stderr, "usage: %s <port>\n", argv[0]);
    exit(1);
  }

  listenfd = Open_listenfd(argv[1]);
  while (1) {
    clientlen = sizeof(clientaddr);
    connfd = Accept(listenfd, (SA *)&clientaddr,
                    &clientlen);  // line:netp:tiny:accept
    Getnameinfo((SA *)&clientaddr, clientlen, hostname, MAXLINE, port, MAXLINE,
                0);
	/*소켓 구조체로부터*/
    printf("Accepted connection from (%s, %s)\n", hostname, port);
    doit(connfd);   // line:netp:tiny:doit
    Close(connfd);  // line:netp:tiny:close
  }
}

void doit(int fd) /*반복루틴 중, 싱글 루틴*/
{
  int is_static;
  struct stat sbuf;
  char buf[MAXLINE], method[MAXLINE], uri[MAXLINE], version[MAXLINE];
  char filename[MAXLINE], cgiargs[MAXLINE];
  rio_t rio;

  /*Read request line and headers -> 요청 라인을 읽는다*/
  Rio_readinitb(&rio, fd); //파일 식별자와 읽기 버퍼 연결
  Rio_readlineb(&rio, buf, MAXLINE); //파일에서 줄을 읽는다null문자로 끝나고 null문자는 포함x
  printf("Request headers:\n");
  printf("%s", buf); /*입력 받은 내용을 출력한다*/
  sscanf(buf, "%s %s %s", method, uri, version);

  if

  if(strcasecmp(method, "GET")&&!(strcasecmp(method, "HEAD"))) { /*GET 메소드가 아니면 에러메세지 보낸다*/
    clienterror(fd, method, "501", "Not implemented",
    "Tiny Does not implement this method");
    return;
  }



  read_requesthdrs(&rio); /*요청 헤더 URI를 읽어들인다*/

  /*Parse URI from GET request*/
  is_static = parse_uri(uri, filename, cgiargs); /*정적인지 동적인지 플래그 설정*/

  /*파일이 디스크상에 있지 않으면*/
  if(stat(filename, &sbuf) < 0) { 
    clienterror(fd, filename, "404", "Not found", "Tiny couldn't find this file");
    return;
  }

  if(is_static) { /*serve static content*/

    /*보통파일?, 읽기 권한있나?*/
    if(!(S_ISREG(sbuf.st_mode)) || !(S_IRUSR & sbuf.st_mode)) { 
      clienterror(fd, filename, "403", "Forbidden", "Tiny couldn't read the file");
      return;
    }

    serve_static(fd, filename, sbuf.st_size); /*클라이언트에 정적 컨텐츠 제공*/
  }


  else { /*serve dynamic content*/
  
    /*보통파일이고, 읽기권한 있나?*/
    if(!(S_ISREG(sbuf.st_mode)) || !(S_IXUSR & sbuf.st_mode)) {
      clienterror(fd, filename, "403", "Forbidden", "Tiny couldn't run the CGI program");
      return;
    }

    serve_dynamic(fd, filename, cgiargs); /*클라이언트에 동적 컨텐츠 제공*/
  }
}

void clienterror(int fd, char *cause, char *errnum, char *shortmsg, char *longmsg)
{
  char buf[MAXLINE], body[MAXBUF];

  /*Build the HTTP response body*/
  sprintf(body, "<html><title>Tiny Error</title>"); /*버퍼에 문자열을 저장*/
  sprintf(body, "%s<body bgcolor = ""ffffff"">\r\n", body);
  sprintf(body, "%s%s: %s\r\n", body, errnum, shortmsg);
  sprintf(body, "%s<p>%s: %s\r\n", body, longmsg, cause);
  sprintf(body, "%s<hr><em>The Tiny Web server</em>\r\n", body);
  
  /*print the HTTP response*/
  sprintf(buf, "HTTP/1.1 %s %s \r\n", errnum, shortmsg);
  Rio_writen(fd, buf, strlen(buf));
  sprintf(buf, "Content-type: text/html\r\n");
  Rio_writen(fd, buf, strlen(buf));
  sprintf(buf, "Content-length: %d\r\n\r\n", (int)strlen(body));
  Rio_writen(fd, buf, strlen(buf));
  Rio_writen(fd, body, strlen(body));
}

void read_requesthdrs(rio_t *rp)
{
	char buf[MAXLINE];

	Rio_readlineb(rp, buf, MAXLINE);1
	/* strcmp: 매개변수로 주어지는 두 문자열을 비교하여 완전히 같으면 0
	다르면 양수 혹은 음수를 반환한다, 즉, 입력되는 문자가 비어있지 않을
	경우 계속해서 헤더 입력받아 출력만 한다*/
	while(strcmp(buf, "\r\n")) { /* 캐리어 리턴 + 라인 피드, 인쇄하고 끝 */
		Rio_readlineb(rp, buf, MAXLINE);
		printf("%s", buf);
	}
	return;
}

int parse_uri(char *uri, char *filename, char *cgiargs)
{
	char *ptr;

	if(!strstr(uri, "cgi-bin")) { /*static content*/
		strcpy(cgiargs, ""); /*""를 널문자 포함하여 cgiargs에 복사한다*/
		strcpy(filename, ".");
		strcat(filename, uri); /*filename뒤에 uri를 붙인다*/
		if(uri[strlen(uri)-1] == '/')/* /로 끝나면 기본 파일 이름을 추가한다*/
			strcat(filename, "home.html");
		return 1;
	}
	else { /*dynamic content*/
		ptr = index(uri, '?');
		/*모든 cgi인자들을 추출한다*/
		if(ptr) {
			strcpy(cgiargs, ptr+1);
			*ptr = '\0';
		}
		else
			strcpy(cgiargs, "");
		strcpy(filename, ".");
		strcat(filename, uri);
		return 0;
	}
}

void serve_static(int fd, char *filename, int filesize)
{
	int srcfd;
	char *srcp, filetype[MAXLINE], buf[MAXBUF];
	rio_t rio;

	/*Send response headers to client*/
	get_filetype(filename, filetype); /*접미어 부분을 탐색해서 파일 타입을 찾는다*/
	sprintf(buf, "HTTP/1.1 200 OK\r\n"); //상태 코드
	sprintf(buf, "%sServer: Tiny WEb Server\r\n", buf); // 서버 이름
	sprintf(buf, "%sConnection: close\r\n", buf); // 연결 방식
	sprintf(buf, "%sContent-length: %d\r\n", buf, filesize); // 컨텐츠 길이
	sprintf(buf, "%sContent-type: %s\r\n\r\n", buf, filetype); //컨텐츠 타입
	Rio_writen(fd, buf, strlen(buf)); //buf에서 fd로 전송(헤더 정보)
	printf("Response headers: \n");
	printf("%s", buf);

	if(strcasecmp(method, "GET")==0) { //HEAD요청을 처리하기 위해 추가(HEAD에서는 안들어간다)
	/*Send response body to client*/
	srcfd = Open(filename, O_RDONLY, 0);
	// srcp = Mmap(0, filesize, PROT_READ, MAP_PRIVATE, srcfd, 0);
	srcp = malloc(filesize);          // 파일을 위한 메모리 할당
    Rio_readinitb(&rio, srcfd);       // 파일을 버퍼로 읽기위한 초기화
    Rio_readn(srcfd, srcp, filesize); // 읽기
	/*파일을 메모리에 매핑시킨다*/
	Close(srcfd); /*매핑후 필요없으므로 파일 닫는다*/
	Rio_writen(fd, srcp, filesize);
	/*파일을 클라이언트에 전송한다, */
	free(srcp);
	// Munmap(srcp, filesize);
	/*srcp에서 시작하는 프로세스 주소 공간에 위치한 페이지를 포함하는 
	 * 모든 맵핑 제거(길이는 filesize 바이트 만큼)
	 */
	}

}
/* 
 * get_filetype - Derive file type from filename
 */
void get_filetype(char *filename, char *filetype)
{
	/*strstr: 첫번째 매개변수에서 두번째 매개변수를 찾는다*/
	if(strstr(filename, ".html"))
		strcpy(filetype, "text/html");
	else if(strstr(filename, ".gif"))
		strcpy(filetype, "image/gif");
	else if(strstr(filename, ".png"))
		strcpy(filetype, "image/png");
	else if(strstr(filename, ".jpg"))
		strcpy(filetype, "image/jpg");
	else if(strstr(filename, ".jpeg"))
		strcpy(filetype, "image/jpeg");
	else if(strstr(filename, ".mp4"))
		strcpy(filetype, "video/mp4");
	else
		strcpy(filetype, "text/plain");
}

void serve_dynamic(int fd, char *filename, char *cgiargs)
{
	char buf[MAXLINE], *emptylist[] = { NULL };

	/* Return first part of HTTP response, 성공시 */
	sprintf(buf, "HTTP/1.1 200 OK\r\n");
	Rio_writen(fd, buf, strlen(buf));
	sprintf(buf, "Server: Tiny Web Server\r\n");
	Rio_writen(fd, buf, strlen(buf));

	if (Fork() == 0) { /*Child*/
		/* Real server would set all CGI vars here*/
		setenv("QUERY_STRING", cgiargs, 1); /*query string을 uri의 cgi인자들로 초기화*/
		Dup2(fd, STDOUT_FILENO); /* Redirect stdout to client 자식의 표준 출력을 연결파일 식별자로 재지정*/
		Execve(filename, emptylist, environ); /* Run CGI program 실행
		* execve(const char *path, donst char *arg, ..., char *const envp[]):
		다른 프로그램 실행하고 자신은 종료. path: 디렉토리 포함 전체 파일명
		arg: 인수목록, envp: 환경 설정 목록, 반환: 실패일 때만 -1
		*/
	}
	Wait(NULL); /*Parent waits for and reaps child*/
}
CC = gcc # gcc를 이용해서
CFLAGS = -g -Wall # 
LDFLAGS = -lpthread # library pthread를 쓰겠다

all: echo

csapp.o: csapp.c csapp.h # 의존파일
	$(CC) $(CFLAGS) -c csapp.c

echo.o: echo.c csapp.h
	$(CC) $(CFLAGS) -c echo.c

echo: echo.o echoserver.c echoclient.c csapp.o
	$(CC) $(CFLAGS) echoclient.c csapp.o -o echoclient $(LDFLAGS)
	$(CC) $(CFLAGS) echo.o echoserver.c csapp.o -o echoserver $(LDFLAGS)
   
clean:
	rm -f *~ *.o echo echoclient echoserver
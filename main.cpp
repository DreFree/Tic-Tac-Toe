#include <iostream>
#include <ctime>
#include <cstdlib>
#include <cstring>
#include <cstdio>

#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#define PORT 13000

struct Message{
	int cmd;
	char board[3][3];
};

using namespace std;

char Board[3][3];
char Player[2];
int winner;
void init()
{
	int rnd=rand()%2;
	winner=-1;
	for (int i=0;i<9;i++)
		Board[(int)(i/3)][(i%3)]=(char)i;
	Player[rnd]='X';
	Player[1-rnd]='O';
	return;
}
int isChoice()
{
	for (int i=0;i<3;i++)
		for (int j=0;j<3;j++)
			if ((Board[i][j]!='X') && (Board[i][j]!='O'))
				return 1;
	return 0;
}
struct Message prepMessage(int n)
{
	struct Message temp;
	temp.cmd=n;
	for (int i=0;i<3;i++)
		for (int j=0;j<3;j++)
			temp.board[i][j]=Board[i][j];
	return temp;
}
void setWinner(char W)
{
	if (W==Player[0])
		winner=0;
	else
		winner=1;
}
int isWin()
{
	char temp;
	if ((Board[0][0]==Board[1][1]) && (Board[0][0]==Board[2][2]))
	{
		setWinner(Board[0][0]);
		return 1;
	}
	if ((Board[0][2]==Board[1][1]) && (Board[0][2]==Board[2][0]))
	{
		setWinner(Board[0][2]);
		return 1;
	}
	for (int j=0;j<3;j++)
	{
		if((Board[j][j]==Board[j][(j+1)%3]) && Board[j][j]==Board[j][(j+2)%3])
		{
			setWinner(Board[j][j]);
			return 1;
		}
		else if ((Board[j][j]==Board[(j+1)%3][j]) &&Board[j][j]==Board[(j+2)%3][j])
		{
			setWinner(Board[j][j]);
			return 1;
		}
	}
	return 0;
}
int isValid(int cmd)
{
	if (cmd<0 || cmd>8)
		return 0;
	if (Board[(int)(cmd/3)][(cmd%3)]!='X' && Board[(int)(cmd/3)][(cmd%3)]!='O')
        return 1;
    else
        return 0;
}
int AIplay()
{
   bool flag1=false, flag2=false;

    for (int i=0;i<3;i++)
        for (int j=0;j<3;j++)
        {
            if (Board[i][j]==Player[0])
            {
                if(!flag1)
                {
                    if ((Board[0][0]==Board[1][1]))
                        if (Board[(int)8/3][8%3] !='X' && Board[(int)8/3][8%3]!='O')
                            return 8;
                    if ((Board[1][1]==Board[2][2]))
                        if (Board[(int)0/3][0%3] !='X' && Board[(int)0/3][0%3]!='O')
                            return 0;
                    if ((Board[0][0]==Board[2][2]))
                        if (Board[(int)4/3][4%3] !='X' && Board[(int)4/3][4%3]!='O')
                            return 4;
                    if ((Board[2][0]==Board[1][1]))
                        if (Board[(int)2/3][2%3] !='X' && Board[(int)2/3][2%3]!='O')
                            return 2;
                    if ((Board[2][0]==Board[0][2]))
                        if (Board[(int)4/3][4%3] !='X' && Board[(int)4/3][4%3]!='O')
                            return 4;
                    if ((Board[0][2]==Board[1][1]))
                        if (Board[(int)6/3][6%3] !='X' && Board[(int)6/3][6%3]!='O')
                            return 6;
                    flag1=true;
                }

                if((Board[i][j]==Board[i][(j+1)%3]))
                    if ((Board[i][(j+2)%3]!='X') && (Board[i][(j+2)%3]!='O'))
                        return ((i*3)+((j+2)%3));
                if ((Board[i][j]==Board[(i+1)%3][j]))
                    if ((Board[(i+2)%3][j]!='X') && (Board[(i+2)%3][j]!='O'))
                        return (((i+2)%3)*3) +j;
            }
        }
    for (int i=0;i<3;i++)
        for (int j=0;j<3;j++)
        {
            if (Board[i][j]==Player[1])
            {
                if (!flag2)
                {
                    if ((Board[0][0]==Board[1][1]))
                        if (Board[(int)8/3][8%3] !='X' && Board[(int)8/3][8%3]!='O')
                            return 8;
                    if ((Board[1][1]==Board[2][2]))
                        if (Board[(int)0/3][0%3] !='X' && Board[(int)0/3][0%3]!='O')
                            return 0;
                    if ((Board[0][0]==Board[2][2]))
                        if (Board[(int)4/3][4%3] !='X' && Board[(int)4/3][4%3]!='O')
                            return 4;
                    if ((Board[2][0]==Board[1][1]))
                        if (Board[(int)2/3][2%3] !='X' && Board[(int)2/3][2%3]!='O')
                            return 2;
                    if ((Board[2][0]==Board[0][2]))
                        if (Board[(int)4/3][4%3] !='X' && Board[(int)4/3][4%3]!='O')
                            return 4;
                    if ((Board[0][2]==Board[1][1]))
                        if (Board[(int)6/3][6%3] !='X' && Board[(int)6/3][6%3]!='O')
                            return 6;
                    flag2=false;
                }

                if((Board[i][j]==Board[i][(j+1)%3]))
                    if ((Board[i][(j+2)%3]!='X') && (Board[i][(j+2)%3]!='O'))
                        return ((i*3)+((j+2)%3));
                if ((Board[i][j]==Board[(i+1)%3][j]))
                    if ((Board[(i+2)%3][j]!='X') && (Board[(i+2)%3][j]!='O'))
                        return (((i+2)%3)*3) +j;
            }
        }
	return (rand()%9);

}
void run_server()
{
	//intialize the socket
	int listenfd,acceptfd,n;
	sockaddr_in servaddr;
	struct Message rec,send;
	if((listenfd=socket(AF_INET,SOCK_STREAM,0))<0)
	{
		perror("Error 100: Socket Initailization error");
		exit(EXIT_FAILURE);
	}
	bzero(&servaddr,sizeof(servaddr));
	servaddr.sin_family=AF_INET;
	servaddr.sin_port=htons(PORT);
	servaddr.sin_addr.s_addr=htonl(INADDR_ANY);
	bind(listenfd,(struct sockaddr*)&servaddr,sizeof(servaddr));
	cout<<"X and O Server intialized.\nAwaiting socket pair connection"<<endl;
	listen(listenfd,1);
	if((acceptfd=accept(listenfd,NULL,NULL))<0)
	{
		perror("Error 101: Socket client accept error");
		exit(EXIT_FAILURE);
	}
	cout<<"CLient connected...\n";
	do
	{
        if (rand()%2)
        {
            while(!isValid((n=AIplay())));
            Board[(int)n/3][n%3]=Player[0];
        }
        send=prepMessage((int)Player[1]);
        if ((n=write(acceptfd,&send,sizeof(send)))<0)
        {
            perror("Error 102: Initializing client console");
            exit(EXIT_FAILURE);
        }
        while (1)
        {
            if (isWin())
                break;
            if(!isChoice())
                break;
            if ((n=read(acceptfd,&rec,sizeof(rec)))<0)
            {
                perror("Error 104: Receiving client choice");
                exit(EXIT_FAILURE);
            }
            if (rec.cmd==998)
            {
                cout<<"Server Terminating...\n";
                exit(EXIT_SUCCESS);
            }
            while (!isValid(rec.cmd))
            {
                send=prepMessage(-1);
                if ((n=write(acceptfd,&send,sizeof(send)))<0)
                {
                    perror("Error 105: Message not sent");
                    exit(EXIT_FAILURE);
                }
                if ((n=read(acceptfd,&rec,sizeof(rec)))<0)
                {
                    perror("Error 106: Receiving client choice");
                    exit(EXIT_FAILURE);
                }
            }
            Board[(int)rec.cmd/3][rec.cmd%3]=Player[1];
            if (isWin())
                break;
            if(!isChoice())
                break;

            while(!isValid((n=AIplay())));
            Board[(int)n/3][n%3]=Player[0];

            if (isWin())
                break;
            if(!isChoice())
                break;

            send=prepMessage(1);
            if (write(acceptfd,&send,sizeof(send))<0)
            {
                perror("Error 107: Sending to CLinet error.");
                exit(EXIT_FAILURE);
            }
        }
        send=prepMessage(10+winner);
        if ((n=write(acceptfd,&send,sizeof(send)))<0)
        {
            perror("Error 108: Win Message not sent");
            exit(EXIT_FAILURE);
        }
        if (read(acceptfd,&rec,sizeof(rec))<0)
        {
            perror("Error 109: REstart Error comand");
            exit(EXIT_FAILURE);
        }
        if (rec.cmd==0)
        {
            init();
        }

    }while(rec.cmd==0);
	cout<<"Server TErminating...\n";
	close(listenfd);
	close(acceptfd);
	return;
}

int main(int argv, char** agrc)
{
	srand(time(NULL));
	init();
	run_server();
    return EXIT_SUCCESS;
}

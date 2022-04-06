#include <iostream>
#include <cstdlib>
#include <cstring>
#include <cstdio>
#include "errno.h"

#include "unistd.h"
#include "sys/socket.h"
#include "arpa/inet.h"
#include "netinet/in.h"

#define PORT 13000
#define SERVER "127.0.0.1"

struct Message{
	int cmd;
	char board[3][3];
};

using namespace std;
char Player;
void print(char B[3][3])
{
    for (int i=0;i<3;i++)
    {
        for (int j=0;j<3;j++)
        {
            if (j!=0)
                cout<<"|";
            if ((B[i][j]=='X')||(B[i][j]=='O'))
                cout<<B[i][j];
            else
                cout<<((int)B[i][j]+1);
        }
        cout<<endl;
    }
    return;
}
int choice()
{
    int temp;
    cout<<"Type 999 to EXIT.\nEnter the number for position you want to place you "<<Player<<": ";
    cin>> temp;
    if (temp==999)
        return temp;
    while ((temp<1)||(temp>9))
    {
        cout<<"Error in inputted option. Try Again: ";
        cin>> temp;
        if (temp==999)
            return temp;
    }
    return temp;
}
int main()
{
    int sockfd,value;
    bool flag=false;
    sockaddr_in servaddr;
    struct Message rec, send;
    cout<<"X and O client initializing.\n";
    if((sockfd=socket(AF_INET,SOCK_STREAM,0))<0)
    {
        perror("Socket creation Error");
        return EXIT_FAILURE;
    }
    bzero(&servaddr,sizeof(servaddr));
    servaddr.sin_family=AF_INET;
    servaddr.sin_port=htons(PORT);
    if(inet_pton(AF_INET,SERVER,&servaddr.sin_addr)<=0)
    {
        perror("Arpa net server assign error");
        return EXIT_FAILURE;
    }
    if(connect(sockfd,(struct sockaddr *)&servaddr,sizeof(servaddr))<0)
    {
        cout<<SERVER;
        perror("Connection error");
        return EXIT_FAILURE;
    }
    cout<<"Connected to Server...\n";
    do
    {
        if(read(sockfd,&rec,sizeof(rec))<0)
        {
            perror("Error 102: Innitialization error");
            return EXIT_FAILURE;
        }
        Player=(char)rec.cmd;
        rec.cmd=0;
        while (1)
        {
            switch(rec.cmd)
            {
                case -1:
                    cout<<"Choice inputted is not valid\n";
                    break;
                case 0:
                case 1:
                    break;

                case 9:
                case 10:
                case 11:
                    print(rec.board);
                    cout<<"Game Over\n";
                    if (rec.cmd==9)
                        cout<<"It is a draw\n";
                    else if (rec.cmd==10)
                        cout<<"Server CPU wins\n";
                    else
                        cout<<"You WIN\n";
                        flag=true;
                        break;
                default:
                    cout<<"Server response not supported";
                    return EXIT_FAILURE;
            }
            if (flag)
                break;
            print(rec.board);
            send.cmd=choice()-1;
            if (write(sockfd,&send,sizeof(send))<0)
            {
                perror("Error 103: Error replying to server");
                return EXIT_FAILURE;
            }
            if (send.cmd==998)
            {
                cout<<"Thank you for playing...\n";
                return EXIT_SUCCESS;
            }
            if (read(sockfd,&rec,sizeof(rec))<0)
            {
                perror("Error 104: Error obtain response from server");
                return EXIT_FAILURE;
            }
        }
        cout<<"Type 0 to play again: ";
        cin>>value;
        send.cmd=value;
        if (write(sockfd,&send,sizeof(send))<0)
        {
            perror("Error 105: Restart command error");
            return EXIT_FAILURE;
        }
        if (value==0)
        {
            cout<<endl;
            cout<<"New Game\n";
            flag=false;
        }
    }while(value==0);
    cout << "Thank you for playing..." << endl;
    close(sockfd);
    return 0;
}

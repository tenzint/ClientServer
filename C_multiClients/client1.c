/*
    C ECHO client example using sockets
*/
#include <stdio.h> //printf
#include <string.h>    //strlen
#include <sys/socket.h>    //socket
#include <arpa/inet.h> //inet_addr
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>

/*
int select(int n, fd_set *readfds, fd_set *writefds, fd_set *exceptfds,
           struct timeval *timeout);

/*
FD_SET(int fd, fd_set *set);
FD_CLR(int fd, fd_set *set);
FD_ISSET(int fd, fd_set *set);
FD_ZERO(fd_set *set);*/
/*
void FD_CLR(int fd, fd_set *set);
int  FD_ISSET(int fd, fd_set *set);
void FD_SET(int fd, fd_set *set);
void FD_ZERO(fd_set *set);*/

#define LOGIN 1
#define LO_ACK 2
#define LO_NAK 3            
#define EXIT 4
#define JOIN 5
#define JN_ACK 6
#define JN_NAK 7
#define LEAVE_SESS 8
#define NEW_SESS 9
#define NS_ACK 10
#define MESSAGE 11
#define QUERY 12
#define QU_ACK 13
#define MAX_NAME 100
#define MAX_DATA 1000
#define MAX_DATA2 2000
#define STDIN 0

struct lab3 {
      int type;
      int size;
      char source[MAX_NAME];
      char data[MAX_DATA];
      char session_id[MAX_DATA];
      char msg[MAX_DATA];
};
 
int main(int argc , char *argv[])
{
    int sock;
    int n;
    struct sockaddr_in server;
    char message[1000] , server_reply[2000], tempmsg[1000];
    struct lab3 lab3message;
    lab3message.type = 0;
    lab3message.size=0;
    memset(lab3message.source,'\0',MAX_NAME);
    memset(lab3message.data,'\0',MAX_DATA);
    memset(lab3message.session_id,'\0',MAX_DATA);
    memset(lab3message.msg,'\0',MAX_DATA);
    memset(tempmsg,'\0',1000);
    
    int loginbit=0;

//---------------------------NEW------------

    fd_set readfds;
    fd_set writefds;
    struct timeval tv;
    int retval;
    int retval2;
//-----------------------------------------   


START:
    //first need login
    memset(message,'\0',1000);
    printf("Please log in: \n");
    printf(">>");
    gets(message);
    if(strcmp(message,"\n")==0)
    {
        printf("here");
         goto START;   
    }
        
    int count = 0;
    char *temp;
    temp = strtok (message," ");
   if(strcmp(temp,"/login")!=0 )
        goto START;

    temp = strtok (NULL, " ");
    
    char client_id[strlen(temp)];
    strcpy (client_id,temp);
    strcpy(lab3message.source,temp);
    temp = strtok (NULL, " ");

    char password[strlen(temp)];
    strcpy (password,temp);
    temp = strtok (NULL, " ");

    char server_ip[strlen(temp)];
    strcpy (server_ip,temp);
    temp = strtok (NULL, " ");

    char server_port[strlen(temp)];
    strcpy (server_port,temp);
    temp = strtok (NULL, " ");

    /*printf("client_id: %s\n", client_id);
     printf("pw: %s\n", password);
      printf("server_ip: %s\n", server_ip);
       printf("server_port: %s\n", server_port);
*/

    //Create socket
    sock = socket(AF_INET , SOCK_STREAM , 0);
    if (sock == -1)
    {
        printf("Could not create socket");
    }
    puts("Socket created");
    bzero (&server, sizeof(server));
   // server.sin_addr.s_addr = inet_addr(server_ip);
    server.sin_family = AF_INET;
    //server.sin_port = htons(atoi(server_port));
    server.sin_port = htons(atoi(server_port));

    inet_pton (AF_INET, server_ip, &server.sin_addr);
 
    //Connect to remote server
    if (connect(sock , (struct sockaddr *)&server , sizeof(server)) < 0)
    {
        perror("connect failed. Error");
        return 1;
    }
     
    puts("Connected\n");


    //create message
    lab3message.type = LOGIN; 
    lab3message.size = strlen(password)+strlen(client_id);
    char realmsg[2000];

    sprintf(realmsg, "%d#%d#%s#%s:%s\0",lab3message.type,lab3message.size,lab3message.source,client_id,password);
    //printf("here1\n");
  //  printf("realmsg: %s\n", realmsg);
     if( send(sock , realmsg , strlen(realmsg) , 0) < 0)
        {
            puts("Send failed");
            return 1;
        }
         //printf("here2\n");
        //Receive a reply from the server
        if( recv(sock , server_reply , 2000 , 0) < 0)
        {
            puts("recv failed");
            return 1;
        }
//printf("server reply: %s\n", server_reply);

        temp = strtok (server_reply,"#");

        //if LO_NACK, needto re-login
        if(strcmp(temp,"3")==0){
            temp = strtok (NULL, "#");
            printf("LO_NAK: %s\n",temp);
            goto START;
        }
        else if(strcmp(temp,"2")==0)
          //  printf("LO_ACK\n");

        loginbit=1;
        int sessionTalk=0;
        char session_id[MAX_NAME];
     
    //keep communicating with server
    //while strtok alive...
again: 
    printf("You've logged in, what do you want to do?\n");
  //  printf(">>");
   
    //printf(">>");
    while(loginbit)
    {
         //----------------------------NEW------------------------
                                            // clear the set ahead of time
                                            FD_ZERO(&readfds);
                                            FD_ZERO(&writefds);
                                            // add our descriptors to the set
                                            FD_SET(sock, &readfds);
                                            FD_SET(sock, &writefds);
                                            FD_SET(STDIN, &readfds);
                                            n=sock+1;

                                            /* Wait up to five seconds. */
                                            tv.tv_sec = 1;
                                            tv.tv_usec = 0;
    //-------------------------------------------------
        //reset arrays
        memset(message,'\0',1000);
        memset(realmsg,'\0',2000);
        memset(server_reply,'\0',2000);
        memset(lab3message.data,'\0',MAX_DATA);
        memset(lab3message.msg,'\0',MAX_DATA);
        memset(tempmsg,'\0',1000);

        lab3message.size=0;

        //if there's something in recv and you've joined a session, allow for the server to 
        //post the messages clients are posting

       
       // retval = select(n, &readfds, &writefds, NULL, &tv);
        retval = select(n, &readfds, NULL, NULL, &tv);
       // retval2 = select(n, NULL, &writefds, NULL, &tv);
        
      //  printf("1: retval: %d\n", retval);
      //  printf("1: retval2: %d\n", retval2);
        
       
            //printf("here3\n");
           
            if (retval == -1 )
                perror("select()");

            else if (retval){

                if(FD_ISSET(sock, &readfds))
                { 
                  //  printf("inretval1\n");
                  //  printf("Data is available now.\n");
                    if( recv(sock , server_reply , 2000 , 0) < 0){
                        puts("recv failed");
                        break;
                        }

                      //  puts("Server reply1 :");
                        //puts(server_reply);

                    char *temp;           
                    temp = strtok (server_reply,"#");
                    //LO_NACK
                    if(strcmp(temp,"3")==0){
                        temp = strtok (NULL, "#");
                        printf("LO_NAK: %s\n",temp);
                        goto START;
                    }//LO_ACK
                    else if(strcmp(temp,"2")==0)
                        printf("LO_ACK\n");
                    //Join ACK=6
                    else if(strcmp(temp,"6")==0){
                        temp = strtok (NULL, "#");
                        printf("Joined session %s\n",temp);
                        strcpy(lab3message.session_id,temp);
                        sessionTalk=1;
                    }
                    //JN_NAK
                    else if(strcmp(temp,"7")==0){
                         temp = strtok (NULL, "#");
                        printf("Not able to join session %s\n",temp);
                        temp = strtok (NULL, "#");
                        printf("Reason: %s", temp);
                    }//NS_ACK
                    else if(strcmp(temp,"10")==0){
                        temp = strtok (NULL, "#");
                        printf("Created new session %s\n",temp);
                        strcpy(lab3message.session_id,temp);
                        sessionTalk=1;
                    }//QU_ACK
                    else if(strcmp(temp,"13")==0){
                        temp = strtok (NULL, "#");
                        printf("Users and sessions: %s\n",temp);
                    }
                     else if(strcmp(temp,"11")==0){
                        temp = strtok (NULL, "#");
                        printf("msg: %s\n",temp);
                    }
                }

                if(FD_ISSET(STDIN, &readfds))
                    {
                       // printf("inretval2\n");
                        gets(message);

                       // printf("msg: %s\n", message);
                        //save for message text
                        strcpy(tempmsg,message);

                        char *temp;
                        temp = strtok (message," ");
                       
                        if(strcmp(temp,"/logout")==0)
                        {
                            lab3message.type=EXIT;
                            loginbit=0;
                            sessionTalk=0;
                            memset(lab3message.session_id,'\0',MAX_DATA); 
                        }

                        //printf("\n\nClient to server: %s\n\n\n", realmsg);

                        else if(strcmp(temp,"/joinsession")==0)
                        {
                            lab3message.type=JOIN;  
                           // sessionTalk=1;
                          //  printf("here bitch\n");
                        }

                        else if(strcmp(temp,"/leavesession")==0)
                        {
                            lab3message.type=LEAVE_SESS;    
                            sessionTalk=0; 
                            memset(lab3message.session_id,'\0',MAX_DATA); 
                        }

                        else if(strcmp(temp,"/createsession")==0)
                        {
                            lab3message.type=NEW_SESS;
                          //  sessionTalk=1;
                        }

                        else if(strcmp(temp,"/list")==0)
                        {
                            lab3message.type=QUERY;
                        }

                        else if(strcmp(temp,"/quit")==0)
                        {
                            loginbit=0;
                            sessionTalk=0;
                            break;
                        }

                        else if(strcmp(temp,"/logout")!=0 && strcmp(temp,"/joinsession")!=0 
                            && strcmp(temp,"/leavesession")!=0 && strcmp(temp,"/createsession")!=0
                            && strcmp(temp,"/list")!=0 && strcmp(temp,"/quit")!=0 ){

                            if(sessionTalk==1){
                                //printf("in msg centre\n");
                                lab3message.type=MESSAGE;
                               // printf("message: %s\n", tempmsg);
                                strcpy(lab3message.msg,tempmsg);
                                // printf("lab3message.msg: %s\n", tempmsg);
                              //   printf("lab3message.session_id: %s\n", lab3message.session_id);
                                sprintf(lab3message.data,"%s:%s",lab3message.session_id,tempmsg); 
                               // printf("lab3message.data: %s\n", lab3message.data);  
                                }

                            else{
                                printf("You need to join/create a session first to talk!\n");
                                goto again;
                                }

                            goto skip;
                        }
                        temp = strtok (NULL, " ");
                        if(temp!=NULL)
                            strcpy(lab3message.data,temp);
                        lab3message.size = strlen(lab3message.data);

                    skip:

                        //create message
                        sprintf(realmsg, "%d#%d#%s#%s\0",lab3message.type,lab3message.size,lab3message.source,lab3message.data);
                      
                      //printf("\n\nClient to server: %s\n\n\n", realmsg);

                      if( send(sock , realmsg , strlen(realmsg) , 0) < 0 ){
                        puts("Send failed");
                        return 1;
                        }
                          //retval = select(n, &readfds, NULL, NULL, &tv);
                        //retval2 = select(n, NULL, &writefds, NULL, &tv);
                       // printf("2: retval1: %d\n", retval);
                      //  printf("2: retval2: %d\n", retval2);
                    //    if( recv(sock , server_reply , 2000 , 0) < 0){
                     //   puts("recv failed");
                     //   break;
                     //   }

                     //   puts("Server reply1 :");
                       // puts(server_reply);


                  //   printf(">>");

                    //retval = select(n, &readfds, NULL, NULL, &tv);     
                     }
            
            //get new msg after displaying what was received
           // printf("here3\n");
           // printf(">>");
          //  gets(message);
        }

       // break;
      
    }
    char reply;
    printf("Do you want to log in again? y/n\n");
    scanf("%c", &reply);

   
     close(sock);

      if(reply=='y')
       goto START;

    return 0;
}


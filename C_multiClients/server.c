/*
    C socket server example, handles multiple clients using threads
*/
 
#include <stdio.h>
#include <string.h>    //strlen
#include <stdlib.h>    //strlen
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h> //inet_addr
#include <unistd.h>    //write
#include <pthread.h> //for threading , link with lpthread
#include <math.h>
#include <stdbool.h>

// structure for user ID's reference by server; hardcoded
struct ref_info
{
	char *ID;
	char *pw;
};

// up-to-date info about user ID and his/her session, IP address and port number. ID may not be unique.
struct active_info
{
	char *ID;
	char *session;
	
	int IP;
	int port;
	
	int sock;
};

struct node
{
	struct active_info *info;
	struct node *next;
};
struct socklist
{
	int sock;
	struct socklist *next;
};

//struct ref_info ref_array[10];
char ref_array[10][10];					// 2nd [10] is the length of the string
struct node *head;

//the thread function
void *connection_handler(void *);
void boot_server();

//helper functions
bool is_in_ref(char *ID, char *pw);
void insert_node (struct active_info *info);
struct active_info *get_node (char *ID, char *session);
bool session_exists (char * session);
bool session_self (char *ID, char * session);
struct socklist *socks_in_sess (char *ID, char *session);
void rm_node (char *ID);
void rm_node_sess (char *ID, char *sess);


int main(int argc , char *argv[])
{
	printf("starting...\n");
	if (argc > 2)
	{
		printf("More than 2 arguments. Enter 'server port_num'\n");
		return -1;
	}
	
    int socket_desc , client_sock , c , *new_sock;
    struct sockaddr_in server , client;
    int result;
	
	
	boot_server();
	 
    //Create socket
    socket_desc = socket(AF_INET , SOCK_STREAM , 0);
    if (socket_desc == -1)
    {
        printf("Could not create socket");
    }
     
    //Prepare the sockaddr_in structure
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons( atoi (argv[1]) );
     
    //Bind
    if( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0)
    {
        //print the error message
        perror("bind failed. Error\n");
        return -1;
    }
    
	int num_clients=20; 			// max num of clients possible.
	result = 0;
    //Listen
    result = listen(socket_desc , num_clients);
    if (result < 0)
	{
		perror("listen failed. Error\n");
		return -1;
	}
    //Accept an incoming connection
    puts("Waiting for incoming connections...\n");
    c = sizeof(struct sockaddr_in);
     
	
    while( (client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c)) )
    {
        puts("Connection accepted");
         
        pthread_t sniffer_thread;
        new_sock = malloc(4);
        *new_sock = client_sock;
         
        if( pthread_create( &sniffer_thread , NULL ,  connection_handler , (void*) new_sock) < 0)
        {
            perror("could not create thread");
            return 1;
        }
         
        //Now join the thread , so that we dont terminate before the thread
        //pthread_join( sniffer_thread , NULL);
        puts("Handler assigned");
    }
     
    if (client_sock < 0)
    {
        perror("accept failed");
        return -1;
    }
     
    return 0;
}
 
void boot_server()
 {
	head = NULL;
	//ref_array = (struct ref_info **)malloc((sizeof (struct ref_info*)) * 10);
	char *temp[10];
	
	strcpy ((ref_array[0]), "tenzin");
	strcpy ((ref_array[1]), &"tenzpass");
	
	strcpy ((ref_array[2]), "will");
	strcpy (ref_array[3], "willspass");
	
	strcpy (ref_array[4], "someguy");
	strcpy (ref_array[5], "pass1");
	
	strcpy (ref_array[6], "someguy4");
	strcpy (ref_array[7], "pass4");
	
	
 }
 
 
/*
 * This will handle connection for each client
 * */
void *connection_handler(void *socket_desc)
{
	int *sock = (int *)socket_desc;
    int read_size, send_size;
    char *message , client_msg[2000], *session;
	struct active_info *info;
    struct socklist *sess_socks;
	char *data, *context;
	char *ID, *pw;
	int size, new_sess;
	while(1)
	{
		//Receive a message from client
		memset(client_msg,'\0',2000); 
		read_size = recv(*sock , client_msg , 2000 , 0);
		
		if(read_size == 0)
		{
			printf("read size = 0\n");
		}
		else if(read_size == -1)
		{
			perror("recv failed");
			return NULL;
		}
		data = strtok_r(client_msg, "#", &context);				// get 'TYPE'
		if (data != NULL)
		{
			if(0==strcmp(data, "1"))			// lOGIN
			{
				printf("---LOGIN---\n");
				data = strtok_r(NULL, "#", &context);			// get 'SIZE' in string
				if(data==NULL)
				{
					perror ("Error: data is NULL");
					
				}
				
				// convert into int
				size = atoi(data);
				ID = strtok_r(NULL, "#", &context);			// get 'SOURCE' --- ID of the sender client
				
				data = strtok_r(NULL, "#", &context);			// get 'DATA'
				
				ID = strtok_r(NULL, ":", &data);
				if (is_in_ref(ID, data))
				{
					
					send_size = send(*sock , "2" , 2 , 0);				// LO_ACK = 2
					if (send_size ==-1)
					{
						printf("Error: ack not sent!\n");
						free(socket_desc);
						return NULL;
					}
				}
				else
				{
					//strcpy(data, "LO_NAK#20#server#0");
					strcpy(data, "3#ID and/or password not in the server's database\n");
					send_size = send(*sock , data , strlen(data) , 0);				// LO_NAK = 3
					if (send_size ==-1)
					{
						printf("Error: Nak not sent!\n");
						free(socket_desc);
						return NULL;
					}
				}
				
			}
			else if(0==strcmp(data, "4"))				// EXIT
			{
				printf("---Exit---\n");
				data = strtok_r(NULL, "#", &context);			// get 'SIZE' in string
				if(data==NULL)
				{
					perror ("Error: data is NULL");
					
				}
				// convert into int
				size = atoi(data);
				ID = strtok_r(NULL, "#", &context);			// get 'SOURCE' --- ID of the sender client
				
				rm_node(ID);
				//Free the socket pointer
				free(socket_desc);
				 
				return NULL;
			}
			else if(0==strcmp(data, "5"))				// JOIN
			{
				
				printf("---JOIN---\n");
				data = strtok_r(NULL, "#", &context);			// get 'SIZE' in string
				if(data==NULL)
				{
					perror ("Error: data is NULL");
					
				}
				// convert into int
				size = atoi(data);
				
				ID = strtok_r(NULL, "#", &context);			// get 'SOURCE' --- ID of the sender client
				
				//data = strtok_r(NULL, "#", &context);			// get 'DATA'
				// context remains with data
				
				// returns bool
				if (session_exists(context))
				{
					if (session_self(ID, context))
					{
						sprintf(data, "7#the existing session is made by the same user.");
					}
					else
					{
						info = malloc(sizeof(struct active_info));
						info->ID=malloc(strlen(ID));
						info->session=malloc(strlen(context));
						strcpy(info->ID,ID);
						strcpy(info->session, context);
					
						info->sock = *sock;
						insert_node(info);
						//strcpy(data, "JN_ACK#20#server#1");
						sprintf(data, "6#%s", context);
					}
					
					send_size = send(*sock , data , strlen(data) , 0);				// JN_ACK=6
					if (send_size ==-1)
					{
						printf("Error: ack not sent!\n");
						free(socket_desc);
						return NULL;
					}
				}
				else
				{
					
					sprintf(data, "7#session doesn't exist.");
					send_size = send(*sock , data , strlen(data) , 0);				// JN_NAK
					if (send_size ==-1)
					{
						printf("Error: Nak not sent!\n");
						free(socket_desc);
						return NULL;
					}
				}
				
			}
			else if(0==strcmp(data, "8"))					// LEAVE_SESS
			{
				printf("---LEAVE SESSION---\n");
				data = strtok_r(NULL, "#", &context);			// get 'SIZE' in string
				if(data==NULL)
				{
					perror ("Error: data is NULL");
					
				}
				
				// convert into int
				size = atoi(data);
				ID = strtok_r(NULL, "#", &context);			// get 'SOURCE' --- ID of the sender client
				
				session = strtok_r(NULL, "#", &context);			// get 'SOURCE' --- ID of the sender client
				rm_node_sess (ID, session);
			}
			else if(0==strcmp(data, "9"))					// NEW_SESS
			{
				
				printf("---NEW Session---\n");
				data = strtok_r(NULL, "#", &context);			// get 'SIZE' in string
				if(data==NULL)
				{
					perror ("Error: data is NULL");
					
				}
				
				// convert into int
				size = atoi(data);
				
				ID = strtok_r(NULL, "#", &context);			// get 'SOURCE' --- ID of the sender client
				
				session = strtok_r(NULL, "#", &context);			// get 'DATA'
				
				// new session ID
				if(session_exists(session))
				{
					// session already exists, send NAK
					data = malloc(5);
					
					sprintf(data, "14#");
					send_size = send(*sock , data , strlen(data) , 0);				// NS_NAK = 14
					if (send_size ==-1)
					{
						printf("Error: Nak not sent!\n");
						free(socket_desc);
						return NULL;
					}
				}
				else
				{
					// session is now unique, load the new session in linked list!
					
					struct active_info *newnode;
					newnode = malloc (sizeof(struct active_info));
					newnode->ID = malloc( (strlen(ID)));
					newnode->session = malloc(strlen(session));
					
					
					strcpy(newnode->ID,ID);
					strcpy(newnode->session,session);
					newnode->sock = *sock;
					
					insert_node(newnode);
					
					// setup data to be sent for NS_ACK
					sprintf(data, "10#%s", session);
					send_size = send(*sock , data , strlen(data) , 0);				// NS_ACK
					if (send_size ==-1)
					{
						printf("Error: Nak not sent!\n");
						free(socket_desc);
						return NULL;
					}
				}
			}
			else if(0==strcmp(data, "11"))						// MESSAGE
			{
				printf("---MESSAGE---\n");
				data = strtok_r(NULL, "#", &context);			// get 'SIZE' in string
				if(data==NULL)
				{
					perror ("Error: data is NULL");
					
				}
				// convert into int
				size = atoi(data);
				
				ID = strtok_r(NULL, "#", &context);			// get 'SOURCE' --- ID of the sender client
				//message = strtok_r(NULL, "#", &context);			// get 'DATA'
				
				// MSG format: "...#sessionID:message data"
				session = NULL;
				session = strtok_r(NULL, ":", &context);
				memset(data,'\0',strlen(data));
				data = malloc(strlen(context)+4);
				sprintf(data, "11#%s", context);
				printf("\n");
				printf("session: %s, message: %s\n", session, data);
				sess_socks = socks_in_sess(ID, session);
				for( 	; sess_socks!=NULL; sess_socks = sess_socks->next)
				{
					send_size = send(sess_socks->sock , data , strlen(data) , 0);				// MSG
					if (send_size ==-1)
					{
						printf("Error: Nak not sent!\n");
						free(socket_desc);
						return NULL;
					}
				}
				//break;
			}
			else if(0==strcmp(data, "12"))					// QUERY
			{
				// QUERY request sent!!
				printf("---QUERY---\n");
				// get all users and sessions --- user1:session1#user2:session2 
				struct node *temp;
				temp = head;
				data = malloc(100);
				sprintf(data, "13#");
				
				while (temp != NULL)
				{
					sprintf(data, "%s (%s:%s) \n", data, temp->info->ID, temp->info->session);
					
					temp = temp->next;
				}
				printf("\n------------------------\ndata sent:--- %s\n------------------------\n", data);
				// now data has all the required info
				send_size = send(*sock , data , strlen(data)+1 , 0);				// QU_ACK
				if (send_size ==-1)
				{
					printf("Error: Qu_ACK not sent!\n");
					free(socket_desc);
					return NULL;
				}
			}
			else
			{
			
			}
		}
		
	}
    //Free the socket pointer
    free(socket_desc);
     
    return NULL;
}


bool is_in_ref(char *ID, char *pw)
{
	int i;
	for (i=0; i<8; i+=2)
	{
		if ( 0==strcmp(ref_array[i],ID) && 0==strcmp(ref_array[i+1],pw))
			return true;
	}
	return false;
}

void insert_node (struct active_info *info)
{
	struct node *temp, *temp2;
	if (head==NULL)
	{
		temp = malloc(sizeof (struct node));
		temp->info = info;
		temp->next = NULL;
		head = temp;
	}
	else if (head->next==NULL)
	{
		temp = malloc(sizeof (struct node));
		temp->info = info;
		temp->next = NULL;
		head->next = temp;
	}
	else
	{
		temp2 = head->next;
		while(temp2->next != NULL)
		{
			temp2 = temp2->next;
		}
		// temp2.next is now NULL
		temp = malloc(sizeof (struct node));
		temp->info = info;
		temp->next = NULL;
		temp2->next = temp;
	}
}

struct active_info *get_node (char *ID, char *session)
{
	struct node *temp;
	for (temp=head; temp!=NULL; temp = temp->next)
	{
		if (temp == NULL)
		{
			return NULL;
		}
		else if ( 0==strcmp(temp->info->ID, ID) && 0==strcmp(temp->info->session, session) )
			return (temp->info);
	}
	return NULL;
}

bool session_exists (char * session)
{
	struct node *temp;
	for (temp = head; temp!=NULL; temp = temp->next)
	{
			
			if ( 0==strcmp(temp->info->session, session))
			{
				return true;
			}
	}
	
	return false;
}
bool session_self (char *ID, char * session)
{
	struct node *temp;
	for (temp = head; temp!=NULL; temp = temp->next)
	{
			
			if ( 0==strcmp(temp->info->session, session) && 0==strcmp(temp->info->ID,ID))
			{
				return true;
			}
	}
	
	return false;
}

struct socklist *socks_in_sess (char *ID, char *session)
{
	struct node *temp;
	struct socklist *shead = malloc(sizeof(struct socklist));
	int i;
	shead = NULL;
	struct socklist *stemp, *stemp2;
	for (temp = head; temp!=NULL; temp = temp->next)
	{
		
		if ( 0 ==strcmp(temp->info->session,session)  )
		{
			if ( 0 == strcmp(temp->info->ID, ID))
				goto endside;
			
			if (shead == NULL)
			{
				shead = malloc(sizeof(struct socklist));
				shead->sock = temp->info->sock;
				shead->next = NULL;
			}
			else if (shead->next == NULL)
			{
				stemp= malloc(sizeof(struct socklist));
				stemp->sock = temp->info->sock;
				stemp->next = NULL;
				shead->next = stemp;
			}
			else
			{
				stemp2 = shead->next;
				while(stemp2->next != NULL)
				{
					stemp2 = stemp2->next;
				}
				stemp= malloc(sizeof(struct socklist));
				stemp->sock = temp->info->sock;
				stemp->next = NULL;
				stemp2->next = stemp;
			}
		}
		endside:
		i =0;
	}
	return shead;

}

void rm_node (char *ID)
{
	struct node *curr, *prev;
	
	prev = NULL;
	curr = head;
	while(curr != NULL)
	{
		if(0==strcmp(curr->info->ID,ID))
		{
			// found the node to remove!
			if(prev == NULL)
			{					// linked list only has head node
				free(curr->info);
				free(curr);
				head = NULL;
				curr = NULL;
			}
			else
			{
				prev->next = curr->next;
				// remove every entries of client's ID in the linked list
				free(curr->info);
				free(curr);
				curr = prev->next;
				
			}
		}
		else
		{
			prev = curr;
			curr = curr->next;
		}
	}
	
	
}

void rm_node_sess (char *ID, char *sess)
{
	struct node *curr, *prev;
	
	prev = NULL;
	curr = head;
	while(curr != NULL)
	{
		
		if(0==strcmp(curr->info->ID,ID) && 0==strcmp(curr->info->session,sess))
		{
			// found the node to remove!
			if(prev == NULL)
			{					// linked list only has head node
				free(curr->info);
				free(curr);
				
				head = NULL;
				curr = NULL;
			}
			else
			{
				prev->next = curr->next;
				// remove every entries of client's ID in the linked list
				free(curr->info);
				free(curr);
				curr = prev->next;
				
			}
		}
		else
		{
			prev = curr;
			curr = curr->next;
		}
	}
	
}

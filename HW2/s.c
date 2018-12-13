#include<stdio.h>
#include<string.h>    //strlen
#include<stdlib.h>    //strlen
#include<sys/socket.h>
#include<arpa/inet.h> //inet_addr
#include<unistd.h>    //write
#include<pthread.h> //for threading , link with lpthread
#include<netinet/in.h>
#define _GNU_SOURCE

#define LISTENQ 5
#define MAXLINE 512
#define MAXMEM 10
#define NAMELEN 20

int listenfd,connfd[MAXMEM];
char name[MAXMEM][NAMELEN];
void quit();
void rcv_snd(int n);

int main()
{
	pthread_t thread;
	struct sockaddr_in servaddr,cliaddr;
	socklen_t len;
	time_t ticks;
	char buff[MAXLINE];

	
	printf("Socket...\n");
	listenfd=socket(AF_INET,SOCK_STREAM,0);
	if(listenfd<0)
	{
		printf("Socket created failed.\n");
		return -1;
	}
	
	printf("Bind...\n");
	servaddr.sin_family=AF_INET;
	servaddr.sin_port=htons(6666);
	servaddr.sin_addr.s_addr=htonl(INADDR_ANY);
	if(bind(listenfd,(struct sockaddr*)&servaddr,sizeof(servaddr))<0)
	{
		printf("Bind failed.\n");
		return -1;
	}
	
	printf("listening...\n");
	listen(listenfd,LISTENQ);


	pthread_create(&thread,NULL,(void*)(&quit),NULL);
	
	int i=0;
	for(i=0;i<MAXMEM;i++)
	{
		connfd[i]=-1;
	}
	while(1)
	{
		len=sizeof(cliaddr);
		for(i=0;i<MAXMEM;i++)
		{
			if(connfd[i]==-1)
			{
				break;
			}
		}
		
		connfd[i]=accept(listenfd,(struct sockaddr*)&cliaddr,&len);
		ticks=time(NULL);
		sprintf(buff,"%.24s \r \n",ctime(&ticks));
		
		
		pthread_create(malloc(sizeof(pthread_t)),NULL,(void*)(&rcv_snd),(void*)i);

	}
	return 0;
}
void quit()
{
	char msg[10];
	while(1)
	{
		scanf("%s",msg);
		if(strcmp("quit",msg)==0)
		{
			printf("Byebye...\n");
			close(listenfd);
			exit(0);
		}
	}
}
void rcv_snd(int n)
{
	char buff[MAXLINE];
	char buff1[MAXLINE];
	char buff2[MAXLINE];
	time_t ticks;
	int i=0;
	int retval;
	char f[]="your name is ";
	char *ptr;
	char *str;
	char pmn[NAMELEN]={'\0'};
	int pmc=0;
	
	int len;
	len=read(connfd[n],name[n],NAMELEN);
	if(len>0)
	{
		name[n][len]=0;
	}
	strcat(f,name[n]);
	write(connfd[n],f,strlen(f));
	
	strcpy(buff,name[n]);
	strcat(buff,"\tjoin in\n");
	for(i=0;i<MAXMEM;i++)
	{
		if(connfd[i]!=-1)
		{
			write(connfd[i],buff,strlen(buff));
		}
	}
	
	while(1)
	{
		memset(buff1,0,sizeof(buff));
		if((len=read(connfd[n],buff1,MAXLINE))>0)
		{
			buff1[len]=0;

			
			if(strcmp("bye\n",buff1)==0)
			{
				close(connfd[n]);
				connfd[n]=-1;
				pthread_exit(&retval);
			}
			if(strcmp("/all\n",buff1)==0)
			{
				strcpy(buff,"NOW on:\n");
				for(i=0;i<MAXMEM;i++)
				{
					if(connfd[i]!=-1)
						strcat(buff,name[i]);
				}
				write(connfd[n],buff,strlen(buff));
				continue;
			}
			memset(pmn,'\0',NAMELEN);
			ptr=buff1;
			str=buff1;
			if(strstr(ptr,"pm ")==str)
			{
				
				pmc=0;
				ptr=ptr+3;
				while(*ptr!='|')
				{
					if(pmc>NAMELEN)
					{
						printf("something wrong\n");
						break;
					}
					pmn[pmc]=*ptr;
					pmc++;
					ptr++;
				}
				if(pmc>NAMELEN)continue;
				ptr++;
				pmn[pmc]='\n';
				strcpy(buff1,ptr);
				for(i=0;i<MAXMEM;i++)
				{
					
					if(strcmp(name[i],pmn)==0)
					{
						ticks=time(NULL);
						sprintf(buff2,"%.24s\r\n",ctime(&ticks));
						memset(buff,0,sizeof(buff));
						strcpy(buff,name[n]);
						strcat(buff,"\tpm you ");
						strcat(buff,buff2);
						strcat(buff,buff1);
						write(connfd[i],buff,strlen(buff));
						break;
					}
				}
				continue;				
			}
			if(strstr(ptr,"sendto ")==str)
			{
				
				pmc=0;
				ptr=ptr+7;
				while(*ptr!='|')
				{
					pmn[pmc]=*ptr;
					pmc++;
					ptr++;
				}
				ptr++;
				printf("!@%s\n",ptr);
				pmn[pmc]='\n';
				memset(buff2,0,sizeof(buff1));
				pmc=0;
				str=ptr;
				printf("strstr%s\n",str);
				while(*str!='\n')
				{
					printf("%c",*str);
					buff2[pmc]=*str;
					pmc++;
					str++;
				}
				printf("0!@#%s\n",buff2);
				for(i=0;i<MAXMEM;i++)
				{
					
					if(strcmp(name[i],pmn)==0)
					{
						write(connfd[n],"sending don't do anything until end\n",strlen("sending don't do anything until end\n"));
						memset(buff,0,sizeof(buff));
						strcpy(buff,"do you want to get file from ");
						strcat(buff,name[n]);
						strcat(buff," y/n \n");
						write(connfd[i],buff,strlen(buff));
						//memset(buff,0,sizeof(buff));
						len=read(connfd[i],buff,MAXLINE);
						buff[len]=0;
						
						if(strcmp(buff,"y\n")==0||strcmp(buff,"Y\n")==0)
						{
							
							memset(buff,0,sizeof(buff));
							FILE *ftp;
							
							printf("1\n!@%s\n",buff2);
							ftp=fopen(buff2,"r");
							
							if(ftp!=NULL)
							{
								
								write(connfd[i],"/filestart",strlen("/filestart"));
								sleep(1);
								write(connfd[i],buff2,strlen(buff2));
								sleep(1);
								printf("1\n");
								while(fgets(buff1,sizeof(buff1),ftp)!=NULL)
								{
									printf("%s",buff1);
									write(connfd[i],buff1,strlen(buff1));
									memset(buff1,0,sizeof(buff1));
									sleep(1);
								}
								write(connfd[i],"end\n",strlen("end\n"));
								write(connfd[n],"end\n",strlen("end\n"));
							}
							printf("2\n");
							fclose(ftp);
						}
						else write(connfd[n],"he don't want your file\n end\n",strlen("he don't want your file\n end\n"));
						break;
					}
				}
				continue;
			}
			ticks=time(NULL);
			sprintf(buff2,"%.24s\r\n",ctime(&ticks));

			strcpy(buff,name[n]);
			strcat(buff,"\t");
			strcat(buff,buff2);
			strcat(buff,buff1);

			for(i=0;i<MAXMEM;i++)
			{
				if(i==n)continue;
				if(connfd[i]!=-1)
				{
					write(connfd[i],buff,strlen(buff));
				}
			}
		}
	}
}


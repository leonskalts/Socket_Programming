#include "node.h"

void client(Conf conf)
{
	
	Data din=input("data.in");
	unsigned int start_timer,package_send,end_timer,elapsed_time,ind;
	char recvline[MAXLINE - 1];
	int sockfd, n,clientfd;
	char message[255];
	unsigned int count_packages=0;
	unsigned int data_len=0;
	double jitter=0;
	double owd=0;
	struct sockaddr_in servaddr;
	
	if ( (sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0 )
		printf("Socket error");
	
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(conf.port);
	
	if ( inet_pton(AF_INET, conf.server_ip, &servaddr.sin_addr) <= 0)
		printf("inet_pton error for %s", conf.server_ip);

	if ( clientfd=connect(sockfd, (struct sockaddr *) &servaddr,sizeof(servaddr)) < 0 )
		printf("Connection error:%s, TCP PORT:%u",conf.server_ip,conf.port);

	start_timer=time(NULL);
	ind=conf.experiment_interval;
	while(1) {
		for(int i=0,t=din.size;i<t;i++)
		{
			send(sockfd, din.data[i], strlen(din.data[i]), 0);
			if((n = read(sockfd, recvline, 1024))<=0) 
				break;
			recvline[n] = '\0';
			elapsed_time=time(NULL)-start_timer;
			if(elapsed_time>=ind)
			{
				send(sockfd,STATS,strlen(STATS),0);
				if((n = read(sockfd, recvline, 1024))<=0) 
				break;
				recvline[n] = '\0';
				ind*=2;
			}
		}

		elapsed_time=time(NULL)-start_timer;
		if(elapsed_time>conf.experiment_duration)
		{
			send(sockfd, CLOSEFLAG, strlen(CLOSEFLAG), 0);
			break;
		}
	}
	close(clientfd);
	clear_data_buffer(&din);
}

void server(Conf conf)
{
	Data dout=input("data.out");
	srand(time(NULL));
	int new_socket,value_read,server_fd,number_of_packages;
	struct sockaddr_in address;
	unsigned int address_len=sizeof(address);
	char buffer[1024]={0};
	int opt=1;
	char server_message[255];
	unsigned int count_packages=0;
	double data_len=0.0,jitter=0.0,owd=0.0;
	clock_t previous_timer;

	if ((server_fd= socket(AF_INET, SOCK_STREAM, 0))<0)
	printf("Socket error");
	if(setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) 
	{
		printf("setsockopt");
		exit(EXIT_FAILURE);
	}
	address.sin_family = AF_INET;
	address.sin_addr.s_addr=inet_addr(conf.server_ip);
	address.sin_port = htons(conf.port);

	// Bind Connections
	if(bind(server_fd,(struct sockaddr*)&address,sizeof(address))<0)
	{
		printf("bind failure");
		exit(EXIT_FAILURE);
	}
	if(listen(server_fd,3)<0)
	{
		printf("Listen error");
		exit(EXIT_FAILURE);
	}
	if((new_socket=accept(server_fd,(struct sockaddr*)&address,(socklen_t*)&address_len))<0)
	{
		printf("Error in accepting connections");
		exit(EXIT_FAILURE);
	}

	printf("Server side: Performance test started\n");
	printf("----------------------------------------------------\n");
	printf("Server listening on TCP PORT %u\n",conf.port);
	printf("----------------------------------------------------\n");

	printf("[ INFO] local %s port %u connected with %s port %u\n",ip_address(),conf.port,conf.server_ip,conf.port);
	printf("[   ID] Interval\tThroughput\tJitter\tOne Way delay(OWD)\n");
	unsigned int start_timer,end_timer,previous_elapsed_time=0,elapsed_previous,elapsed;
	unsigned int checkpoint=start_timer;
	while(1)
	{	
		start_timer=time(NULL);
		value_read=read(new_socket,buffer,1024);
		buffer[value_read]='\0';
		number_of_packages=send(new_socket,server_message,strlen(server_message),0);
		strcpy(server_message,dout.data[rand()%dout.size]);
		end_timer=time(NULL);
		data_len+=strlen(server_message)+value_read;
		elapsed=end_timer-start_timer;
		if(count_packages>0)
		{
			elapsed_previous=end_timer-previous_timer;
			jitter+=elapsed_previous-elapsed;
		}
		previous_timer=end_timer;
		owd+=end_timer-start_timer;
		count_packages++;
		
		if(strcmp(buffer,STATS)==0)
		{
			printf("[ INFO] %u - %u\t%.3lfKbps \t%.3lfms \t%.3lfms \n",checkpoint,time(NULL)-start_timer,(data_len*1e-3)/(end_timer-start_timer),jitter*1e-3/count_packages,owd*1e-3/(2*count_packages));
			checkpoint=time(NULL)-start_timer;
		}

		if(strcmp(buffer,CLOSEFLAG)==0)
		{
			break;
		}

		// if((time(NULL)-start_timer)>conf.experiment_duration)
		// {
		// 	break;
		// }

		// if((time(NULL)-start_timer)%conf.experiment_interval==0)
		// {
		// 	previous_elapsed_time=time(NULL)-start_timer;
		// }
	}
	close(new_socket);
	shutdown(server_fd,SHUT_RDWR);
	clear_data_buffer(&dout);
}


int main(int argc, char **argv) {
	Conf conf=configure(argc,argv);

	if(strcmp(conf.dedicated,"client")==0)
	{
		client(conf);
	}
	else if(strcmp(conf.dedicated,"server")==0)
	{
		server(conf);
	}	
	exit(0);
}

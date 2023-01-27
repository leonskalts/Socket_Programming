#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>
#include <math.h>
#include <assert.h>
#ifdef linux
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/uio.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <net/if.h>
#include <unistd.h>
#include <arpa/inet.h>
#endif
#define MAXLINE 1000
#define STATS "stats\n"
#define CLOSEFLAG "exit\n"
#ifdef linux
const char sep='/';
#else
const char sep='\\';
#endif


typedef struct
{
	char server_ip[255];
	char dedicated[255];
	int port;
	unsigned int experiment_interval;
	unsigned int experiment_duration;
	int sizeof_packages;
}Conf;

typedef struct 
{
	int size;
	char **data;
}Data;


Data input(char *filename)
{
	Data din;
	char line[255];
	din.size=0;
	FILE *fp=fopen("data.in","r");
	while(fgets(fp,line,255)!=NULL)
	{
		din.size++;
	}
	fclose(fp);
	din.data=(char**)malloc(sizeof(char *)*din.size);
	for(int i=0;i<din.size;i++)
	{
		din.data[i]=(char *)malloc(sizeof(char)*255);
	}

	fp=fopen("data.in","r");
	int c=0;
	while(fgets(fp,line,255)!=NULL)
	{
		strcpy(din.data[c],line);
		c++;
	}
	fclose(fp);
	return din;
}




Conf configure(int argc,char **argv)
{
	int argcounter=1;
	Conf conf; 
	while(argcounter<argc)
	{
		if(strcmp(argv[argcounter],"--s")==0)
		{
			strcpy(conf.dedicated,"server");
			argcounter++;	
		}
		else if(strcmp(argv[argcounter],"--p")==0)
		{
			conf.port=atoi(argv[++argcounter]);
			argcounter++;
		}	
		else if(strcmp(argv[argcounter],"--c")==0)
		{
			strcpy(conf.dedicated,"client");
			argcounter++;
		}
		else if(strcmp(argv[argcounter],"--a")==0)
		{
			strcpy(conf.server_ip,argv[++argcounter]);
			argcounter++;
		}
		else if(strcmp(argv[argcounter],"--t")==0)
		{
			conf.experiment_duration=atoll(argv[++argcounter]);
			argcounter++;
		}
		else if(strcmp(argv[argcounter],"--l")==0)
		{
			conf.sizeof_packages=atoi(argv[++argcounter]);
			argcounter++;
		}
		else if(strcmp(argv[argcounter],"--i")==0)
		{
			conf.experiment_interval=atoll(argv[++argcounter]);
			argcounter++;
		}
	}
	return conf;
}

char* ip_address()
{
	int n;
    struct ifreq ifr;
    char array[] = "eth0";
    n = socket(AF_INET, SOCK_DGRAM, 0);
    //Type of address to retrieve - IPv4 IP address

    ifr.ifr_addr.sa_family = AF_INET;
    //Copy the interface name in the ifreq structure
    strncpy(ifr.ifr_name , array , IFNAMSIZ - 1);
    ioctl(n, SIOCGIFADDR, &ifr);
    close(n);
	return inet_ntoa(( (struct sockaddr_in *)&ifr.ifr_addr )->sin_addr);
}

void clear_data_buffer(Data *d)
{
	for(int i=0;i<d->size;i++)
	{
		free(d->data[i]);
	}
	free(d->data);
}
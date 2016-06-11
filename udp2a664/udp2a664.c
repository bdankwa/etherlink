/*
 ***************************************************************************************************
 ** 				Copyright 2016 Rolls-Royce Controls and Data Services Limited.
 ** 	The information in this document is the property of Rolls-Royce Controls and Data Services
 ** 		Limited and may not be copied or communicated to a third party, or used for
 ** 	any purpose other than for which it was supplied without the express written consent of
 ** 						Rolls-Royce Controls and Data Services Limited.
 ***************************************************************************************************/
 
#include <stdio.h>
#include <string.h> 
#include <stdlib.h> 
#include <libgen.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <netinet/udp.h>   //Provides declarations for udp header
#include <netinet/ip.h>    //Provides declarations for ip header
#include <linux/if_packet.h>
#include <netinet/ether.h>
#include <net/if.h>

#define BUFLEN 2000        //Max length of buffer
#define MAXPORTS (127)     //Max number of openable ports

struct pseudo_header
{
    u_int32_t source_address;
    u_int32_t dest_address;
    u_int8_t placeholder;
    u_int8_t protocol;
    u_int16_t udp_length;
};


int recvSockList[MAXPORTS];
int sendSockList[MAXPORTS];
char *name;
int startPort;
int portOffset = 0;
char *destAddress;
char *interfaceName;
char *spoofIP = 0;
uint8_t spoofMAC[6] = {0x02,0,0,0,0,0x10};
uint8_t destMAC[6] = {0x02,0,0,0,0,0x21};
char versionNum[] = "0.0.4";
int verbose = 0;
 
// Generic checksum calculation function
unsigned short csum(unsigned short *ptr,int nbytes) 
{
    register long sum;
    unsigned short oddbyte;
    register short answer;
 
    sum=0;
    while(nbytes>1) {
        sum+=*ptr++;
        nbytes-=2;
    }
    if(nbytes==1) {
        oddbyte=0;
        *((u_char*)&oddbyte)=*(u_char*)ptr;
        sum+=oddbyte;
    }
 
    sum = (sum>>16)+(sum & 0xffff);
    sum = sum + (sum>>16);
    answer=(short)~sum;
     
    return(answer);
} 
 
// Close all open sockets
static void closeSockets()
{
   int i;
    // Initialise the socket list
    for (i=0;i<MAXPORTS;i++) {
       if (recvSockList[i] != -1)
              close(recvSockList[i]);
       if (sendSockList[i] != -1)
              close(sendSockList[i]);
    }
}

// Die in a nice way
static void die(char *s)
{
    // Clean up
    closeSockets();
    perror(s);
    exit(1);
}




// Print usage
static void usage() {
   printf("Syntax: %s [-d destMAC] [-h] [-i ifname] [-m srcMAC] [-s IP] [-o offset] [-v] baseport numports destIP\n",name);
   printf("        -d xx:xx:xx:xx:xx:xx    - set dest MAC address (default=02:00:00:00:00:10)\n");
   printf("        -m xx:xx:xx:xx:xx:xx    - spoof source MAC address (default=02:00:00:00:00:21)\n");
   printf("        -s x.x.x.x              - spoof source IP address (default=source receive address) \n");
   printf("        -h                      - print this help and exit\n"); 
   printf("        -v                      - verbose debugging output\n");
   printf("        -o offset               - add or subtract offset to sending port number\n");
   printf("        -i ifname               - send interface (default lo)\n"); 
   printf("        baseport                - first local UDP port to listen to (>= 1024)\n");
   printf("        numports                - number of ports to listen to\n");
   printf("        destIP                  - IP address to forward the A664 packets to\n");
   printf("\n");
   printf("Version number: %s\n",versionNum);
   printf("%s listens on a range of ports and forwards those to a specified IP address with the additional A664 sequence number byte added to enable compatibility with A664 HW. The sequence number is set to zero. It must be run as root to operate on raw sockets. \n", name);
}	

static void parseMac(char *str,uint8_t *macAddress) {

   char *ptr;

   if (strlen(str) != 17) {
      printf("MAC addresses must be in the format xx:xx:xx:xx:xx:xx\n");
      usage();
      exit(1);
   }
   
   macAddress[0] = strtol(str,&ptr,16);
   macAddress[1] = strtol(ptr+1, &ptr,16);
   macAddress[2] = strtol(ptr+1, &ptr,16);
   macAddress[3] = strtol(ptr+1, &ptr,16);
   macAddress[4] = strtol(ptr+1, &ptr,16);
   macAddress[5] = strtol(ptr+1, &ptr,16);

    

} 


// Send out A664 packet corresponding to the port index
static void sendA664Packet(int index, char *srcdata, int len, int sourcePort, char *srcAddress) {
   
   struct sockaddr_ll socket_address;
   struct ifreq if_idx;
   
   // Datagram
   char datagram[4096], * data, *pseudogram;    
   struct ether_header *eh = (struct ether_header *) datagram;
   struct iphdr *iph = (struct iphdr *) (datagram + sizeof(struct ether_header));
        
   memset(datagram, 0, 4096);
   
   /* Ethernet header */
   eh->ether_shost[0] = spoofMAC[0];
   eh->ether_shost[1] = spoofMAC[1];
   eh->ether_shost[2] = spoofMAC[2];
   eh->ether_shost[3] = spoofMAC[3];
   eh->ether_shost[4] = spoofMAC[4];
   eh->ether_shost[5] = spoofMAC[5];
   eh->ether_dhost[0] = destMAC[0];
   eh->ether_dhost[1] = destMAC[1];
   eh->ether_dhost[2] = destMAC[2];
   eh->ether_dhost[3] = destMAC[3];
   eh->ether_dhost[4] = destMAC[4];
   eh->ether_dhost[5] = destMAC[5];
   eh->ether_type = htons(ETH_P_IP);
   
        
   //UDP header
   struct udphdr *udph = (struct udphdr *) (datagram + sizeof (struct ip) + sizeof(struct ether_header));
   // Pseudo header for checksum calculation
   struct pseudo_header psh;
   
   data = datagram + sizeof(struct iphdr) + sizeof(struct udphdr) + sizeof(struct ether_header);
   memcpy(data,srcdata,len);

   //Fill in the IP Header
   iph->ihl = 5;
   iph->version = 4;
   iph->tos = 0;
   iph->tot_len = htons( sizeof (struct iphdr) + sizeof (struct udphdr) + len);
   iph->id = htonl(0); //Id of this packet
   iph->frag_off = 0;
   iph->ttl = 255;
   iph->protocol = IPPROTO_UDP;
   iph->check = 0;    
         
   if (spoofIP == 0)                
      iph->saddr = inet_addr ( srcAddress );  
   else 
      iph->saddr = inet_addr ( spoofIP );  
     
   iph->daddr = inet_addr ( destAddress );
   
   
   iph->check = csum ((unsigned short *) iph, sizeof(struct iphdr));

   //Set the UDP header
   udph->source = (u_int16_t) htons( sourcePort );
   udph->dest   =  (u_int16_t) htons ( startPort + index + portOffset);
   udph->len    = (u_int16_t) htons(sizeof(struct udphdr) + len);
   udph->check    = (u_int16_t) 0;
   
   //Now the calculate the UDP checksum upon the Pseudo header
   if (spoofIP == 0)                
      psh.source_address = inet_addr( srcAddress );
   else 
      psh.source_address = inet_addr( spoofIP ); 
   psh.dest_address = inet_addr( destAddress );
   psh.placeholder = 0;
   psh.protocol = IPPROTO_UDP;
   psh.udp_length = htons(sizeof(struct udphdr) + len);
     
   int psize = sizeof(struct pseudo_header) + sizeof(struct udphdr) + len;
   pseudogram = malloc(psize);
     
   // Copy relevant information into the datagram  
   memcpy(pseudogram , (char*) &psh , sizeof (struct pseudo_header));
   memcpy(pseudogram + sizeof(struct pseudo_header) , udph , sizeof(struct udphdr) + len);

   udph->check = csum( (unsigned short*) pseudogram , psize);
   
   // Get interface index from name
   memset(&if_idx, 0, sizeof(struct ifreq)); 
   if (interfaceName == 0)
      strncpy(if_idx.ifr_name, "lo", IFNAMSIZ-1); 
   else
      strncpy(if_idx.ifr_name, interfaceName, IFNAMSIZ-1); 
   if (ioctl(sendSockList[index], SIOCGIFINDEX, &if_idx) < 0) 
 	    die("SIOCGIFINDEX"); 
   
   
   socket_address.sll_ifindex = if_idx.ifr_ifindex; 
   /* Address length*/ 
   socket_address.sll_halen = ETH_ALEN; 
 	
   /* Destination MAC */ 
   socket_address.sll_addr[0] = destMAC[0];; 
   socket_address.sll_addr[1] = destMAC[1];; 
   socket_address.sll_addr[2] = destMAC[2];; 
   socket_address.sll_addr[3] = destMAC[3];; 
   socket_address.sll_addr[4] = destMAC[4];; 
   socket_address.sll_addr[5] = destMAC[5];; 
 
   /* Add A664 Sequence Byte */
   datagram[ntohs(iph->tot_len) + sizeof(struct ether_header)] = 0x01;

   /* Send packet */ 
   if (sendto(sendSockList[index], datagram, ntohs(iph->tot_len) + sizeof(struct ether_header) + 1, 0, (struct sockaddr*)&socket_address, sizeof(struct sockaddr_ll)) < 0)
     die("sendto failed");
 
}

int main(int argc, char *argv [])
{
    struct sockaddr_in si_other;
    struct sockaddr_in *si_me;
    
    int i, slen = sizeof(si_other) , recv_len;
    char buf[BUFLEN];
    int numPorts;
    fd_set sockets;
    int maxfd = 0;
    int c;
    
    FD_ZERO(&sockets);
    
    // Initialise the socket list
    for (i=0;i<MAXPORTS;i++)
       recvSockList[i] = -1;
       
    // Squirrel away program name
    name = (char *) malloc(strlen(basename(argv[0])) + (size_t) 1);
    strcpy(name, basename(argv[0]));
       
    while ((c = getopt(argc, argv,"o:vhs:m:d:i:")) != -1) {
       switch (c) {
          case 's':
             spoofIP = malloc(strlen(optarg)+1);
             strcpy(spoofIP,optarg);
             break;
          case 'i':
             interfaceName = malloc(strlen(optarg)+1);
             strcpy(interfaceName,optarg);
             break;
          case 'm':
             parseMac(optarg,spoofMAC);
             break;
          case 'd':
             parseMac(optarg,destMAC);
             break;
          case 'o':
        	 portOffset = atoi(optarg);
        	 break;
          case 'v':
        	 verbose++;
        	 break;
          case 'h':
             usage();
             exit(0);
          default:
             usage();
             exit(1);
        }
    }  

    if ((argc - optind) != 3) {	
	    printf("%s requires three arguments.\n", argv[0]);
	    usage();
	    exit(1);
    }
    
    // Read in start port
    startPort = atoi(argv[optind]);
    
    // Sanity check - don't want to listen on low port numbers
    if (startPort < 1024) {
       printf("Port number %d too low. Port numbers must be >= 1024\n", (int) startPort);
       usage();
       exit(1);
    }
       
    numPorts = atoi(argv[1+optind]);
    
    // Sanity check - need a valid number of ports to open
    if (numPorts < 1 || numPorts > 127) {
       printf( "Port range not valid. Port range must be between 1 and 127"); 
       usage();
       exit(1);
    }
    
    // Squirrel the destination IP address
	destAddress = malloc(strlen(argv[2+optind]) + 1);
	strcpy(destAddress, argv[2+optind]);
    
       
    // Open the set of receive sockets we need and add them to the file descriptor set and
    // then bind the sockets to the port numbers
    for (i=0;i<numPorts;i++) {
        if ((recvSockList[i]=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
            die("socket");   
            
       // Create storage and zero out
       si_me = malloc(sizeof(struct sockaddr_in));
       memset(si_me, 0, sizeof(*si_me));
       
       si_me->sin_family = AF_INET;
       si_me->sin_port = htons(i+startPort);
       si_me->sin_addr.s_addr = INADDR_ANY;
            
       //bind socket to port
       if( bind(recvSockList[i] , (struct sockaddr*)si_me, sizeof(*si_me) ) == -1)
       {
        die("bind");
       }            
       
        FD_SET(recvSockList[i], &sockets);
        
        // Record the max file descriptor for the select statement
        if (recvSockList[i] > maxfd)
           maxfd = recvSockList[i];
           
    }
    
    // Open the set of send sockets we need
    for (i=0;i<numPorts;i++) {
        if ((sendSockList[i]= socket(AF_PACKET, SOCK_RAW, IPPROTO_RAW)) == -1) {
            printf("socket : Couldn't create RAW socket, are you running as root?\n");   
            die("socket");   
        }
    }
    
    int retval;
    
    
    
    // Loop forever
    while (1) {
    
    
       // Receive data from all sockets for which data is available
       if ((retval = select(maxfd + 1, &sockets, (fd_set *)0, (fd_set *)0, 0)) != 0) {
           if (retval == -1)
              die("select");    
    
          for (i=0;i<numPorts;i++) {
             if (FD_ISSET(recvSockList[i], &sockets)) {
                if ((recv_len = recvfrom(recvSockList[i], buf, BUFLEN, 0, (struct sockaddr *) &si_other, &slen)) == -1)
                   die("recvfrom()");

                if (verbose) {
                	printf("%d Bytes received from port %d.\n",recv_len,startPort+i);
                	fflush(0);
                }

                 // Send A664 packet
                 sendA664Packet(i, buf, recv_len,ntohs(si_other.sin_port),inet_ntoa(si_other.sin_addr));
              }
          }
          // Reset the FD set
          FD_ZERO(&sockets);
          for (i=0;i<numPorts;i++) {
              FD_SET(recvSockList[i], &sockets);
          }
    }
    }
}

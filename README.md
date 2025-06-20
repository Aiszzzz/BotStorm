BotStorm is a C++-based Distributed Denial of Service (DDoS) simulation tool created for educational and lab environments.  
It follows a controller-bot architecture where a central controller sends attack instructions to multiple bots.  
Each bot executes raw socket-based flood attacks, demonstrating how coordinated DDoS attacks work in a controlled setting.

> This project is strictly for educational and testing purposes in isolated lab environments.
---
## Features

- Controller-bot communication over TCP sockets
- Supports three attack types:
  - UDP Flood Attack
  - TCP SYN Flood Attack
  - ICMP Echo (Ping) Flood Attack
- Raw socket-based packet crafting
- `nmap`-based open port scanning before attack initiation
- Multi-bot synchronized attack execution
- Designed for Kali/Ubuntu Linux VMs in a local network
- `.gitignore` configured to exclude compiled binaries
---
## Installation

### Step 1: Clone the repository


```git clone https://github.com/Aiszzzz/BotStorm.git```

```cd BotStorm```

---
## Dependencies
Make sure your system has the following installed:

g++ – To compile the C++ code

Install using: ```sudo apt install g++```

nmap – Used by the controller for scanning open ports

Install using: ```sudo-apt install nmap```
 
sudo/root privileges – Required for raw socket operations (bots)

Run bots using: ```sudo ./bots/bots```

Linux OS – Recommended: Kali Linux or Ubuntu
Tested in virtual lab setups

---
## Compilation

```g++ controller/control.cpp -o controller/control```

```g++ bots/bot.cpp -o bots/bots```

---
## Configuration
Note: The bot is hardcoded to connect to the controller using 127.0.0.1 (localhost).

If you're running the bots on different virtual machines, update the IP in bots/bot.cpp:

```controllerAddr.sin_addr.s_addr = inet_addr("127.0.0.1");```

Replace 127.0.0.1 with your controller VM's actual IP address (e.g., 192.168.1.10).

---
## Usage
Step 1: Start the bots on each VM

```sudo ./bots/bots```

Step 2: Run the controller

```./controller/control```

Step 3: Follow on-screen prompts
You will be asked to:

Select attack type (UDP, TCP, ICMP)

Enter target IP address

If UDP or TCP is selected, the controller will scan and display open ports for the given IP.

For UDP/TCP: Select an open port to target

Once selected, the controller sends the attack command to all connected bots, which begin flooding the target.

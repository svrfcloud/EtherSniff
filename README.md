# EtherSniff  
### Minimal PF_PACKET Ethernet Sniffer with ARP JSON Logging  
Created by *svrfcloud & Pi (CoPilot).*

EtherSniff is a lightweight raw‑socket packet sniffer written in C using Linux’s `PF_PACKET` interface.  
It captures Ethernet frames directly from the NIC, prints them to stdout, and logs **ARP frames** into a JSON array (`frame.txt`) that remains valid even if the program is interrupted.

This project is ideal for learning:

- Low‑level Linux networking  
- Raw Ethernet frame parsing  
- ARP behavior (broadcast → unicast)  
- JSON logging from C  
- Real‑time packet inspection without libpcap  

---

## Features

- Captures all Ethernet frames using `PF_PACKET`
- Prints:
  - Source MAC  
  - Destination MAC  
  - EtherType (IPv4, ARP, IPv6, Other)  
  - Frame size  
  - Running frame count  
- Logs **only ARP frames** into a JSON array
- JSON log is **always valid**, even if the program is killed
- Supports `jq` indexing:
  ```bash
  jq '.[0]' frame.txt

# EtherSniff  
### Minimal PF_PACKET Ethernet Sniffer with ARP JSON Logging  

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
  ```

---

## Steps

### **1. Compile the binary**
```bash
gcc raw.c -o raw
```

### **2. SSH into a LAN terminal**
```bash
ssh host@ip
```

### **3. Run message protocols from the SSH terminal**
```bash
arping ip   # ARP
ping ip     # ICMP
```

### **4. Run the C program**
```bash
sudo ./raw
```

### **5. Kill the running commands**
Press **CTRL+C**

### **6. Analyze the outputs with jq**
```bash
cat frame.txt | jq .
```

---

## Creators

**svrfcloud & Pi (CoPilot).**  

---
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <linux/if_packet.h>
#include <linux/if_ether.h>
#include <net/if.h>
#include <arpa/inet.h>

void print_mac(const unsigned char *mac)
{
    printf("%02x:%02x:%02x:%02x:%02x:%02x",
           mac[0], mac[1], mac[2],
           mac[3], mac[4], mac[5]);
}

void mac_to_string(const unsigned char *mac, char *out)
{
    sprintf(out, "%02x:%02x:%02x:%02x:%02x:%02x",
            mac[0], mac[1], mac[2],
            mac[3], mac[4], mac[5]);
}

int first_json = 1;

int main()
{
    int fd;
    struct sockaddr_ll sll;
    unsigned char buffer[2048];
    unsigned long frame_count = 0;

    fd = socket(PF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
    if (fd < 0)
    {
        perror("socket");
        return 1;
    }

    memset(&sll, 0, sizeof(sll));
    sll.sll_family = AF_PACKET;
    sll.sll_protocol = htons(ETH_P_ALL);
    sll.sll_ifindex = if_nametoindex("eth0");

    if (bind(fd, (struct sockaddr *)&sll, sizeof(sll)) < 0)
    {
        perror("bind");
        close(fd);
        return 1;
    }

    printf("Listening for packets…\n");

    // Start fresh JSON array every run
    FILE *fp_start = fopen("frame.txt", "w");
    if (fp_start)
    {
        fprintf(fp_start, "[\n");
        fclose(fp_start);
    }

    while (1)
    {
        ssize_t n = recvfrom(fd, buffer, sizeof(buffer), 0, NULL, NULL);
        if (n < 0)
        {
            perror("recvfrom");
            break;
        }

        frame_count++;

        const unsigned char *dst_mac = buffer;
        const unsigned char *src_mac = buffer + 6;
        unsigned short ethertype = (buffer[12] << 8) | buffer[13];

        printf("\n=== Frame #%lu ===\n", frame_count);
        printf("Captured %zd bytes\n", n);

        printf("Source MAC:      ");
        print_mac(src_mac);
        printf("\n");

        printf("Destination MAC: ");
        print_mac(dst_mac);
        printf("\n");

        printf("EtherType: 0x%04x → ", ethertype);

        switch (ethertype)
        {
        case 0x0800:
            printf("IPv4\n");
            break;
        case 0x0806:
            printf("ARP\n");
            break;
        case 0x86DD:
            printf("IPv6\n");
            break;
        default:
            printf("Other\n");
            break;
        }

        // Log ARP frames to JSON array
        if (ethertype == 0x0806)
        {
            FILE *fp = fopen("frame.txt", "r+");
            if (fp)
            {
                fseek(fp, 0, SEEK_END);
                long size = ftell(fp);

                // If file ends with ']', remove it
                if (size > 2)
                {
                    fseek(fp, -2, SEEK_END); // overwrite "\n]"
                }

                char src_str[32], dst_str[32];
                mac_to_string(src_mac, src_str);
                mac_to_string(dst_mac, dst_str);

                if (!first_json)
                {
                    fprintf(fp, ",\n");
                }
                first_json = 0;

                fprintf(fp,
                        "  { \"frame\": %lu, \"bytes\": %zd, \"src\": \"%s\", \"dst\": \"%s\", \"ethertype\": \"0x0806\" }\n]",
                        frame_count, n, src_str, dst_str);

                fclose(fp);
            }
        }
    }

    close(fd);

    // Close JSON array
    FILE *fp_end = fopen("frame.txt", "a");
    if (fp_end)
    {
        fprintf(fp_end, "\n]\n");
        fclose(fp_end);
    }

    return 0;
}

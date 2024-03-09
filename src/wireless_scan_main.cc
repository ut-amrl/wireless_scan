#include <iostream>
#include <iwlib.h>

int main() {
    wireless_scan_head head;
    wireless_scan *result;
    iwrange range;
    int sock;

    /* Open socket to kernel */
    sock = iw_sockets_open();

    /* Get some metadata to use for scanning */
    if (iw_get_range_info(sock, "wlan0", &range) < 0) {
        std::cout << "Error during iw_get_range_info. Aborting." << std::endl;
        exit(2);
    }

    /* Perform the scan */
    if (iw_scan(sock, "wlan0", range.we_version_compiled, &head) < 0) {
        std::cout << "Error during iw_scan. Aborting." << std::endl;
        exit(2);
    }

    /* Traverse the results */
    result = head.result;
    while (nullptr != result) {
        std::cout << result->b.essid << '\t'
                  << result->stats.qual.level << std::endl;
        result = result->next;
    }

    /* Close the socket. */
    iw_sockets_close(sock);

    return 0;
}
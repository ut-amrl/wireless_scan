#include <iwlib.h>
#include <stdio.h>

#include <iostream>

DEFINE_string(interface, "wlo1", "The interface to scan");

std::string GetBSSID(const wireless_scan* result) {
  char bssid[18];
  snprintf(bssid, sizeof(bssid), "%02x:%02x:%02x:%02x:%02x:%02x",
           (result->ap_addr.sa_data[0] & 0xFF),
           (result->ap_addr.sa_data[1] & 0xFF),
           (result->ap_addr.sa_data[2] & 0xFF),
           (result->ap_addr.sa_data[3] & 0xFF),
           (result->ap_addr.sa_data[4] & 0xFF),
           (result->ap_addr.sa_data[5] & 0xFF));
  return std::string(bssid);
}

int main(int argc, char *argv[]) {
  std::string interface("wlo1");
  if (argc > 1) {
    interface = std::string(argv[1]);
  }

  // Check if running as root.
  if (geteuid() != 0) {
    fprintf(stderr, "WARNING: This program must be run as root to perform an active scan. Returning only cached results.\n");
  }

  printf("Performing scan on interface: %s\n", interface.c_str());
  wireless_scan_head head;
  wireless_scan* result = nullptr;
  iwrange range;
  const int sock = iw_sockets_open();

  if (iw_get_range_info(sock, interface.c_str(), &range) < 0) {
    fprintf(stderr, "Error during iw_get_range_info. Aborting.\n");
    exit(1);
  }

  if (iw_scan(sock,
              const_cast<char *>(interface.c_str()), 
              range.we_version_compiled, 
              &head) < 0) {
    fprintf(stderr, "Error during iw_scan. Aborting.\n");
    exit(2);
  }

  result = head.result;
  while (result != nullptr) {
    printf("BSSID:%s Signal: %3d ESSID: \"%s\" \n",
            GetBSSID(result).c_str(),
            result->stats.qual.level,
            result->b.essid);
    result = result->next;
  }

  /* Close the socket. */
  iw_sockets_close(sock);

  return 0;
}
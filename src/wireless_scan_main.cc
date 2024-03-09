#include <iwlib.h>
#include <stdio.h>

#include <iostream>

#include "gflags/gflags.h"

DEFINE_string(interface, "wlan0", "The interface to scan");

int main(int argc, char *argv[]) {
  gflags::ParseCommandLineFlags(&argc, &argv, true);
  wireless_scan_head head;
  wireless_scan* result = nullptr;
  iwrange range;
  const int sock = iw_sockets_open();

  if (iw_get_range_info(sock, FLAGS_interface.c_str(), &range) < 0) {
    fprintf(stderr, "Error during iw_get_range_info. Aborting.\n");
    exit(1);
  }

  if (iw_scan(sock,
              const_cast<char *>(FLAGS_interface.c_str()), 
              range.we_version_compiled, 
              &head) < 0) {
    fprintf(stderr, "Error during iw_scan. Aborting.\n");
    exit(2);
  }

  result = head.result;
  while (result != nullptr) {
    printf("ESSID: %s Signal: %d\n", result->b.essid, result->stats.qual.level);
    result = result->next;
  }

  /* Close the socket. */
  iw_sockets_close(sock);

  return 0;
}
#!/usr/bin/env python
import os
import sys
from mt_ota_preprocess import generate_updatelist

def main(argv):
  if len(argv) != 2:
    sys.exit(1)

  scatter_file = argv[0]
  update_list = argv[1]

  # generate update list
  if os.getenv("AB_OTA_UPDATER") != "true":
    generate_updatelist(scatter_file, update_list)

if __name__ == '__main__':
  main(sys.argv[1:])


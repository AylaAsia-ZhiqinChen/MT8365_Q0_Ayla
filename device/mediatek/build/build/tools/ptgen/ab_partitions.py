#!/usr/bin/env python
import os
import sys
from mt_ota_preprocess import copy_ab_ota_filese

def main(argv):
  if len(argv) != 1:
    sys.exit(1)

  scatter_file = argv[0]

  if os.getenv('AB_OTA_UPDATER') == 'true':
    copy_ab_ota_filese(None, scatter_file, '', None)

if __name__ == '__main__':
  main(sys.argv[1:])


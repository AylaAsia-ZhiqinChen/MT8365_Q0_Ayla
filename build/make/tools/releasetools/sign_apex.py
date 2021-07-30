#!/usr/bin/env python
#
# Copyright (C) 2019 The Android Open Source Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

"""
Signs a standalone APEX file.

Usage:  sign_apex [flags] input_apex_file output_apex_file

  --container_key <key>
      Mandatory flag that specifies the container signing key.

  --payload_key <key>
      Mandatory flag that specifies the payload signing key.

  --payload_extra_args <args>
      Optional flag that specifies any extra args to be passed to payload signer
      (e.g. --payload_extra_args="--signing_helper_with_files /path/to/helper").
"""

import logging
import shutil
import sys

import apex_utils
import common

logger = logging.getLogger(__name__)


def main(argv):

  options = {}

  def option_handler(o, a):
    if o == '--container_key':
      # Strip the suffix if any, as common.SignFile expects no suffix.
      DEFAULT_CONTAINER_KEY_SUFFIX = '.x509.pem'
      if a.endswith(DEFAULT_CONTAINER_KEY_SUFFIX):
        a = a[:-len(DEFAULT_CONTAINER_KEY_SUFFIX)]
      options['container_key'] = a
    elif o == '--payload_key':
      options['payload_key'] = a
    elif o == '--payload_extra_args':
      options['payload_extra_args'] = a
    else:
      return False
    return True

  args = common.ParseOptions(
      argv, __doc__,
      extra_opts='',
      extra_long_opts=[
          'container_key=',
          'payload_extra_args=',
          'payload_key=',
      ],
      extra_option_handler=option_handler)

  if (len(args) != 2 or 'container_key' not in options or
      'payload_key' not in options):
    common.Usage(__doc__)
    sys.exit(1)

  common.InitLogging()

  input_zip = args[0]
  output_zip = args[1]
  with open(input_zip) as input_fp:
    apex_data = input_fp.read()

  signed_apex = apex_utils.SignApex(
      apex_data,
      payload_key=options['payload_key'],
      container_key=options['container_key'],
      container_pw=None,
      codename_to_api_level_map=None,
      signing_args=options.get('payload_extra_args'))

  shutil.copyfile(signed_apex, output_zip)
  logger.info("done.")


if __name__ == '__main__':
  try:
    main(sys.argv[1:])
  except common.ExternalError:
    logger.exception("\n   ERROR:\n")
    sys.exit(1)
  finally:
    common.Cleanup()

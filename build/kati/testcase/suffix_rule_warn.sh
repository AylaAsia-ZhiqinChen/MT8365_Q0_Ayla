#!/bin/bash
#
# Copyright 2018 Google Inc. All rights reserved
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http:#www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

set -u

mk="$@"

cat <<EOF > Makefile
test:
	@echo "PASS"
.c.o:
	cp $< $@
EOF

if echo "${mk}" | grep -qv "kati"; then
  # Make doesn't support these warnings, so write the expected output.
  echo 'Makefile:3: warning: suffix rules are deprecated: .c.o'
  echo 'PASS'
else
  ${mk} --no_builtin_rules --warn_suffix_rules 2>&1
fi

if echo "${mk}" | grep -qv "kati"; then
  # Make doesn't support these warnings, so write the expected output.
  echo 'Makefile:3: *** suffix rules are obsolete: .c.o'
else
  ${mk} --no_builtin_rules --werror_suffix_rules 2>&1
fi

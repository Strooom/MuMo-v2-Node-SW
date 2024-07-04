"""
#
# Warning: This is a workaround for builds where build-wrapper doesn't work
#
"""

import sys
import json
import shlex
from collections import OrderedDict
import os

if len(sys.argv) < 2:
  sys.stderr.write("Usage: %s <path to compile_commands.json>\n" % (sys.argv[0]))
  sys.exit(1)

with open(sys.argv[1]) as db_file:
  db = json.load(db_file)

env = []
for k, v in os.environ.items():
  env.append("%s=%s" % (k, v))

captures = []
for entry in db:
  if 'arguments' in entry:
    cmd = entry['arguments']
  else:
    cmd = shlex.split(entry['command'])

  capture = OrderedDict()
  capture['compiler'] = 'clang'
  capture['cwd'] = entry['directory']
  capture['executable'] = cmd[0]
  capture['cmd'] = cmd
  capture['env'] = env
  captures.append(capture)

bw = OrderedDict()
bw['version'] = 0
bw['captures'] = captures

os.mkdir('cfamily-compilation-database')
with open('cfamily-compilation-database/build-wrapper-dump.json', 'w') as bw_file:
  json.dump(bw, bw_file, indent = 2)
print('File created: cfamily-compilation-database/build-wrapper-dump.json')
print('Warning: This is a workaround for builds where build-wrapper doesn\'t work')

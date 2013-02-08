#!/usr/bin/env python

import os
import sys
import time
import math
from sys import argv, exit

try:
    from gi.repository import GExiv2
except ImportError:
    exit('You need to install gexiv2 first.')

if len(argv) < 3:
    exit('Usage: '+argv[0]+' <IMG_0001.CR2> [IMG_0002.CR2] ... <output_combined.jpegs>')

def eta_remains(seconds):
  secs = seconds % 60
  minutes = math.floor(seconds/60) % 60
  hours = math.floor(seconds/(60*60)) % (24)
  days = math.floor(seconds/(60.0*60*60*24))
  if days > 0:
    return '%(days)dd %(hours)dh %(minutes)dm %(secs)ds' % {"days": days, "hours": hours, "minutes": minutes, "secs":secs}
  if hours > 0:
    return '%(hours)dh %(minutes)dm %(secs)ds' % {"hours": hours, "minutes": minutes, "secs":secs}
  if minutes > 0:
    return '%(minutes)dm %(secs)ds' % { "minutes": minutes, "secs":secs}
  else:
    return '%(secs)ds' % {"secs":secs}

# Note, these are equivalent:
# metadata = GExiv2.Metadata()
# metadata.open_path(argv[-1])
script_name = argv.pop(0)
joined_preview_name = argv.pop(-1)

# truncate output file
f1 = open(joined_preview_name, 'wb')
f1.close()

start=time.time()
if joined_preview_name == '-':
  f = sys.stdout.buffer
else:
  f = open(joined_preview_name, 'a+b')
  
n=1
count = len(argv)
str_count = str(count)
#sys.stderr = os.fdopen(sys.stderr.fileno(), 'w', 0)
for file in argv:
  if file == '-':
    for line in sys.stdin:
      l = line.strip()
      sys.stderr.write('('+str(n)+') '+l+"\n")  
      sys.stderr.flush()
      metadata = GExiv2.Metadata(l)
      props = metadata.get_preview_properties()[2]
      image = metadata.get_preview_image(props)
      f.write(image.get_data())
      n+=1
  else:
    percent = 100*n/count
    duration = time.time()-start
    eta = (duration/percent)*(100-percent)
    sys.stderr.write(str("%3.2f"%round(percent, 2))+'% ('+str(n)+'/'+str_count+') '+eta_remains(eta)+' remains '+file+"\n")  
    metadata = GExiv2.Metadata(file)
    props = metadata.get_preview_properties()[2]
    image = metadata.get_preview_image(props)
    f.write(image.get_data())
    n+=1
    sys.stderr.flush()
f.flush()
f.close()



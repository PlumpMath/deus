import os
import sys

def read(file):
  data = ''
  first = False
  for line in file:
    if (line and line.strip()):
      if not first:
        data += line
        first = True
      else:
        data += '    ' + line
  return data.rstrip('\n')

def merge(css, src, name, size):
  ifile = open('res/html/index.html', "rt")
  ofile = open('bin/index.html', "wt")
  for line in ifile:
    line = line.replace('/* == css == */', read(open(css, "rt")))
    line = line.replace('/* == src == */', read(open(src, "rt")))
    line = line.replace('{name}', name)
    line = line.replace('{size}', size)
    ofile.write(line)

if __name__ == "__main__":
  css = sys.argv[1]
  src = sys.argv[2]
  name = os.path.basename(sys.argv[3])
  size = str(os.path.getsize(sys.argv[3]) + os.path.getsize(sys.argv[4]) + os.path.getsize(sys.argv[5]))
  merge(css, src, name, size)
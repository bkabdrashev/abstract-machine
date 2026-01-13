#!/usr/bin/env python3
import sys, shutil

template = sys.argv[1]
out      = sys.argv[2]
max_len  = int(sys.argv[3])
placeholder = sys.argv[4]
mainargs = sys.argv[5]

if len(mainargs) >= max_len:
    print(f"Error: mainargs should not be longer than {max_len} bytes")
    sys.exit(1)

shutil.copyfile(template, out)

with open(out, "r+b") as fp:
    data = fp.read()
    idx = data.find(placeholder.encode())
    if idx == -1:
        print("Error: placeholder not found!")
        sys.exit(1)

    fp.seek(idx)
    fp.write(mainargs.encode() + b"\0" * (max_len - len(mainargs)))

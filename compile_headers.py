import glob
import os
import re
import sys

github_location = "github.com/bkthomps/Containers"

if len(sys.argv) != 2:
    sys.exit("Format: python3 compile_headers.py <version_number>")
version = str(sys.argv[1])
if not re.match(r"v[0-9]+\.[0-9]+\.[0-9]+", version):
    sys.exit("Error: version format must be: vi.j.k")

license_list = []
license_file = open("LICENSE", "r")
i = 0
for line in license_file:
    if i >= 2:
        license_list.append(line)
    i += 1
license_file.close()

first_line = license_list[0].split(" ")
name = ""
i = 0
for part in first_line:
    if i >= 3:
        name += part
        name += " "
    i += 1

header = "/*\n"
for line in license_list:
    if line == "\n":
        header += " *" + line
    else:
        header += " * " + line
header += " */\n\n"
header += "/*\n"
header += " * The Containers library is hosted at: " + github_location + "\n"
header += " * The author is: " + name
header += "* This local version is: " + version + "\n"
header += " */\n"

version_file = open("src/include/VERSION", "w+")
version_file.write(header)
version_file.close()

folder_path = 'src/include'
for filename in sorted(glob.glob(os.path.join(folder_path, '*.h'))):
    with open(filename, 'r') as file:
        text = file.read()
        entire_file = text.split("*/", 1)[1]
        split_around_text = '#include "_bk_defines.h"\n\n'
        split_around_include = entire_file.split(split_around_text, 1)
        header += split_around_include[0]
        if len(split_around_include) == 2:
            header += split_around_include[1]

containers_header_file = open("containers.h", "w+")
containers_header_file.write(header)
containers_header_file.close()

import sys
from pathlib import Path

if len(sys.argv) < 2:
    print("missing folder")
    exit(0)

work_folder = Path(sys.argv[1]).resolve()
if not work_folder.is_dir():
    print("%s is not a directory" % sys.argv[1])
    exit(0)

root = (work_folder / 'root.proj')
if not root.is_file():
    print("%s/root.proj is not a file" % sys.argv[1])
    exit(0)

def parse_main_line(line, projfile):
    line.strip()
    if line.startswith("CONFIG_FILE"):
        print("config line (SKIP)")
    elif line.startswith("PLATFORM_FILES"):
        print("platform files (IGNORED)")
    # elif line.empty():
    #     extraline = projfile.readline()
    #     print("extra line of %s is %s" % (line, extraline))

def main(root_proj):
    print(root_proj)
    inputfile = open(root_proj)
    for line in inputfile:
        parse_main_line(line, inputfile)
    inputfile.close()

if __name__ == '__main__':
    sys.exit(main(root))

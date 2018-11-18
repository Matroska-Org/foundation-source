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

def main(root_proj):
    print(root_proj)

if __name__ == '__main__':
    sys.exit(main(root))

#!/usr/bin/env python3

# Copyright (c) 2006-2010, CoreCodec, Inc.
# Copyright (c) 2024, Matroska (non-profit organisation)
# SPDX-License-Identifier: BSD-3-Clause

import argparse
import os
import filecmp
import shutil

redirect_src=[]
redirect_dst=[]
header=[]

def create_missing_dirs(path: str):
    new_path = os.path.dirname(os.path.abspath(path))
    if os.path.exists(new_path):
        return
    os.makedirs(new_path)

def compare(srcname: str, dstname: str) -> bool:
    if not os.path.exists(dstname):
        return True
    return not filecmp.cmp(srcname, dstname, False)

def copy(srcname: str, dstname: str):
    print('{} -> {}'.format(srcname, dstname))

    s = os.stat(srcname)
    create_missing_dirs(dstname)

    shutil.copy2(srcname, dstname)

def same_ch(a :str, b: str) -> bool:
    # TODO: check depending if file system is case sensitive
    return a[0:1] == b[0:1]

def match(name: str, mask: str) -> bool:
    if mask.startswith('*'):
        mask = mask[1:]
        while not match(name, mask):
            if len(name) == 0:
                return False
            name = name[1:]
        return True
    else:
        if len(mask) == 0:
            return len(name) == 0

        if len(name) == 0:
            return False

        return (mask == '?' or same_ch(name, mask)) and match(name[1:],mask[1:])

def refresh(src: str, dst: str, force: bool):
    if dst == '-' or '*' in src or '?' in src:
        return

    if not force:
        for i in range(0,len(redirect_src)):
            if match(src,redirect_src[i]):
                return

    if os.path.isdir(src):
        if not os.path.exists(dst):
            os.makedirs(dst)
        if not force or not dst.endswith('.'):
            dir = os.listdir(src)
            for f in dir:
                src2 = os.path.join(src, f)
                dst2 = os.path.join(dst, f)
                refresh(src2, dst2, False)
    else:
        if compare(src, dst):
            copy(src, dst)

def main():
    parser = argparse.ArgumentParser(prog='branch', description="Generate a source/binary package directory",
                                    epilog= "config file format:\"<src_path>\" \"<dst_path>\"")
    parser.add_argument('config_file', type=argparse.FileType('r', encoding='UTF-8'), help="file with list of files to test")
    args = parser.parse_args()

    base = os.path.dirname(os.path.abspath(args.config_file.name))

    while True:
        for line in args.config_file:
            if line.startswith('#'):
                continue
            line = line.strip()
            if line == '':
                continue

            parts = line.split(' ')
            if '*' in parts[0] or '?' in parts[0] or os.path.isabs(parts[0]):
                redirect_src.append(parts[0])
            else:
                redirect_src.append(os.path.join(base, parts[0]))
            redirect_dst.append(parts[1])

        break

    for i in range(0,len(redirect_src)):
        refresh(redirect_src[i], redirect_dst[i], True)

    return 0

if __name__ == "__main__":
    exit(main())

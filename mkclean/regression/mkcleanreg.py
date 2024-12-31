#!/usr/bin/env python3

# SPDX-License-Identifier: ISC
# Copyright (c) 2022, Matroska (non-profit organisation)

import argparse
import pathlib
import os
import sys
import subprocess
import hashlib

def testFile(cli, line_num, src_file, mkclean_options, filesize, hash):
    if not os.path.isfile(src_file):
        print(f"Fail:2:{line_num}: {src_file} doesn't exist", file=sys.stderr)
        return
    mkclean_exe = cli.mkclean
    if not mkclean_exe:
        if sys.platform.startswith('win32'):
            mkclean_exe = "mkclean.exe"
        else:
            mkclean_exe = "mkclean"
    outputfile = '_'
    if '--remux' in mkclean_options:
        outputfile = outputfile + 'm'
    if '--optimize' in mkclean_options:
        outputfile = outputfile + 'o'
    if '--no-optimize' in mkclean_options:
        outputfile = outputfile + 'n'
    if '--unsafe' in mkclean_options:
        outputfile = outputfile + 'u'
    if '--live' in mkclean_options:
        outputfile = outputfile + 'l'
    outputfile = outputfile + src_file + '.mkv'
    mkclean_run = [mkclean_exe, "--regression"]
    if cli.quiet:
        mkclean_run.append("--quiet")
    if mkclean_options:
        for option in mkclean_options.strip().split():
            mkclean_run.append(option)
    mkclean_run.append(src_file)
    mkclean_run.append(outputfile)
    result = subprocess.run(mkclean_run)

    if not result.returncode == 0:
        print(f"Fail:3:{line_num}: mkclean returned {result.returncode} for {mkclean_options}", file=sys.stderr)
        return

    if not os.path.isfile(outputfile):
        print(f"Fail:4:{line_num}: failed to stat file {outputfile}", file=sys.stderr)
        return

    outsize = os.path.getsize(outputfile)
    if not cli.generate and not outsize == filesize:
        print(f"Fail:5:{line_num}: wanted {filesize} got {outsize} size in {src_file}", file=sys.stderr)
        return

    if not cli.generate:
        # test with mkvalidator
        mkvalidator_exe = cli.mkvalidator
        if not mkvalidator_exe:
            if sys.platform.startswith('win32'):
                mkvalidator_exe = "mkvalidator.exe"
            else:
                mkvalidator_exe = "mkvalidator"
        mkvalidator_run = [mkvalidator_exe]
        if cli.quiet:
            mkvalidator_run.append("--quiet")
        if '--live' in mkclean_options:
            mkvalidator_run.append("--live")
        if '--remux' in mkclean_options:
            mkvalidator_run.append("--no-warn")
        mkvalidator_run.append(outputfile)
        result = subprocess.run(mkvalidator_run)

        if not result.returncode == 0:
            print(f"Fail:6:{line_num}: mkvalidator returned {result} for {outputfile}", file=sys.stderr)
            return

    filemd5 = hashlib.md5(open(outputfile,'rb').read()).hexdigest()
    if cli.generate:
        print(f"\"{src_file}\" \"{mkclean_options}\" {outsize} {filemd5}", file=sys.stderr)
    elif not filemd5 == hash:
        print(f"Fail:8:{line_num}: bad MD5 {filemd5} for {outputfile}", file=sys.stderr)
        return

    if not cli.keep:
        os.remove(outputfile)
    if not cli.generate:
        print(f"Success:0:{line_num}: {src_file} {mkclean_options}", file=sys.stderr)

def main():
    parser = argparse.ArgumentParser(prog='mkcleanreg', description="Handle mkclean regression tests",
                                    epilog= "regression file format:\"<file_path>\" \"<mkclean_options>\" <expected_size> <expected_md5>")
    parser.add_argument('regression_file', type=argparse.FileType('r', encoding='UTF-8'), help="file with list of files to test")
    parser.add_argument('--mkclean',     type=pathlib.Path, help="path to mkclean to test")
    parser.add_argument('--mkvalidator', type=pathlib.Path, help="path to mkvalidator to use")
    parser.add_argument('--generate', action='store_true', dest='generate', help="output is usable as a regression list file")
    parser.add_argument('--quiet',    action='store_true', dest='quiet',    help="don't display messages from mkvalidator")
    parser.add_argument('--keep',     action='store_true', dest='keep',     help="keep the output files")
    parser.add_argument('--version',  action='store_true', dest='version',  help="show the version of mkvalidator")
    args = parser.parse_args()

    i = 0
    for line in args.regression_file:
        if not line.startswith('#') and not line.strip() == '':
            regression = line.split('\"')
            if len(regression) == 1:
                print(regression[0])
                continue
            src_file = regression[1]
            mkclean_options = regression[3].strip()
            values = regression[4].strip().split()
            filesize = int(values[0])
            hash = values[1]
            testFile(args, i, src_file, mkclean_options, filesize, hash)
            i = i+1
    return 0

if __name__ == "__main__":
    exit(main())

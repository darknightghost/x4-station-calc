#! /usr/bin/env python3
# -*- coding: utf-8 -*-

import argparse
import os


def main():
    #Parse argument
    parser = argparse.ArgumentParser(description="Genetate resource list.")
    parser.add_argument("-r",
                        "--root",
                        type=str,
                        required=True,
                        help="Root directory.")
    parser.add_argument("-o",
                        "--output",
                        type=str,
                        required=True,
                        help="Output file.")
    parser.add_argument("inputs", type=str, nargs='+', help="Input files.")

    args = parser.parse_args()
    args.root = os.path.abspath(args.root)

    # Write .qrc file
    with open(args.output, "wb") as f:
        f.write("<RCC>\n".encode(encoding="utf-8"))
        f.write("    <qresource prefix=\"/\">\n".encode(encoding="utf-8"))

        for path in args.inputs:
            path = os.path.abspath(path)
            relpath = os.path.relpath(path, args.root)
            f.write(("        <file >%s</file>\n" %
                     (relpath)).encode(encoding="utf-8"))

        f.write(("        <file alias=\"%s\">%s</file>\n" %
                 ("Text/changelog", "../CHANGELOG")).encode(encoding="utf-8"))

        f.write("    </qresource>\n".encode(encoding="utf-8"))
        f.write("</RCC>".encode(encoding="utf-8"))

    return 0


if __name__ == "__main__":
    exit(main())

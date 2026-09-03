#!/usr/bin/env python3
# Copyright (c) 2026 AGH University of Krakow
# Developed by AGH Logic Unit
# SPDX-License-Identifier: Apache-2.0
"""
Simple filelist generator for Verible Language Server and Simulators.

This script scans directories for Verilog/SystemVerilog sources, identifies include
directories, and formats them into a standard filelist (.f / verible.filelist).
"""

import os
import argparse

def main():
    parser = argparse.ArgumentParser(description="Generate a filelist for Verible Language Server and Simulators.")
    parser.add_argument('-o', '--output', default='verible.filelist',
                        help="Output file path (default: verible.filelist)")
    parser.add_argument('-d', '--dirs', nargs='+', default=['rtl', 'verif'],
                        help="Directories to scan recursively (default: rtl verif)")
    parser.add_argument('--defines', nargs='+', default=[],
                        help="Preprocessor defines to include (+define+)")
    args = parser.parse_args()

    # Find project root (assumed to be one level above scripts/ directory)
    root = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))

    sources = []
    incdirs = set()

    # Walk specified directories to gather sources and includes
    for sdir in args.dirs:
        abs_sdir = os.path.join(root, sdir)
        if not os.path.isdir(abs_sdir):
            continue
        for r, _, files in os.walk(abs_sdir):
            # Prune hidden directories
            if any(part.startswith('.') for part in os.path.normpath(r).split(os.sep)):
                continue
            for f in files:
                path = os.path.relpath(os.path.join(r, f), root)
                if f.endswith(('.v', '.sv')):
                    sources.append(path)
                elif f.endswith(('.vh', '.svh')):
                    incdirs.add(os.path.relpath(r, root))

    # Simple name-based package sorting: compile packages before other modules
    pkgs = sorted([s for s in sources if 'pkg' in os.path.basename(s).lower()])
    others = sorted([s for s in sources if 'pkg' not in os.path.basename(s).lower()])
    sorted_sources = pkgs + others

    # Write the filelist
    output_path = os.path.join(root, args.output)
    with open(output_path, 'w', encoding='utf-8') as out:
        out.write("# Generated filelist for Verible LS and Simulators\n\n")

        if args.defines:
            out.write("# Preprocessor Defines\n")
            for df in sorted(args.defines):
                out.write(f"+define+{df}\n")
            out.write("\n")

        if incdirs:
            out.write("# Include Directories\n")
            for inc in sorted(list(incdirs)):
                out.write(f"+incdir+{inc}\n")
            out.write("\n")

        if sorted_sources:
            out.write("# Source Files\n")
            for src in sorted_sources:
                out.write(f"{src}\n")

    print(f"Generated {args.output} (sources: {len(sorted_sources)}, includes: {len(incdirs)})")

if __name__ == '__main__':
    main()

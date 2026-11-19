#!/usr/bin/env python3
# Copyright (c) 2026 AGH University of Krakow
# Developed by AGH Logic Unit
# SPDX-License-Identifier: Apache-2.0

import sys
import os

def check_file(filepath):
    # Pomiń foldery i nieistniejące pliki
    if not os.path.isfile(filepath):
        return True

    # Całkowicie omiń pliki vendora (zezwalamy na brak naszej licencji w kodzie zewnętrznym)
    normalized_path = os.path.normpath(filepath)
    path_parts = normalized_path.split(os.sep)
    if 'vendor' in path_parts:
        return True

    # Sprawdź sygnaturę licencyjną w pierwszych 1000 znakach pliku
    try:
        with open(filepath, 'r', encoding='utf-8', errors='ignore') as f:
            header = f.read(1000)
            if 'SPDX-License-Identifier: Apache-2.0' in header:
                return True
    except Exception as e:
        print(f"Error reading {filepath}: {e}")
        return False

    return False

def main():
    files_to_check = sys.argv[1:]

    if not files_to_check:
        sys.exit(0)

    non_compliant = []
    for filepath in files_to_check:
        if not check_file(filepath):
            non_compliant.append(filepath)

    if non_compliant:
        print("Error: The following files are missing the SPDX-License-Identifier header:")
        for nc in non_compliant:
            print(f"  - {nc}")
        print("\nPlease add the following header to the top of these files:")
        print("For SystemVerilog files (*.sv, *.v, *.svh, *.vh):")
        print("  // Copyright (c) 2026 AGH University of Krakow")
        print("  // Developed by AGH Logic Unit")
        print("  // SPDX-License-Identifier: Apache-2.0")
        print("\nFor Python files (*.py):")
        print("  # Copyright (c) 2026 AGH University of Krakow")
        print("  # Developed by AGH Logic Unit")
        print("  # SPDX-License-Identifier: Apache-2.0")
        sys.exit(1)

    print("All checked files have correct SPDX-License-Identifier headers.")
    sys.exit(0)

if __name__ == '__main__':
    main()

import re
import sys
from typing import Dict

class Pattern:
    def __init__(self, height: int, width: int, born: str, survive: str, cells: list[list[bool]]):
        self.height = height
        self.width = width
        self.born = born
        self.survive = survive
        self.cells = cells

    def write_header_file(self, nm: str):
        fnm = nm.lower() + ".h"
        prefix = nm.upper() + "_"
        with open(fnm, mode='w') as f:
            print(f"/* File: {fnm} */", file=f)
            print("", file=f)
            for row in self.cells:
                characters = ["#" if v else " " for v in row]
                print(f"//  {''.join(characters)}", file=f)
            print("", file=f)
            print('#include "stdint.h"', file=f)
            print("", file=f)
            print(f"#define {prefix}HEIGHT ({self.height})", file=f)
            print(f"#define {prefix}WIDTH ({self.width})", file=f)
            print(file=f)
            print(f"uint8_t {nm}[{prefix}HEIGHT][{prefix}WIDTH] = {{", file=f)
            for row in self.cells:
                words = ["1" if v else "0" for v in row]
                print(f"    {{{', '.join(words)}}},", file=f)
            print("};", file=f)


def read_file(filename):
    with open(filename) as file:
        lines = file.readlines()
    return [line.strip() for line in lines if not line.startswith('#')]


digits = ['0', '1', '2', '3', '4', '5', '6', '7', '8', '9']

def parse_rle_header(line: str) -> Dict[str, str]:
    res: Dict[str, str] = {}
    values = line.split(",")
    for value in values:
        pieces = value.split('=')
        res[pieces[0].strip()] = pieces[1].strip()
    return res

def parse_rle(filename) -> Pattern:
    lines = read_file(filename)
    header = lines[0]
    values: Dict[str, str] = parse_rle_header(header)
    width = int(values['x'])
    height = int(values['y'])
    if 'rule' in values:
        pieces = values['rule'].split('/')
        born = pieces[0].strip()
        survive = pieces[1].strip()
    else:
        born = "B3"
        survive = "S23"

    lines = lines[1:]
    lines = ''.join(lines).strip('\n')

    repeat = 0
    saw_repeat = False
    rows: list[list[bool]] = []
    row: list[bool] = []
    for c in lines:
        if c in digits:
            repeat = 10*repeat + digits.index(c)
            saw_repeat = True
        else:
            if c == 'b':
                if not saw_repeat:
                    row.append(False)
                else:
                    row.extend([False]*repeat)
            elif c == 'o':
                if not saw_repeat:
                    row.append(True)
                else:
                    row.extend([True]*repeat)
            elif c == '$' or c == '!':
                while len(row) < width:
                    row.append(False)
                rows.append(row)
                if saw_repeat:
                    while repeat>1:
                        rows.append([False] * width)
                        repeat -= 1
                row = []
            else:
                print("Unexpected character '{c}' in RLE string")
            saw_repeat = False
            repeat = 0
    height = len(rows)
    return Pattern(height, width, born, survive, rows)


def convert_pattern(rle_file: str, prefix: str):
    pattern: Pattern = parse_rle(rle_file)
    pattern.write_header_file(prefix)


def run_main():
    args = sys.argv
    if len(args) < 2:
        print(f"Usage: {args[0]} <pattern-file>.rle")
        sys.exit(1)
    fnm = args[1]
    prefix = fnm[:-4]
    convert_pattern(fnm, prefix)

if __name__ == '__main__':
    run_main()
# -*- coding: utf-8 -*-
import os
import re
import sys
import time


def get_last_modified_year(filepath):
    """获取文件的最后修改年份"""
    modified_time = os.path.getmtime(filepath)
    return time.localtime(modified_time).tm_year


def update_copyright_year(filepath, modified_year):
    """更新版权声明中的年份"""
    with open(filepath, 'r') as f:
        lines = f.readlines()

    updated_lines = []
    copyright_found = False
    pattern = r"\* Copyright \(c\) (\d{4}) - (\d{4}),  Sifli Technology"

    for line in lines[:30]:
        match = re.search(pattern, line)
        if match:
            create_year = match.group(1)
            updated_line = line.replace('{} - {}'.format(create_year, match.group(2)),
                                        '{} - {}'.format(create_year, modified_year))
            updated_lines.append(updated_line)
            copyright_found = True
        else:
            updated_lines.append(line)
    updated_lines.extend(lines[30:])
    if copyright_found:
        with open(filepath, 'wb') as f:
            f.writelines(updated_lines)
        print("Updated copyright in:", filepath)


def process_directory(directory):
    """递归遍历目录，处理所有 .c, .cpp, .h 文件"""
    for root, _, files in os.walk(directory):
        for filename in files:
            if filename.endswith(('.c', '.cpp', '.h')):
                filepath = os.path.join(root, filename)
                modified_year = get_last_modified_year(filepath)
                update_copyright_year(filepath, modified_year)


if __name__ == '__main__':
    if len(sys.argv) != 2:
        print("Usage: python update_copyright.py <Destination Path>")
        sys.exit(1)
    directory_path = sys.argv[1]
    process_directory(directory_path)

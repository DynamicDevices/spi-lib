#!/usr/bin/env python
# -*- coding: utf-8 -*-
"""
Script purpose is to parse files and verify it's grammar.
Additional libs:
pyenchant: https://pypi.python.org/pypi/pyenchant/

Author: Serhii Kharenko (khso@cypress.com)
Version: 1.0
"""

import os
import sys
import fnmatch
import enchant
import argparse
import re


def read_file(file_name):
    """
    Read defined file into list of file lines

    :param file_name: path to file
    :return: read_data - list of file lines
    """
    with open(file_name, "r", encoding="utf8") as f:
        read_data = f.read().splitlines()
    f.close()
    return read_data


def get_word_parts(word, known_words):
    """
    Recursive function for word split into parts.
    E.g.
    my_func_testMyData => ["my", "func", "test", "My", "Data"]

    :param word: input word (like in e.g.)
    :param known_words: list of words which are in exception list
    :return: word_parts - list with parts of word
    """
    word_parts = []
    if "" == word:
        # Empty string. Don't do anything
        pass
    elif word in known_words:
        # Known word. Don't do anything
        pass
    elif re.match(r'0[x|X][0-9a-fA-F]{1,16}[uUlL]{0,2}|[0-9]{1,16}[uUlL]{0,2}', word):
        # Number. Don't do anything
        pass
    elif re.match(r'\w+[A-Z]\w+', word):
        split_word = re.findall("(?<=[a-z])(?=[A-Z])|(?<=[A-Z])(?=[A-Z][a-z])", word)
        for word_part in split_word:
            word_parts.extend(get_word_parts(word_part, known_words))
    elif "_" in word:
        # Define. Need to parse parts of it split by "_".
        split_word = word.split("_")
        for word_part in split_word:
            word_parts.extend(get_word_parts(word_part, known_words))
    else:
        word_parts.append(word)
    return word_parts


def check_grammar(path_list, file_list_patterns, user_dict):

    """
    Parsing files and verify grammar. Grammar verification is based on internal "enchant"
    lib and user-defined dictionaries.

    :param path: root path of file(s) location
    :param file_list_patterns: file extension mask list e.g. ['*.c', '*.h']
    :param exception_list_path: list with path of file(s) with exception words
    :return:  err_message - contains error message
              issues_dict -  dictionary of tuples with issues description in format
                                    [file_name, (word, incorrect_parts, , line_num)]
    """
    err_message = None
    issues_dict = {}

    known_words = []
    exception_list_path = []
    if os.path.isdir(user_dict):
        for file_name in os.listdir(user_dict):
            file_path = os.path.join(user_dict, file_name)
            if os.path.isfile(file_path):
                known_words.extend(read_file(file_path))
            else:
                err_message = "\"" + file_path + "\" doesn't match any file"
                return err_message, issues_dict

    dict = enchant.Dict("en_US")

    for path in path_list:
        file_list = []
        if os.path.isdir(path):
            if path == ".":
                # special case: root directory scan is always non-recursive
                file_names = os.listdir(path)
                for ext in file_list_patterns:
                    for file_name in fnmatch.filter(file_names, ext):
                        if os.path.isfile(file_name):
                            file_list.append(file_name)
            else:
                # recursively scan all sub-directories, excluding GeneratedSource
                for root, dir_names, file_names in os.walk(path):
                    if not "GeneratedSource" in root:
                        for ext in file_list_patterns:
                            for file_name in fnmatch.filter(file_names, ext):
                                file_list.append(os.path.join(root, file_name))
        elif os.path.isfile(path):
            file_list.append(path)
        else:
            err_message = "\"" + path + "\" doesn't match folder or file"
            return err_message, issues_dict

        #print(file_list)
        if 0 == len(file_list):
            err_message = "\"" + path + "\" doesn't contain any required file"
            return err_message, issues_dict

        files = {}
        for file_name in file_list:
            files[file_name] = read_file(file_name)

        val = 1
        for file_name, content in sorted(files.items()):
            if sys.stdout.isatty():
                print(str(val) + "/" + str(len(list(files.items()))), end="\r")
            val += 1
            line_num = 0
            for line in content:
                line_num += 1
                for word in re.split(r"[\W]+", line):
                    incorrect_parts = []
                    word_parts = get_word_parts(word, known_words)
                    for part in word_parts:
                        if len(part) <= 4:
                            # Ignore word parts <=4 chars
                             continue
                        # dict does not like whole capital words
                        if not dict.check(part[:1]+part[1:].lower()):
                            incorrect_parts.append(part)

                    if len(incorrect_parts):
                        file_name = file_name.replace("\\", "/")
                        if not file_name in issues_dict:
                            issues_dict[file_name] = []
                        issues_dict[file_name].append((word, incorrect_parts, str(line_num)))
    return err_message, issues_dict


def print_issues(issues_dict, log_path, txt_path):
    """
    Print info about issues in words

    :param issues_dict: dictionary of tuples with issues description in format
                                    (word, incorrect_parts, file_name, line_num)
    :rtype: object
    """
    file_log = open(log_path, 'w', encoding="utf-8")
    file_txt = open(txt_path, 'w', encoding="utf-8")
    unic = set()
    if len(issues_dict) == 0:
        print("No spelling issues found")
    else:
        for file_name in issues_dict:
            for word, incorrect_parts, line_num in issues_dict[file_name]:
                issue_line = "%-50s" % (file_name + ":" + line_num) + "\t" + word + " " + str(incorrect_parts)
                for path in incorrect_parts:
                    unic.add(path)
                file_log.write(issue_line + "\n")

        file_txt.write(str.join('\n', unic) + "\n")
    file_log.close()
    file_txt.close()


# This is the standard boilerplate that calls the main() function.
if __name__ == '__main__':

    parser = argparse.ArgumentParser()
    parser.add_argument('-p', '--path', nargs="*", action='store', dest='path', required=True,
                        help="Path to source code dir. All sub folders will be parsed.")
    parser.add_argument('-m', '--mask', nargs="*", action='store', dest='mask', required=True,
                        help="File masks in format *.<extension>")
    parser.add_argument('-d', '--dict', action='store', dest='dict', required=True,
                        help="Path to the user defined dictionary.")
    parser.add_argument('-l', '--log', action='store', dest='log', required=True,
                        help="Path to the full log file.")
    parser.add_argument('-t', '--txt', action='store', dest='txt', required=True,
                        help="Path to the short txt report file.")
    args = parser.parse_args()

    err, issues = check_grammar(args.path, args.mask, args.dict)
    if err is None:
        print_issues(issues, args.log, args.txt)
    else:
        print(err)

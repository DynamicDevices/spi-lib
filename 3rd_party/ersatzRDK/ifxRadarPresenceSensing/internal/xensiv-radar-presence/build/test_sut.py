#!/usr/bin/env python
"""
Parse INI file and run SUT build

Copyright 2019, Cypress Semiconductor Corporation.  All rights reserved.
You may use this file only in accordance with the license, terms, conditions,
disclaimers, and limitations in the end user license agreement accompanying
the software package with which this file was provided.
"""
import os
import argparse
import sys
import configparser
import subprocess
import re
import json
import shutil
from test_sut_config import *

# Switch to the repository root directory
os.chdir(os.path.join(os.path.dirname(__file__), ".."))

class bcolors:
    OKGREEN = '\033[1;92m'
    SKIP = '\033[1;33m'
    FAIL = '\033[91m'
    ENDC = '\033[0m'

class MakeError(Exception):
    def __init__(self, message, errorcode):
        super(MakeError, self).__init__(message)
        self.errorcode = errorcode

def send_command(cmdslist):
    '''
        Internal helper method for sending a command.
        Appends the path of the exe and project parameters as needed to the command list.
        Handles any errors raised when calling the subprocess command.
    '''
    output = None
    try:
        # Make sure to use a list of commands so that spaces within the paths are not treated as command separators
        output = subprocess.check_output(cmdslist, stderr=subprocess.STDOUT)
    except subprocess.CalledProcessError as E:
        output = E.output
        return output.decode("utf-8"), E.returncode
    return output.decode("utf-8"), 0


class Logger:
    ansi_escape = re.compile(r"\x1b\[[0-?]*[ -/]*[@-~]")

    @staticmethod
    def log_to_file(log_str, sut_log_file):
        f = open(sut_log_file, "a+", encoding="utf-8")
        f.write(log_str.replace('\r', ''))
        f.close()

    @staticmethod
    def log_write_header(sut_name, target, toolchain, core, vfp, config):
        logHeader = "++++++++++++++++++++++++++++++++++++++++++++++++++\n"
        logHeader += "#-TEST-#: %s\n" % sut_name
        logHeader += "#-TARGET-#: %s\n" % target
        logHeader += "#-TOOLCHAIN-#: %s\n" % toolchain
        logHeader += "#-CORE-#: %s\n" % core
        logHeader += "#-VFP_SELECT-#: %s\n" % vfp
        logHeader += "#-CONFIG-#: %s\n" % config
        logHeader += "++++++++++++++++++++++++++++++++++++++++++++++++++\n"
        return logHeader

    @staticmethod
    def log_write_footer(result):
        logFooter = "++++++++++++++++++++++++++++++++++++++++++++++++++\n"
        logFooter += "#-RESULT: %s-#\n" % result
        logFooter += "++++++++++++++++++++++++++++++++++++++++++++++++++\n"
        return logFooter

class Make_Helper:
    '''
        Run make commands
    '''
    def __init__(self, sut_log_file, use_toolchains, make_args):
        self.sut_log_file = sut_log_file
        self.make_summary = []
        self.use_toolchains = use_toolchains
        self.make_args = make_args
        if sys.platform == "darwin": # macOS
            self.skip_toolchains = ["IAR", "ARM"]
        else:  # Windows/Linux
            self.skip_toolchains = ["A_Clang"]

    def toolchain_skipped(self, toolchain, vfp):
        if not toolchain:
            return False
        if toolchain in self.skip_toolchains:
            return True
        if self.use_toolchains and not toolchain in self.use_toolchains:
            return True
        return False

    def add_to_summary(self, msg):
        # Add single line to the summary dump
        self.make_summary.append(msg)

    def compile_sut(self, sut_path, target, toolchain, core, vfp, config, verbose):
        '''
            Compile SUT with defined configuration
        '''
        make_cwd = f"{ASSET_TEST_DIR}/{sut_path}"
        make_list = ["make", "-C", make_cwd]
        if target:
            make_list.append("TARGET=%s" % target)
        if core:
            make_list.append("CORE=%s" % core)
        if vfp:
            make_list.append("VFP_SELECT=%s" % vfp)
        if config:
            make_list.append("CONFIG=%s" % config)
        if toolchain:
            make_list.append("TOOLCHAIN=%s" % toolchain)
        if verbose:
            make_list.append("VERBOSE=%s" % 1)
        make_list.append("build")
        # Append all optional make arguments (passed to test_sut.py)
        for make_arg in self.make_args:
            make_list.append(make_arg)

        make_cmd = ' '.join(make_list)
        print(make_cmd)
        if self.toolchain_skipped(toolchain, vfp):
            log_ansi = 'Toolchain: %s skipped for %s\n' % (toolchain, sys.platform)
            print(log_ansi)
            Logger.log_to_file(log_ansi, self.sut_log_file)
            result = bcolors.SKIP + "SKIP" + bcolors.ENDC
            self.add_to_summary(result + " " + make_cmd)
        else:
            header = Logger.log_write_header(sut_path, target, toolchain, core, vfp, config)
            print(header)

            output, return_code = send_command(make_list)

            result = bcolors.OKGREEN + "PASS" + bcolors.ENDC if return_code == 0 else bcolors.FAIL + "FAIL" + bcolors.ENDC
            self.add_to_summary(result + " " + make_cmd)
            footer = Logger.log_write_footer(result)
            log_escape = Logger.ansi_escape.sub('', header + output + footer)
            log_ansi = '%s\nLog:\n%s\n' % (make_cmd, log_escape)
            Logger.log_to_file(log_ansi, self.sut_log_file)
            if (return_code != 0):
                raise MakeError(output, return_code)
            else:
                print (output)

            print(footer)


class Test_Info:
    def __init__(self, sut, test_path, target, toolchain, core, vfp, config):
        self.sut = sut
        self.path = test_path
        self.target = target
        self.toolchain = toolchain
        self.core = core
        self.vfp = vfp
        self.config = config

def list_from_ini(cfg, section, key):
    if cfg.has_option(section, key):
        value = cfg.get(section, key)
        if value != "":
            return value.split(" ")
    return [None]

def parse_sut_ini(sut_ini_file):
    '''
        SUT ini file parsing
    '''
    print("Suts ini: ", sut_ini_file)
    sut_list = []
    error = False

    if not os.path.isfile(sut_ini_file):
        raise MakeError(
            "Error: SUT ini file not found. Please make sure the path is relative to root of the repository", -2)

    try:
        cfg = configparser.ConfigParser()
        cfg.read(sut_ini_file)

        for sut in cfg.sections():
            if cfg.has_option(sut, "TEST") and cfg.get(sut, "TEST") != "":
                sut_path = cfg.get(sut, "TEST")
            else:
                print(
                    bcolors.FAIL + "Error: The definition for SUT: %s does not specify sut path" % sut + bcolors.ENDC)
                error = True
                continue

            target = list_from_ini(cfg, sut, "TARGET")
            toolchain = list_from_ini(cfg, sut, "TOOLCHAIN")
            core = list_from_ini(cfg, sut, "CORE")
            vfp = list_from_ini(cfg, sut, "VFP_SELECT")
            config = list_from_ini(cfg, sut, "CONFIG")

            if not error:
                test_info = Test_Info(sut, sut_path, target, toolchain, core, vfp, config)
                sut_list.append(test_info)

    except Exception as ex:
        raise MakeError(ex, -2)

    return sut_list


def main():
    err = False
    argParser = argparse.ArgumentParser()
    argParser.add_argument("sut_ini", help="SUT ini file name")
    argParser.add_argument("-t", "--toolchain", action='append')
    argParser.add_argument("-l", "--log")
    argParser.add_argument("-v", "--verbose", help="Increase make verbosity", action="store_true")

    try:
        # Support passing optional arguments ("-j") directly to make
        args, make_args = argParser.parse_known_args()
        sut_ini = args.sut_ini
        use_toolchains = args.toolchain
        sut_log_file = args.log
        verbose = args.verbose
    except Exception as ex:
        print("FAIL: %s" % ex)
        err = True
        sys.exit(-1)

    sut_ini_path = os.path.join(ASSET_TEST_DIR, sut_ini)

    # Run tests from ini file
    if not os.path.exists(ASSET_OUTPUT_DIR):
        os.makedirs(ASSET_OUTPUT_DIR)
    if not sut_log_file:
        sut_log_file = os.path.join(ASSET_OUTPUT_DIR, SUT_LOG_NAME)
    sut_json_path = os.path.join(ASSET_OUTPUT_DIR, REPORT_JSON_NAME)
    print('sut_log_file=', sut_log_file)
    if (os.path.exists(sut_log_file)):
        os.remove(sut_log_file)

    sut_list = parse_sut_ini(sut_ini_path)
    make = Make_Helper(sut_log_file, use_toolchains, make_args)

    # Run test case for configuration and generate report json file:
    res_dict = dict()
    for test_info in sut_list:
        test_configuration = list()
        print(test_info.sut)
        for target in test_info.target:
            for toolchain in test_info.toolchain:
                for core in test_info.core:
                    # Skip VFP_SELECT for CM0+ targets
                    vfp_sel = [ None ] if core == "CM0P" else test_info.vfp
                    for vfp in vfp_sel:
                        for config in test_info.config:
                            test_case_result = 'PASS'
                            test_case_message = ''
                            try:
                                make.compile_sut(test_info.path, target, toolchain, core, vfp, config, verbose)
                            except MakeError as error:
                                print(bcolors.FAIL + "FAIL: %s" % error + bcolors.ENDC)
                                test_case_result = 'FAIL'
                                test_case_message = str(error)
                                print(test_case_message)
                                err = True
                            test_configuration.append({
                                "result": test_case_result,
                                "message": test_case_message,
                                "test": test_info.sut,
                                "configuration": {
                                    "TARGET": target,
                                    "TOOLCHAIN": toolchain,
                                    "CORE": core,
                                    "VFP_SELECT": vfp,
                                    "CONFIG": config
                                }
                            })

        # Add test case result in report
        res_dict[test_info.sut] = test_configuration

    # Dump summary
    print('\n'.join(make.make_summary))
    print()

    # Save report json file
    with open(sut_json_path, 'w') as json_file:
        json.dump(res_dict, json_file)

    if (err == False):
        print(bcolors.OKGREEN + "SUCCESS \n" + bcolors.ENDC)
    else:
        print(bcolors.FAIL + "FAIL \n" + bcolors.ENDC)
        sys.exit(1)

if __name__ == '__main__':
    main()
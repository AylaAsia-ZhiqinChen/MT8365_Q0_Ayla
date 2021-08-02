#!/usr/bin/env python

import os
import sys

def parse(filename, option):
    value = ''
    with open(filename,'r') as f:
        for line in f.readlines():
            if option in line:
                value = line[len(option + "="):]
    return value

def main():
    filename = "vendor/mediatek/proprietary/buildinfo/label.ini"
    option = "MTK_INTERNAL_BUILD_VERNO"
    optionvalue = parse(filename,option)
    if optionvalue.startswith("alps"):
        optionvalues = optionvalue[optionvalue.find("."):].replace("-","").replace(".","").replace("_","")
        if len(optionvalues) <=16:
            print optionvalues
        elif len(optionvalues) >16:
            print optionvalues[len(optionvalues)-16:]
    else:
        print optionvalue.replace("-","").replace(".","").replace("_","")


if '__main__'==__name__:
    ret = main()
    sys.exit(ret)

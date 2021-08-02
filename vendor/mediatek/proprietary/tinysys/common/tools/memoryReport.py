#!/usr/bin/env python

from __future__ import print_function
import sys
import os, os.path
import re
import string
import argparse
import ConfigParser

class LlvmMapFile:

    def __init__(self, f):
        self.f = f
        self.section_order = []
        self.sections = {}
        self.outOffset = 0
        self.inOffset = 0
        self.symbolOffset = 0
        self.index = 0

    def parse_header(self):
        line = next(self.f)
        self.outOffset = line.find('Out')
        self.inOffset = line.find('In')
        self.symbolOffset = line.find('Symbol')

    def parse_sections(self):
        last_out = None
        for rawLine in self.f:
            length = len(rawLine)
            vma, lma, size, align = re.split(r'\s+', rawLine.strip(), 4)[0:4]
            vma = int(vma, 16)
            lma = int(lma, 16)
            size = int(size, 16)
            align = int(align)

            if size == 0: continue

            if self.outOffset < length and rawLine[self.outOffset] != ' ':
                outData = rawLine[self.outOffset:].strip()
                inData = ''
                symbolData = ''
                self.section_order.append((outData, vma, size))
                self.sections[outData] = {"addr": vma, "size": size}
                self.sections[outData].setdefault("objects", [])
                last_out = outData
            elif self.inOffset < length and rawLine[self.inOffset] != ' ':
                outData = ''
                inData = rawLine[self.inOffset:].strip().split(':')[0].split('(')[0]
                symbolData = ''
                if inData.find('=') != -1: continue
                if not os.path.isabs(inData):
                    inData = os.path.abspath(inData).partition(os.getcwd() + '/')[2]
                sec = last_out + '.' + os.path.basename(inData)
                self.sections[last_out]["objects"].append([sec, vma, size, inData, []])
            elif self.symbolOffset < length and rawLine[self.symbolOffset] != ' ':
                outData = ''
                inData = ''
                symbolData = rawLine[self.symbolOffset:].strip()
                if self.sections[last_out]["objects"][-1][-1] != [] and self.sections[last_out]["objects"][-1][-1][-1][0] == vma:
                   continue
                self.sections[last_out]["objects"][-1][-1].append((vma, symbolData))

            # print("%s,%s,%s,%s,%s,%s,%s" %
            #       (vma, lma, size, align, outData, inData, symbolData))

def parse_path(path):
    if path.find('mtkeda') != -1:
        return path
    elif path.find('tinysys_out') != -1:
        return '/'.join(path.split('/')[1:])
    else:
        return '/'.join(path.split('_intermediates/')[-1].split('/')[2:])

def query_feature(features, path):
    feature = 'Platform'; subFeature = ''

    if path.find('heap') != -1:
        feature = 'Heap'

    elif path.find('HEAP') != -1:
        feature = 'Heap'
    elif path.find('RTOS') != -1:
        feature = 'RTOS'
    elif path.find('mtkeda') != -1:
        feature = 'C-lib'
    else:
        for k in features.iterkeys():
            if path.find(k) != -1:
                feature = (features[k])[0]
                subFeature = (features[k])[1]
                break
    return (feature, subFeature)

def main():
    p = argparse.ArgumentParser(prog='memoryReport.py', add_help=True,
                                description="Report memory map by features")
    p.add_argument('-d', action='store_true', default=False, dest='dump', help='dump all output')
    p.add_argument('-e', action='store_true', default=False, dest='excel', help='dump all output to excel')
    p.add_argument('projType', help='project type', metavar='Project_Type')
    p.add_argument('iniFile', help='feature list', metavar='Feature_List')
    p.add_argument('mapFile', help='memory map', metavar='Memory_Map')
    # p.add_argument('prefix', help='prefix', metavar='Path_Prefix')
    args = vars (p.parse_args())
    dump = args['dump']
    excel = args['excel']
    projType = args['projType'].upper()
    iniFile = args['iniFile']
    mapFile = args['mapFile']
    # prefix = args['prefix']
    platform = os.environ.get('PLATFORM').lower()
    settings = []
    features = {}
    sectionS = set()
    featureS = set(['Platform', 'Heap', 'RTOS', 'C-lib'])
    subFeatureS = set()
    records = []
    result1 = {}
    result2 = {}
    result3 = {}
    criteria = {}
    chart = None
    bar = None
    wb = None
    ws1 = None
    ws2 = None
    index1x = 0
    index1y = 0
    index2 = 0
    chartMin = 0
    chartMax = 0
    chartL = 0
    chartD = 0
    barMin = 0
    barMax = 0
    barL = 0
    barD = 0
    platformSection = ''
    ret = 0

    if projType == 'SSPM':
        settings = ['TinySys-COMMON', 'TinySys-SSPM']
    elif projType == 'SCP':
        settings = ['TinySys-COMMON', 'TinySys-SCP']
    elif projType == 'MCUPM':
        settings = ['TinySys-COMMON', 'TinySys-MCUPM']

    f = open(iniFile)
    config = ConfigParser.ConfigParser()
    config.optionxform = lambda option: option
    config.readfp(f)

    for i in settings:
        for j in config.items(i):
            features[j[0]] = j[1].strip().split(':')
            featureS.add(features[j[0]][0])
            subFeatureS.add(features[j[0]][1])

    platformSection = "%s-%s" % (projType, platform)
    if config.has_section(platformSection):
        for i in config.items(platformSection):
            criteria[i[0]] = i[1]

    f.close()
    featureS = filter(None, featureS)
    subFeatureS = filter(None, subFeatureS)

    f = open(mapFile)
    m = LlvmMapFile(f)
    m.parse_header()
    m.parse_sections()
    m.section_order.sort(None, lambda l: l[1])

    ignoredSection = set([".comment", ".riscv.attributes", ".symtab", ".shstrtab", ".strtab"])
    for k, addr, sz in m.section_order:
        # print("%08x %08x %s" % (addr, sz, k))
        section = k; size = sz
        if k.find('.debug_') != -1 or k in ignoredSection:
            continue
        sectionS.add(k)

        if m.sections[k]['objects'] == []:
            records.append((k, 'Platform', '', k, m.sections[k]['addr'], m.sections[k]['size'], '', k))
            continue

        for sec, addr, sz, obj, symbols in m.sections[k]['objects']:
            # print(" %08x %08x %s" % (addr, sz, obj.replace(prefix, "")))
            symbol = '.'.join((sec.split('.'))[2:])
            address = addr; size = sz
            # path = obj.replace(prefix, '')
            path = parse_path(obj)
            [feature, subFeature] = query_feature(features, path)
            if symbols and len(symbols) > 1:
                for i in range(len(symbols) -1):
                    address = symbols[i][0]; symbol = symbols[i][1]
                    # print("  %08x %s" % (address, symbol))
                    records.append([symbol, feature, subFeature, section, address, int(symbols[i+1][0]) - int(address), path, obj])
                i += 1
                records.append([symbols[i][1], feature, subFeature, section, symbols[i][0], int(addr) + int(sz) - int(symbols[i][0]), path, obj])
            elif len(symbols) == 1:
                i = 0
                records.append([symbols[i][1], feature, subFeature, section, symbols[i][0], size, path, obj])
            else:
                records.append((symbol, feature, subFeature, section, address, size, path, obj))

    f.close()

    for s in sectionS:
        for f in featureS:
            result1[(f, s)] = 0
            result3[f] = ""
        for f in subFeatureS:
            result2[(f, s)] = 0
            result3[f] = ""

    if excel:
        from openpyxl import Workbook
        from openpyxl.chart import Reference, Series, BarChart

        chartL = 1
        chartD = len(sectionS) + 2
        barL = 1
        barD = len(subFeatureS) + 2
        chart = BarChart()
        chart.type = "col"
        chart.grouping = "stacked"
        chart.overlap = 100
        chart.height = 13
        chart.width = 26
        chart.title = "Feature Overall"
        bar = BarChart()
        bar.type = "col"
        bar.grouping = "stacked"
        bar.overlap = 100
        bar.height = 13
        bar.width = 26
        bar.title = "Sub-feature View"
        wb = Workbook()
        ws1 = wb.create_sheet("Memory Report", 0)
        ws2 = wb.create_sheet("Symbols", 1)
        ws2.cell(row=1, column=1, value='symbol')
        ws2.cell(row=1, column=2, value='feature')
        ws2.cell(row=1, column=3, value='sub_feature')
        ws2.cell(row=1, column=4, value='section')
        ws2.cell(row=1, column=5, value='address')
        ws2.cell(row=1, column=6, value='size')
        ws2.cell(row=1, column=7, value='path')
        index2 = 1

    for symbol, feature, subFeature, section, address, size, path, obj in records:
        if dump: print("%-60s\t%-9s\t%-9s\t%-13s\t0x%08x\t%s\t%s" % (symbol, feature, subFeature, section, address, size, path))
        if excel:
            index2 = index2 + 1
            ws2.cell(row=index2, column=1, value=symbol)
            ws2.cell(row=index2, column=2, value=feature)
            ws2.cell(row=index2, column=3, value=subFeature)
            ws2.cell(row=index2, column=4, value=section)
            ws2.cell(row=index2, column=5, value=("0x%08x" % (address,)))
            ws2.cell(row=index2, column=6, value=size)
            ws2.cell(row=index2, column=7, value=path)

        if feature and section:
            result1[(feature, section)] += size
            result3[feature] = obj
        if subFeature and section:
            result2[(subFeature, section)] += size
            result3[feature] = obj

    if dump: print(); print(); print()
    if dump: print("%12s\t" % '', end='')
    if excel: index1x = 1; index1y = 1; chartMin = index1y + 1
    for s in sorted(sectionS):
        if dump: print("%9s\t" % s, end='')
        if excel:
            index1x += 1
            ws1.cell(row=index1y, column=index1x, value=s)
    if dump: print("%9s" % "Sum")
    if excel:
        index1x += 1
        ws1.cell(row=index1y, column=index1x, value="Sum")
    for f in sorted(featureS):
        if dump: print("%12s\t" % f, end='')
        if excel:
            index1y += 1
            index1x = 1
            ws1.cell(row=index1y, column=index1x, value=f)
        result1[(f, 'sum')] = 0
        for s in sorted(sectionS):
            v = result1.get((f, s), 0)
            result1[(f, 'sum')] += v
            if dump: print("%9d\t" % v, end='')
            if excel:
                index1x += 1
                ws1.cell(row=index1y, column=index1x, value=v)
        if dump: print("%9d" % result1[(f, 'sum')])
        if excel:
            index1x += 1
            ws1.cell(row=index1y, column=index1x, value=result1[(f, 'sum')])
    if excel: chartMax = index1y

    if dump: print(); print(); print()
    if dump: print("%12s\t" % '', end='')
    if excel: index1x = 1; index1y += 4; barMin = index1y + 1
    for s in sorted(sectionS):
        if dump: print("%9s\t" % s, end='')
        if excel:
            index1x += 1
            ws1.cell(row=index1y, column=index1x, value=s)
    if dump: print("%9s" % "Sum")
    if excel:
        index1x += 1
        ws1.cell(row=index1y, column=index1x, value="Sum")
    for f in sorted(subFeatureS):
        if dump: print("%12s\t" % f, end='')
        if excel:
            index1y += 1
            index1x = 1
            ws1.cell(row=index1y, column=index1x, value=f)
        result2[(f, 'sum')] = 0
        for s in sorted(sectionS):
            v = result2.get((f, s), 0)
            result2[(f, 'sum')] += v
            if dump: print("%9d\t" % v, end='')
            if excel:
                index1x += 1
                ws1.cell(row=index1y, column=index1x, value=v)
        if dump: print("%9d" % result2[(f, 'sum')])
        if excel:
            index1x += 1
            ws1.cell(row=index1y, column=index1x, value=result2[(f, 'sum')])
    if excel: barMax = index1y

    if excel:
        labels = Reference(ws1, min_col=chartL, min_row=chartMin, max_row=chartMax)
        data = Reference(ws1, min_col=2, min_row=1, max_col=chartD-1, max_row=chartMax)
        chart.add_data(data=data, titles_from_data=True)
        chart.set_categories(labels)
        ws1.add_chart(chart, "A%d" % (barMax+4))

        labels = Reference(ws1, min_col=barL, min_row=barMin, max_row=barMax)
        data = Reference(ws1, min_col=2, min_row=barMin-1, max_col=barD-1, max_row=barMax)
        bar.add_data(data=data, titles_from_data=True)
        bar.set_categories(labels)
        ws1.add_chart(bar, "A%d" % (barMax+4+24+4))

        wb.save('memory_report.xlsx')

    if config.has_section(platformSection):
        if dump: print(); print()
        print()
        for f in sorted(featureS):
            if f == 'Peripheral':
                continue
            c = int(criteria.get(f, 0))
            m = int(result1.get((f, 'sum'), 0))
            if m > c:
                print("%s: %s(%d>%d) is out of memory limitation" % (projType, f, m, c))
                if result3[f]: print("%s: error: out of memroy limitation" % result3[f])
                ret = 13
        for f in sorted(subFeatureS):
            c = int(criteria.get(f, 0))
            m = int(result2.get((f, 'sum'), 0))
            if m > c:
                print("%s: %s(%d>%d) is out of memory limitation" % (projType, f, m, c))
                if result3[f]: print("%s: error: out of memroy limitation" % result3[f])
                ret = 13

        if ret == 0: print("%s: pass memory limitation check" %(projType)); print()
    return ret

if '__main__'==__name__:
        ret = main()
        sys.exit(ret)

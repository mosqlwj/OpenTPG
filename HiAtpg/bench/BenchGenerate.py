# This is a sample Python script.

# Press Shift+F10 to execute it or replace it with your code.
# Press Double Shift to search everywhere for classes, files, tool windows, actions, and settings.
import argparse
import sys

class Gate:
    def __init__(self):
        self.typename = ''
        self.gatename = ''
        self.ports = []

    def GetText(self):
        if self.typename == 'INPUT' or self.typename == 'OUTPUT':
            linestr = self.typename + '(' + self.GetPorts() + ')\n'
            return linestr
        linestr = self.gatename + ' = ' + self.typename + "(" + self.GetPorts() + ')\n'
        return linestr

    def GetPorts(self):
        return ",".join(self.ports)

class TestBench:
    def __init__(self):
        self.inputs = []
        self.outputs = []
        self.dffs = []
        self.coms = []
    def AddGate(self,gate):
        if gate.typename == 'INPUT':
            self.inputs.append(gate)
        elif gate.typename == 'OUTPUT':
            self.outputs.append(gate)
        elif gate.typename == 'DFF':
            self.dffs.append(gate)
        else:
            self.coms.append(gate)

def ParsePorts(line,index, filenum):
    portstr = line[index:len(line)-1]
    ports = portstr.split(',')
    portlen = len(ports)
    for i in range(portlen):
        ports[i] = ports[i].strip() + '_' + filenum
    return ports

def ParseLine(line, linenum, filenum):
    content = line.strip()
    if len(content) == 0:
        return
    if content.startswith('#'):
        return
    gate = Gate()
    if content.startswith('INPUT'):
        gate.typename = 'INPUT'
        gate.ports = ParsePorts(content,6, filenum)
        return gate
    if content.startswith('OUTPUT'):
        gate.typename = 'OUTPUT'
        gate.ports = ParsePorts(content,7,filenum)
        return gate
    strlist = content.split('=')
    if len(strlist) != 2:
        print("Format error1: %d" %linenum)
        return
    gate.gatename = strlist[0].strip() + '_' + filenum
    rightstr = strlist[1].strip()
    listtype = rightstr.split('(')
    if len(listtype) != 2:
        print("Format error2: %d" %linenum)
    gate.typename = listtype[0].strip()
    gate.ports = ParsePorts(listtype[1],0, filenum)
    return gate

def ParseFile(filename, filenum):
    filetext = open(filename,'r')
    testbench = TestBench()
    linenum = 1
    for line in filetext.readlines():
        gate = ParseLine(line, linenum, filenum)
        linenum += 1
        if gate != None:
            testbench.AddGate(gate)
    filetext.close()
    return testbench

def WriteFile(testbench, filename):
    outfile = open(filename,'w')
    for gate in testbench.inputs:
        outfile.write(gate.GetText())
    outfile.write("\n")
    for gate in testbench.outputs:
        outfile.write(gate.GetText())
    outfile.write("\n")
    for gate in testbench.dffs:
        outfile.write(gate.GetText())
    for gate in testbench.coms:
        outfile.write(gate.GetText())
    outfile.close()

def CombineTwoBenchV(bench1, bench2):
    bench1.dffs.extend(bench2.dffs)
    bench1.coms.extend(bench2.coms)
    bench1output = bench1.outputs.copy()
    bench1.outputs.clear()
    outputlen1 = len(bench1output)
    inputlen2 = len(bench2.inputs)
    mincount = min(outputlen1,inputlen2)
    for i in range(mincount):
        gate = Gate()
        gate.typename = 'BUFF'
        gate.gatename = bench2.inputs[i].ports[0]
        gate.ports = bench1output[i].ports
        bench1.coms.append(gate)
    bench1.outputs = bench2.outputs
    if outputlen1 > inputlen2:
        for i in range(mincount,outputlen1):
            bench1.outputs.append(bench1output[i])
    elif outputlen1 < inputlen2:
        for i in range(mincount,inputlen2):
            bench1.inputs.append(bench2.inputs[i])
    return bench1

def _parse_option():
    #默認會自動添加 -h參數， 不需要的可以設置 add_help=False
    parser = argparse.ArgumentParser()
    #parser = argparse.ArgumentParser(add_help=False)
    parser.add_argument("-i",metavar="xxx.bench",dest="input_files",nargs="+", type=str,required=True, help="input bench file")
    parser.add_argument("-o",metavar="xxx.bench", dest="output_file", type=str,required=True, help="output bench file")
    args = parser.parse_args()
    return args

def main():
    # parse args
    parseArgs = _parse_option()
    inputfiles = parseArgs.input_files
    outputfile = parseArgs.output_file
    benchlist = []
    for i in range(len(inputfiles)):
        benchlist.append(ParseFile(inputfiles[i],'%d'%i))
    comBench = benchlist[0]
    for i in range(1,len(benchlist)):
        comBench = CombineTwoBenchV(comBench,benchlist[i])
    WriteFile(comBench,outputfile)

# Press the green button in the gutter to run the script.
if __name__ == '__main__':
    main()

# See PyCharm help at https://www.jetbrains.com/help/pycharm/

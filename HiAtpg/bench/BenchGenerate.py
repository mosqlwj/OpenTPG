# This is a sample Python script.

# Press Shift+F10 to execute it or replace it with your code.
# Press Double Shift to search everywhere for classes, files, tool windows, actions, and settings.
import argparse
import sys

# INPUT, OUTPUT
# BUFF NOT
# AND,NAMD
# OR NOR
# DFF
# XOR XNOR

GateTypeOfDir = {'INPUT':0, 'OUTPUT':1, 'DFF':2, 'NOT':3, 'BUFF':4,'AND':5, 'NAND':6,'OR':7,'NOR':8,'XOR':9,'XNOR':10}
GateNameOfDir = {0:'INPUT', 1:'OUTPUT', 2:'DFF', 3:'NOT', 4:'BUFF', 5:'AND', 6:'NAND', 7:'OR', 8:'NOR', 9:'XOR', 10:'XNOR'}
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
        self.allGates = [list() for i in range(len(GateTypeOfDir))]
    def AddGate(self,gate):
        self.allGates[GateTypeOfDir[gate.typename]].append(gate)

def ParsePorts(line,index, filenum):
    portstr = line[index:len(line)-1]
    ports = portstr.split(',')
    portlen = len(ports)
    if portlen == 0:
        return
    for i in range(portlen):
        ports[i] = ports[i].strip() + filenum
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
        if len(gate.ports) != 1:
            print("INPUT Format error: %d"%linenum)
        if gate.ports[0][:-1] == 'clk1' or gate.ports[0][:-1] == 'clk2':
            return
        return gate
    if content.startswith('OUTPUT'):
        gate.typename = 'OUTPUT'
        gate.ports = ParsePorts(content,7,filenum)
        if len(gate.ports) != 1:
            print("OUTPUT Format error: %d"%linenum)
        return gate
    strlist = content.split('=')
    if len(strlist) != 2:
        print("Format error1: %d" %linenum)
        return
    gate.gatename = strlist[0].strip() + filenum
    rightstr = strlist[1].strip()
    listtype = rightstr.split('(')
    if len(listtype) != 2:
        print("Format error2: %d" %linenum)
    gate.typename = listtype[0].strip()
    gate.ports = ParsePorts(listtype[1],0, filenum)
    if gate.ports == None:
        print("Gate floating error: %d" %linenum)
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

def CombineTwoBenchV(bench1, bench2):
    for i in range(GateTypeOfDir['DFF'],len(GateTypeOfDir)):
        bench1.allGates[i].extend(bench2.allGates[i])
    outputIndex = GateTypeOfDir['OUTPUT']
    bench1output = bench1.allGates[outputIndex].copy()
    bench1.allGates[outputIndex].clear()
    outputlen1 = len(bench1output)
    inputIndex = GateTypeOfDir['INPUT']
    bench2input = bench2.allGates[inputIndex]
    inputlen2 = len(bench2input)
    mincount = min(outputlen1,inputlen2)
    for i in range(mincount):
        gate = Gate()
        gate.typename = 'BUFF'
        gate.gatename = bench2input[i].ports[0]
        gate.ports = bench1output[i].ports
        bench1.allGates[GateTypeOfDir['BUFF']].append(gate)
    bench1.allGates[outputIndex] = bench2.allGates[outputIndex]
    if outputlen1 > inputlen2:
        for i in range(mincount,outputlen1):
            bench1.allGates[outputIndex].append(bench1output[i])
    elif outputlen1 < inputlen2:
        for i in range(mincount,inputlen2):
            bench1.allGates[inputIndex].append(bench2.allGates[inputIndex][i])
    return bench1

def CombineTwoBenchH(bench1, bench2, index):
    outindex = GateTypeOfDir['OUTPUT']
    for i in range(len(GateTypeOfDir)):
        if i != outindex:
            bench1.allGates[i].extend(bench2.allGates[i])
    bench1output = bench1.allGates[outindex].copy()
    bench2output = bench2.allGates[outindex]
    bench1.allGates[outindex].clear()
    ben1outlen = len(bench1output)
    ben2outlen = len(bench2output)
    mincount = min(ben1outlen, ben2outlen)
    for i in range(mincount):
        gate = Gate()
        gate.typename = 'AND'
        gate.gatename = 'gand' + '%d'%i +  '%d'%index
        gate.ports.append(bench1output[i].ports[0])
        gate.ports.append(bench2output[i].ports[0])
        bench1.allGates[GateTypeOfDir['AND']].append(gate)
        gate2 = Gate()
        gate2.typename = 'OUTPUT'
        gate2.ports.append(gate.gatename)
        bench1.allGates[GateTypeOfDir['OUTPUT']].append(gate2)
    if ben1outlen > ben2outlen:
        for i in range(mincount, ben1outlen):
            bench1.allGates[outindex].append(bench1output[i])
    elif ben1outlen < ben2outlen:
        for i in range(mincount, ben2outlen):
            bench1.allGates[outindex].append(bench2output[i])
    return bench1

def AddClkToDFF(bench):
    dffs = bench.allGates[GateTypeOfDir['DFF']]
    dfflen = len(dffs)
    if dfflen == 0:
        return
    for i in range(dfflen):
        dff = dffs[i]
        if len(dff.ports) == 2:
            dff.ports[0] = dff.ports[0][:-1]
            continue
        elif len(dff.ports) == 1:
            clkname = 'clk1' if i%2 == 0 else 'clk2'
            dff.ports.insert(0,clkname)
        else:
            print("DFF port num error: " %len(dff.ports))
    inputs = bench.allGates[GateTypeOfDir['INPUT']]
    gate = Gate()
    gate.typename = 'INPUT'
    gate.ports.insert(0,'clk1')
    inputs.insert(0,gate)
    gate = Gate()
    gate.typename = 'INPUT'
    gate.ports.insert(0,'clk2')
    inputs.insert(1,gate)
    return bench

def WriteFile(testbench, filename):
    outfile = open(filename,'w')
    headerline = '# ' + '%d'%len(testbench.allGates[GateTypeOfDir['INPUT']]) + ' inputs\n'
    outfile.write(headerline)
    headerline = '# ' + '%d'%len(testbench.allGates[GateTypeOfDir['OUTPUT']]) + ' outputs\n'
    outfile.write(headerline)
    headerline = '# ' + '%d'%len(testbench.allGates[GateTypeOfDir['DFF']]) + ' D-type flipflops\n'
    outfile.write(headerline)
    headerline = '# ' + '%d'%len(testbench.allGates[GateTypeOfDir['NOT']]) + ' inverters\n'
    outfile.write(headerline)
    bufflen = len(testbench.allGates[GateTypeOfDir['BUFF']])
    if bufflen > 0:
        headerline = '# ' + '%d'%bufflen + ' buffers\n'
        outfile.write(headerline)
    others = 0
    tempstr ='('
    for i in range(GateTypeOfDir['AND'],len(GateTypeOfDir)):
        tmplen = len(testbench.allGates[i])
        if tmplen > 0:
            if others > 0:
                tempstr += ' + '
            tempstr += '%d'%tmplen + ' ' + GateNameOfDir[i] + 's'
            others += tmplen
    tempstr += ')\n\n'
    headerline = '# ' + '%d'%others + ' gates ' + tempstr
    outfile.write(headerline)
    for i in range(len(testbench.allGates)):
        gates = testbench.allGates[i]
        for j in range(len(gates)):
            gate = gates[j]
            outfile.write(gate.GetText())
        outfile.write("\n")
    outfile.close()

def _parse_option():
    #默認會自動添加 -h參數， 不需要的可以設置 add_help=False
    parser = argparse.ArgumentParser()
    #parser = argparse.ArgumentParser(add_help=False)
    parser.add_argument("-i",metavar="xxx.bench",dest="input_files",nargs="+", type=str,required=True, help="input bench file")
    parser.add_argument("-o",metavar="xxx.bench", dest="output_file", type=str,required=True, help="output bench file")
    parser.add_argument("-s", metavar="combine strategy: H", dest="strategy", type=str, required=False,
                        help="set combine strategy")
    args = parser.parse_args()
    return args

def main():
    # parse args
    parseArgs = _parse_option()
    inputfiles = parseArgs.input_files
    outputfile = parseArgs.output_file
    strategy = parseArgs.strategy
    benchlist = []
    for i in range(len(inputfiles)):
        benchlist.append(ParseFile(inputfiles[i],'%d'%i))
    comBench = benchlist[0]
    for i in range(1,len(benchlist)):
        if strategy == 'H':
            comBench = CombineTwoBenchH(comBench,benchlist[i],i)
        else:
            comBench = CombineTwoBenchV(comBench,benchlist[i])
    AddClkToDFF(comBench)
    WriteFile(comBench,outputfile)

# Press the green button in the gutter to run the script.
if __name__ == '__main__':
    main()

# See PyCharm help at https://www.jetbrains.com/help/pycharm/

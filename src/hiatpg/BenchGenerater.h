//
// Created by 50447 on 2020/7/4.
//

#ifndef HIATPG_BENCHGENERATER_H
#define HIATPG_BENCHGENERATER_H
#include <string>
#include <fstream>
#include <vector>
class Gate;
using std::string;
using std::vector;
using std::ofstream;
typedef vector<Gate*> FlattenGates;
enum EGateGroup {
    GROUP_PI,
    GROUP_COM,
    GROUP_CONNPO,
    GROUP_PO,
    GROUP_SIZE
};
class BenchGenerater {
public:
    BenchGenerater()
    {
        fileName = "_MergedBench.bench";
    }
    ~BenchGenerater()
    {
    }

public:
    bool CombineBenchFile(const vector<string>& files, int times);
private:
    void CombineGate(vector<FlattenGates>& gates, vector<FlattenGates>& divideGates);
    void CopyFlattenGates(vector<FlattenGates>& gates);
    bool SaveToFile(vector<FlattenGates>& gates);
    void WriteLogicGates(const vector<Gate*>& gates, ofstream& ofile);
private:
    std::string fileName;

};


#endif //HIATPG_BENCHGENERATER_H

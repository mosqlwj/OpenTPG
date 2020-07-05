#include "SimGood.h"

#include <queue>

#include "SimUtil.h"

SimGood::SimGood(NetlistParser* parser) : netlistParser(parser) {
    valueManager = new ValueManager(parser->GetFlattenGates().size());
    goodMechine = new SimGM(parser->GetFlattenGates().size());
    memset(goodMechine, 0, sizeof(uint64_t) * parser->GetFlattenGates().size());
}

SimGood::~SimGood() {
    if (nullptr != valueManager) {
        delete valueManager;
        valueManager = nullptr;
    }
    if (nullptr != goodMechine) {
        delete goodMechine;
        goodMechine = nullptr;
    }
}

SimGood::DoSim(uint64_t mask) {
    queue<Gate*> que;

    // 1.prepare input node
    vector<Gate*> flattenGate = netlistParser->GetFlattenGates();
    for (int i = netlistParser->GetPIBegin(); i < netlistParser->GetPIEnd(); i++) {
        que.enqueue(flattenGate[i]);
    }
    for (int i = netlistParser->GetTie0Begin(); i < netlistParser->GetTie0End(); i++) {
        que.enqueue(flattenGate[i]);
    }
    for (int i = netlistParser->GetTie1Begin(); i < netlistParser->GetTie1End(); i++) {
        que.enqueue(flattenGate[i]);
    }

    // 2.do event driven simulator
    Gate* curGate = nullptr;
    while (!que.Empty()) {
        curGate = que->dequeue();
        switch (curGate->GetGateFins()) {
            case 1:
                SimUtil::SimGate1(curGate, goodMechine, mask);
                break;
            case 2:
                SimUtil::SimGate2(curGate, goodMechine, mask);
                break;
            case 3:
                SimUtil::SimGate3(curGate, goodMechine, mask);
                break;
            case 4:
                SimUtil::SimGate4(curGate, goodMechine, mask);
                break;
            default:
                cout << "ERROR:Not Support This Gate." << endl;
        }
        valueManager->Set(curGate->GetGateId());
        for (auto fanout : curGate->GetFanoutGate()) {
            if (valueManager->Contains(fanout->GetGateId())) {
                continue;
            }
            que.enqueue(fanout);
        }
    }
}
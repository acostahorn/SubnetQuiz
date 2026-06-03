#ifndef SUBNETCALCULATOR_H
#define SUBNETCALCULATOR_H

#include <cstdint>
#include <QString>



struct SubnetDetails {
    uint32_t firstUsable;
    uint32_t lastUsable;
    uint32_t broadcast;
    int numHosts;
};

struct SubnetQuestion {
    SubnetDetails answers;
    uint32_t networkId;     // To pass back to SubnetQuiz member variables
    int cidr;
};

class SubnetCalculator {
public:
    static uint32_t parseIp(QString ip);
    static QString ipToString (uint32_t ip);
    static SubnetDetails calculateSubnet(uint32_t networkId, int cidr);
    static SubnetQuestion generateQuestion();
};

#endif // SUBNETCALCULATOR_H


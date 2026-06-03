#include "subnetcalculator.h"
#include <QStringList>
#include <random>
#include <cmath>

SubnetDetails SubnetCalculator::calculateSubnet(uint32_t currentNetworkId, int currentCidr) {
    SubnetDetails result;


    result.numHosts = (1 << (32 - currentCidr)) - 2;
    result.firstUsable = currentNetworkId + 1;

    result.broadcast = currentNetworkId + result.numHosts + 1;
    result.lastUsable = result.broadcast - 1;

    return result;
}



SubnetQuestion SubnetCalculator::generateQuestion() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> ipDist(0, 255);
    std::uniform_int_distribution<> cidrDist(8, 30);

    uint8_t o1 = ipDist(gen);
    uint8_t o2 = ipDist(gen);
    uint8_t o3 = ipDist(gen);
    uint8_t o4 = ipDist(gen);
    int cidr = cidrDist(gen);

    // Efficiently pack 4 octets into a single 32-bit binary integer
    uint32_t binaryIp = (o1 << 24) | (o2 << 16) | (o3 << 8) | o4;

    // Calculate the subnet mask based on CIDR
    uint32_t mask = (cidr == 0) ? 0 : (~0U) << (32 - cidr);

    uint32_t networkIp = binaryIp & mask;

     SubnetQuestion q;

    // Pass the clean binary IP to your calculator engine function to fill the answers!
    q.answers = calculateSubnet(networkIp, cidr);

    q.networkId = networkIp;
    q.cidr = cidr;

    return q;
}

uint32_t SubnetCalculator::parseIp(QString ip) {
    QStringList parts = ip.split('.');

    // Using toUInt() ensures we handle the full 0-255 range correctly
    uint32_t o1 = parts[0].toUInt();
    uint32_t o2 = parts[1].toUInt();
    uint32_t o3 = parts[2].toUInt();
    uint32_t o4 = parts[3].toUInt();

    return (o1 << 24) | (o2 << 16) | (o3 << 8) | o4;
}

QString SubnetCalculator::ipToString(uint32_t ip) {
    return QString("%1.%2.%3.%4")
    .arg((ip >> 24) & 0xFF)
        .arg((ip >> 16) & 0xFF)
        .arg((ip >> 8) & 0xFF)
        .arg(ip & 0xFF);
}
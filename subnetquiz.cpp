#include "subnetquiz.h"
#include <QGridLayout>
#include <QVBoxLayout>
#include <QFont>

SubnetQuiz::SubnetQuiz(QWidget *parent) : QWidget(parent) {
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    // QUESTION AREA

    QLabel *headerLabel = new QLabel("Calculate the Subnet Details for:");
    questionLabel = new QLabel("192.168.10.0 /24");
    QFont qFont;
    qFont.setPointSize(16);
    qFont.setBold(true);
    questionLabel->setFont(qFont);
    questionLabel->setAlignment(Qt::AlignCenter);

    // QUIZ GRID
    QGridLayout *quizGrid = new QGridLayout();

    firstAddrInput = new QLineEdit();
    lastAddrInput = new QLineEdit();
    broadcastInput = new QLineEdit();
    hostsInput = new QLineEdit();

    quizGrid->addWidget(new QLabel("First Usable IP:"), 0, 0);
    quizGrid->addWidget(firstAddrInput, 0, 1);

    quizGrid->addWidget(new QLabel("Last Usable IP:"), 1, 0);
    quizGrid->addWidget(lastAddrInput, 1, 1);

    quizGrid->addWidget(new QLabel("Broadcast Address:"), 2, 0);
    quizGrid->addWidget(broadcastInput, 2, 1);

    quizGrid->addWidget(new QLabel("Number of Hosts:"), 3, 0);
    quizGrid->addWidget(hostsInput, 3, 1);

    // SUBMIT BUTTON
    submitButton = new QPushButton("Check it Out");

    feedbackLabel = new QLabel();


    mainLayout->addWidget(headerLabel);
    mainLayout->addWidget(questionLabel);
    mainLayout->addSpacing(10);
    mainLayout->addLayout(quizGrid);
    mainLayout->addSpacing(10);
    mainLayout->addWidget(submitButton);
    mainLayout->addWidget(feedbackLabel);

    generateNewQuestion();
    connect(submitButton, &QPushButton::clicked, this, &SubnetQuiz::checkAnswer);


    setWindowTitle("Subnet Trainer v1.0");
    resize(400, 350);



}

QuizData SubnetQuiz::generateNewQuestion(){
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> ipDist(0,255);
    std::uniform_int_distribution<> cidrDist(8,30);

    int cidr = cidrDist(gen);
    uint32_t rawIp = (static_cast<uint32_t>(ipDist(gen)) << 24) |
                     (static_cast<uint32_t>(ipDist(gen)) << 16) |
                     (static_cast<uint32_t>(ipDist(gen)) << 8)  |
                     static_cast<uint32_t>(ipDist(gen));

    uint32_t mask = (cidr == 0) ? 0 : 0xFFFFFFFF << (32- cidr);
    uint32_t networkId = rawIp & mask;


    this->currentNetworkId = networkId;
    this->currentCidr = cidr;


    updateDisplay();

    QuizData result;
    result.networkId = networkId;
    result.cidr = cidr;
    return result;



}

bool SubnetQuiz::validate(QString input) {
    QStringList parts = input.split('.');

    // Check if we have exactly 4 parts
    if (parts.size() != 4) {
        return false;
    }

    // Check if each part is a valid number between 0 and 255
    for (const QString &part : parts) {
        bool ok = false;
        int value = part.toInt(&ok);
        if (!ok || value < 0 || value > 255) {
            return false;
        }
    }

    return true; // If we get here, it's a valid IP string!
}

bool SubnetQuiz::validateNumber(QString input) {
    bool ok = false;
    int value = input.toUInt(&ok);
    if (!ok || value < 2) {
        return false;
    }
    return true;
}

uint32_t SubnetQuiz::parseIp(QString ip) {
    QStringList parts = ip.split('.');

    // Using toUInt() ensures we handle the full 0-255 range correctly
    uint32_t o1 = parts[0].toUInt();
    uint32_t o2 = parts[1].toUInt();
    uint32_t o3 = parts[2].toUInt();
    uint32_t o4 = parts[3].toUInt();

    return (o1 << 24) | (o2 << 16) | (o3 << 8) | o4;
}


void SubnetQuiz::checkAnswer () {
    feedbackLabel->setText("");
    if (!validate(firstAddrInput->text())) {
        feedbackLabel->setText("Invalid First IP format! Use x.x.x.x");
        feedbackLabel->setStyleSheet("color: red; font-weight: bold;");
        return;
    }
    if (!validate(lastAddrInput->text())) {
        feedbackLabel->setText("Invalid Last IP Format! Use x.x.x.x");
        feedbackLabel->setStyleSheet("color: red; font-weight: bold;");
        return;
    }
    if (!validate(broadcastInput->text())) {
        feedbackLabel->setText("Invalid Broadcast Format! Use x.x.x.x");
        feedbackLabel->setStyleSheet("color: red; font-weight: bold;");
        return;
    }
    if(!validateNumber(hostsInput->text())) {
        feedbackLabel->setText("Use a valid number for the host count!");
        feedbackLabel->setStyleSheet("color: red; font-weight: bold;");
        return;
    }


    uint32_t gFirst = parseIp(firstAddrInput->text());
    uint32_t gLast = parseIp(lastAddrInput->text());
    uint32_t gBroad = parseIp(broadcastInput->text());
    uint32_t gHosts = (hostsInput->text()).toUInt();




    uint32_t cHosts = (1 << (32 - currentCidr)) - 2;
    uint32_t cFirst = currentNetworkId + 1;
    uint32_t cBroad = currentNetworkId + cHosts + 1; // Your new formula!
    uint32_t cLast  = cBroad - 1;

    if (gFirst == cFirst && gLast == cLast && gBroad == cBroad && gHosts == cHosts) {
        feedbackLabel->setText("Correct! Excellent work.");
        feedbackLabel->setStyleSheet("color: green; font-weight: bold;");
        firstAddrInput->clear();
        lastAddrInput->clear();
        broadcastInput->clear();
        hostsInput->clear();
        generateNewQuestion();

    } else {
        feedbackLabel->setText("Incorrect. Double-check your calculations.");
        feedbackLabel->setStyleSheet("color: orange;");
    }



}

void SubnetQuiz::updateDisplay () {
    int oct1 = (currentNetworkId >> 24) & 0xFF;
    int oct2 = (currentNetworkId >> 16) & 0xFF;
    int oct3 = (currentNetworkId >> 8) & 0xFF;
    int oct4 = currentNetworkId & 0xFF;

    QString ipString = QString("%1.%2.%3.%4 /%5")
                           .arg(oct1)
                           .arg(oct2)
                           .arg(oct3)
                           .arg(oct4)
                           .arg(currentCidr);

    questionLabel->setText(ipString);

}

SubnetQuiz::~SubnetQuiz() {
}

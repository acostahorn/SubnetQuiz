#include "subnetquiz.h"
#include "subnetcalculator.h"
#include <QGridLayout>
#include <QVBoxLayout>
#include <QFont>
#include <QMessageBox>
#include <iostream>
#include <QDebug>
#include <QInputDialog>
#include <QDir>

SubnetQuiz::SubnetQuiz(QWidget *parent) : QWidget(parent) {
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    scoreTrackerLabel = new QLabel(this);
    scoreTrackerLabel->setAlignment(Qt::AlignCenter);
    scoreTrackerLabel->setStyleSheet("font-size: 13px; font-weight: bold; color: #333333; margin-top: 5px;");

    mainLayout->addWidget(scoreTrackerLabel);

    // QUESTION AREA
    playerLabel = new QLabel(m_playerName);
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

    connect(firstAddrInput, &QLineEdit::returnPressed, this, [this]() {
        lastAddrInput->setFocus();
    });

    connect(lastAddrInput, &QLineEdit::returnPressed, this, [this]() {
        broadcastInput->setFocus();
    });

    connect(broadcastInput, &QLineEdit::returnPressed, this, [this]() {
        hostsInput->setFocus();
    });



    quizGrid->addWidget(new QLabel("First Usable IP:"), 1, 0);
    quizGrid->addWidget(firstAddrInput, 1, 1);

    quizGrid->addWidget(new QLabel("Last Usable IP:"), 2, 0);
    quizGrid->addWidget(lastAddrInput, 2, 1);

    quizGrid->addWidget(new QLabel("Broadcast Address:"), 3, 0);
    quizGrid->addWidget(broadcastInput, 3, 1);

    quizGrid->addWidget(new QLabel("Number of Hosts:"), 4, 0);
    quizGrid->addWidget(hostsInput, 4, 1);

    // SUBMIT BUTTON
    submitButton = new QPushButton("Check it Out");
    connect(hostsInput, &QLineEdit::returnPressed, submitButton, &QPushButton::click);
    feedbackLabel = new QLabel();

    // FINISH BUTTON
    finishButton = new QPushButton("Finish");

    mainLayout->addWidget(playerLabel);
    mainLayout->addWidget(headerLabel);
    mainLayout->addWidget(questionLabel);
    mainLayout->addSpacing(10);
    mainLayout->addLayout(quizGrid);
    mainLayout->addSpacing(10);
    mainLayout->addWidget(submitButton);
    mainLayout->addWidget(feedbackLabel);
    mainLayout->addSpacing(10);
    mainLayout->addWidget(finishButton);

    initializeGame();
    generateNewQuestion();
    connect(submitButton, &QPushButton::clicked, this, &SubnetQuiz::checkAnswer);
    connect(finishButton, &QPushButton::clicked, this, [this](){
        int percentage = 0;
        if(m_totalQuestions>0) {
        percentage = (m_score*100)/m_totalQuestions;
        }
        QMessageBox::information(this, "Quiz Finished",
        QString("Final Score: %1%").arg(percentage));
        emit finished();
    });


    setWindowTitle("Subnet Trainer v1.0");
    resize(400, 350);



}

QuizData SubnetQuiz::generateNewQuestion(){
    clearInputs();
    m_startTime = std::chrono::steady_clock::now();

    SubnetQuestion currentJob = SubnetCalculator::generateQuestion();
    this->currentNetworkId = currentJob.networkId;
    this->currentCidr = currentJob.cidr;


    // short cl = 5;

    // if (o1<128 && cidr == 8) {
    //     cl = 0;
    // }
    // else if (o1<192 && o1>=128 && cidr == 16) {
    //     cl = 1;
    // }
    // else if (o1<224 && o1 >=192 && cidr == 24) {
    //     cl = 2;
    // }
    // else if (o1<240 && o1>=224) {
    //     cl = 3;
    // }
    // else if (o1>239) {
    //     cl = 4;
    // }


    updateDisplay();
    QuizData result;
    result.networkId = this->currentNetworkId;
    result.cidr = this->currentCidr;
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



void SubnetQuiz::initializeGame() {
    bool ok;
    QString Name = QInputDialog::getText(this, tr("Welcome to the Subnet Trainer"),
                                         tr("Enter your name:"), QLineEdit::Normal, QDir::home().dirName(), &ok);
if (ok && !Name.isEmpty()) {
        m_playerName = Name;
    } else {
        m_playerName = "Anonymous Host";
    }

    playerLabel->setText(QString("Player: %1").arg(m_playerName));

    m_exercisesSolved = 0;
    m_runningScore = 0;

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

    m_totalExercisesAttemped++;

    auto endTime = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::seconds>(endTime - m_startTime);
    int secondsTaken = static_cast<int>(duration.count());


    // Explicitly scoping the static calls to your decoupled math utility class
    uint32_t gFirst = SubnetCalculator::parseIp(firstAddrInput->text().trimmed());
    uint32_t gLast  = SubnetCalculator::parseIp(lastAddrInput->text().trimmed());
    uint32_t gBroad = SubnetCalculator::parseIp(broadcastInput->text().trimmed());
    uint32_t gHosts = hostsInput->text().trimmed().toUInt();

    SubnetDetails target = SubnetCalculator::calculateSubnet(this->currentNetworkId, this->currentCidr);


    if (gFirst == target.firstUsable && gLast == target.lastUsable &&
        gBroad == target.broadcast   && gHosts == target.numHosts) {

        m_exercisesSolved++;

        // Give more time for larger, more complex subnets
        int timeLimit = 45;
        if (this->currentCidr < 24) {
            timeLimit = 90; // Give 90 seconds for tough Class A/B subnets
        } else if (this->currentCidr >= 30) {
            timeLimit = 20; // Tighten it up for easy point-to-point links
        }

        int speedBonus = std::max(10, 100 - (static_cast<int>(secondsTaken) * (100 / timeLimit)));
        m_runningScore += 50 + speedBonus;


        QMessageBox::information(this, "Correct!",
                                 QString("Great job %1!\nSession Score: %2/%3\nTime: %4s (+%5 XP)")
                                     .arg(m_playerName)
                                     .arg(m_exercisesSolved)
                                     .arg(m_totalExercisesAttemped)
                                     .arg(secondsTaken)
                                     .arg(50 + speedBonus));

        // Move to the next random question
        updateScoreDisplay();
        generateNewQuestion();
    } else {
        // Provide visual mistake feedback on screen without jumping to a new question
        feedbackLabel->setText("Incorrect. Double-check your subnet boundaries!");
        feedbackLabel->setStyleSheet("color: orange; font-weight: bold;");
    }




}

void SubnetQuiz::updateDisplay () {
    QString ipStr = SubnetCalculator::ipToString(this->currentNetworkId);

QString displayForm = QString("%1 /%2").arg(ipStr).arg(this->currentCidr);
questionLabel->setText(displayForm);

}

void SubnetQuiz::clearInputs () {
    firstAddrInput->clear();
    lastAddrInput->clear();
    broadcastInput->clear();
    hostsInput->clear();
    firstAddrInput->setFocus();
}

void SubnetQuiz::updateScoreDisplay() {
    scoreTrackerLabel->setText(QString("Solved: %1 / %2  |  Total XP: %3")
                                   .arg(m_exercisesSolved)
                                   .arg(m_totalExercisesAttemped)
                                   .arg(m_runningScore));
}

SubnetQuiz::~SubnetQuiz() {
}

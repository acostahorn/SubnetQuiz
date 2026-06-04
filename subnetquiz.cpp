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
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QStandardPaths>
#include <QDir>
#include <algorithm>

const int MATCH_DURATION = 3;

SubnetQuiz::SubnetQuiz(QWidget *parent) : QWidget(parent) {
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    scoreTrackerLabel = new QLabel(this);
    scoreTrackerLabel->setObjectName("scoreTrackerLabel");
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


    // PLAY AGAIN BUTTON
    // Inside SubnetQuiz::SubnetQuiz(...)
    playAgainButton = new QPushButton("Play Again", this);
    playAgainButton->setVisible(false); // Hidden until the LAST round ends
    playAgainButton->setStyleSheet("background-color: #2ec4b6; color: #1e1e24; font-weight: bold;");
    connect(playAgainButton, &QPushButton::clicked, this, &SubnetQuiz::resetGame);


    mainLayout->addWidget(playAgainButton);
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
        if (m_totalExercisesAttemped > 0) {
            percentage = (m_exercisesSolved * 100) / m_totalExercisesAttemped;
        }

        QMessageBox::information(this, "Quiz Finished Early",
                                 QString("Player: %1\nFinal Score: %2 XP\nAccuracy: %3%")
                                     .arg(m_playerName)
                                     .arg(m_runningScore)
                                     .arg(percentage));
        emit finished();
    });


    setWindowTitle("Subnet Trainer v1.0");
    resize(400, 350);



}

QuizData SubnetQuiz::generateNewQuestion(){
    clearInputs();


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
    m_startTime = std::chrono::steady_clock::now();
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
                                         tr("Enter your name:"), QLineEdit::Normal, "", &ok);
if (ok && !Name.isEmpty()) {
        m_playerName = Name;
    } else {
        m_playerName = "Anonymous Host";
    }

    playerLabel->setText(QString("Player: %1").arg(m_playerName));

    m_exercisesSolved = 0;
    m_runningScore = 0;

}

void SubnetQuiz::checkAnswer() {
    // 1. Every click counts as a recorded attempt
    m_totalExercisesAttemped++;

    // Safety guard using your correct variable name
    if (m_exercisesSolved >= MATCH_DURATION) {
        return;
    }

    // --- FORM VALIDATION ---
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
    if (!validateNumber(hostsInput->text())) {
        feedbackLabel->setText("Use a valid number for the host count!");
        feedbackLabel->setStyleSheet("color: red; font-weight: bold;");
        return;
    }

    // --- TIMING MATH ---
    auto endTime = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::seconds>(endTime - m_startTime);
    int secondsTaken = static_cast<int>(duration.count());

    // --- PARSE USER DATA ---
    uint32_t gFirst = SubnetCalculator::parseIp(firstAddrInput->text().trimmed());
    uint32_t gLast  = SubnetCalculator::parseIp(lastAddrInput->text().trimmed());
    uint32_t gBroad = SubnetCalculator::parseIp(broadcastInput->text().trimmed());
    uint32_t gHosts = hostsInput->text().trimmed().toUInt();

    SubnetDetails target = SubnetCalculator::calculateSubnet(this->currentNetworkId, this->currentCidr);

    // --- EVALUATION ---
    if (gFirst == target.firstUsable && gLast == target.lastUsable &&
        gBroad == target.broadcast   && gHosts == target.numHosts) {

        // SUCCESS!!!
        m_exercisesSolved++;

        int speedBonus = 10; // Baseline floor for taking over 3 minutes

        if (secondsTaken <= 20) {
            speedBonus = 120;  // 👑 Subnet God Mode (Under 20s) -> 170 XP total
        } else if (secondsTaken <= 40) {
            speedBonus = 100;  // 🔥 Elite Speed (Under 40s) -> 150 XP total
        } else if (secondsTaken <= 60) {
            speedBonus = 80;   // ⚡ Great Pace (Under 1 minute) -> 130 XP total
        } else if (secondsTaken <= 90) {
            speedBonus = 60;   // ⏱️ Good Pace (Under 1.5 mins) -> 110 XP total
        } else if (secondsTaken <= 120) {
            speedBonus = 40;   // 👍 Steady Pace (Under 2 mins) -> 90 XP total
        } else if (secondsTaken <= 180) {
            speedBonus = 25;   // 🛡️ Safe Zone (Under 3 mins) -> 75 XP total
        }

        m_runningScore += 50 + speedBonus;
        updateScoreDisplay();

        QMessageBox::information(this, "Correct!",
                                 QString("Round %1/%2 Correct!\nTime: %3s (+%4 XP)")
                                     .arg(m_exercisesSolved)
                                     .arg(MATCH_DURATION)
                                     .arg(secondsTaken)
                                     .arg(50 + speedBonus));

        // Let the outer logic handle round transitions smoothly now

    } else {
        // --- RETRY LOGIC -FAILURE!- ---
        QMessageBox::warning(this, "Incorrect Boundary",
                             "That configuration is incorrect.\n\nCheck your host bits and subnet mask details, then try again!");

        clearInputs();
        feedbackLabel->setText("Incorrect. Double-check your subnet boundaries!");
        feedbackLabel->setStyleSheet("color: orange; font-weight: bold;");

        updateScoreDisplay(); // Refresh to reflect their new total click attempt count
        return; // Halt execution so they stay on the same question
    }

    // --- GAME STATE NAVIGATION BOUNDARY ---
    if (m_exercisesSolved >= MATCH_DURATION) {
        // Disable text fields and toggle action buttons
        disableInputs();
        submitButton->setVisible(false);
        playAgainButton->setVisible(true);

        QMessageBox::information(this, "Match Finished!",
                                 QString("Game Over, %1!\nFinal Score: %2 XP\nAccuracy: %3/%4")
                                     .arg(m_playerName)
                                     .arg(m_runningScore)
                                     .arg(m_exercisesSolved)
                                     .arg(m_totalExercisesAttemped)); // <-- Dynamically maps click attempts!

        saveAndShowLeaderboard();
        feedbackLabel->setText("Match Complete! Click 'Play Again' to beat your score.");
        feedbackLabel->setStyleSheet("color: lime; font-weight: bold;");

    } else {
        // Clear text fields out and pull the next layout challenge smoothly!
        clearInputs();
        generateNewQuestion();
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
    scoreTrackerLabel->setStyleSheet("color: #cdd6f4; font-weight: bold; background: transparent;");
}

void SubnetQuiz::disableInputs() {
    // Replace these with the actual variable names of your QLineEdits / QPushButtons
    firstAddrInput->setEnabled(false);
    lastAddrInput->setEnabled(false);
    broadcastInput->setEnabled(false);
    hostsInput->setEnabled(false);
    submitButton->setEnabled(false);

    // Switch the text of your alert box to indicate completion
    feedbackLabel->setText("Match Complete! Check the Leaderboard.");
}

void SubnetQuiz::resetGame() {
    // 1. Reset game state variables
    initializeGame();
    m_runningScore = 0;
    m_exercisesSolved = 0;
    m_totalExercisesAttemped = 0;

    // 2. Re-enable all the input boxes and buttons
    firstAddrInput->setEnabled(true);
    lastAddrInput->setEnabled(true);
    broadcastInput->setEnabled(true);
    hostsInput->setEnabled(true);
    submitButton->setEnabled(true);

    // 3. Toggle button visibility back to game mode
    playAgainButton->setVisible(false);
    submitButton->setVisible(true);

    // 4. Wipe old text fields and scores
    clearInputs();
    updateScoreDisplay();
    feedbackLabel->setText("Good luck!");

    // 5. Kick off Round 1 of the new match

    generateNewQuestion();
}

void SubnetQuiz::saveAndShowLeaderboard() {
    QString writePath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir dir(writePath);
    if (!dir.exists()) {
        dir.mkpath(".");
    }
    QString filePath = dir.filePath("leaderboard.json");

    QJsonArray scoresArray;

    QFile file(filePath);
    if (file.open(QIODevice::ReadOnly)) {
        QJsonDocument loadDoc = QJsonDocument::fromJson(file.readAll());
        scoresArray = loadDoc.array();
        file.close();
    }

    // Change MATCH_DURATION to m_totalExercisesAttemped
    QJsonObject currentMatch;
    currentMatch["name"] = m_playerName.isEmpty() ? "Anonymous" : m_playerName;
    currentMatch["score"] = m_runningScore;
    currentMatch["accuracy"] = QString("%1/%2").arg(m_exercisesSolved).arg(m_totalExercisesAttemped);


    scoresArray.append(currentMatch);

    std::vector<QJsonObject> sortedEntries;
    for (int i=0; i <scoresArray.size(); ++i) {
        sortedEntries.push_back(scoresArray.at(i).toObject());
    }

    std::sort(sortedEntries.begin(),sortedEntries.end(), [](const QJsonObject& a, const QJsonObject& b) {
        return a["score"].toInt() > b["score"].toInt();
    });
    QJsonArray finalArray;
    int limit = std::min(10, static_cast<int>(sortedEntries.size()));
    for (int i = 0; i < limit; ++i) {
        finalArray.append(sortedEntries[i]);
    }

    // 6. Write the sorted data right back to the JSON file
    if (file.open(QIODevice::WriteOnly)) {
        QJsonDocument saveDoc(finalArray);
        file.write(saveDoc.toJson());
        file.close();
    }

    // 7. Render a clean visual leaderboard pop-up window
    QString leaderboardText = "=== LEADERBOARD TOP 10 ===\n\n";

    for (int i = 0; i < finalArray.size(); ++i) {
        QJsonObject row = finalArray.at(i).toObject();

        // Put a little indicator flag if this row is the score they literally just achieved
        QString accent = (row["name"].toString() == m_playerName && row["score"].toInt() == m_runningScore) ? " *YOUR SCORE*" : "";

        leaderboardText += QString("#%1 - %2: %3 XP (Accuracy: %4)%5\n")
                               .arg(i + 1)
                               .arg(row["name"].toString())
                               .arg(row["score"].toInt())
                               .arg(row["accuracy"].toString())
                               .arg(accent);
    }

    QMessageBox::information(this, "Match Finished - Hall of Fame", leaderboardText);
}


SubnetQuiz::~SubnetQuiz() {
}

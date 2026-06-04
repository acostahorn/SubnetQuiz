#ifndef SUBNETQUIZ_H
#define SUBNETQUIZ_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QComboBox>
#include <random>
#include <chrono>
#include <QString>
#include <QList>
#include <algorithm>

struct QuizData {
    uint32_t networkId;
    int cidr;
};

struct PlayerRecord {
    QString name;
    int score;
    int solved;
    int total;
    double accuracy;
};

class SubnetQuiz : public QWidget{

    Q_OBJECT

public:
    explicit SubnetQuiz(QWidget *parent = nullptr);
    QuizData generateNewQuestion();
    void updateDisplay();

    ~SubnetQuiz();

private:

    QString m_playerName;

    int m_totalQuestions = 0;
    int m_exercisesSolved = 0;
    int m_totalExercisesAttemped = 0;
    int m_runningScore = 0;

    std::chrono::time_point<std::chrono::steady_clock> m_startTime;


    uint32_t currentNetworkId;
    int currentCidr;
    short currentClass;


    QLabel *questionLabel;
    QLabel *scoreTrackerLabel;
    QLineEdit *firstAddrInput;
    QLineEdit *lastAddrInput;
    QLineEdit *broadcastInput;
    QLineEdit *hostsInput;

    QPushButton *submitButton;
    QPushButton *finishButton;
    QLabel *playerLabel;
    QLabel *feedbackLabel;

    QPushButton* playAgainButton;


signals:
    void finished();

private slots:
    void initializeGame();
    bool validate(QString input);
    bool validateNumber(QString input);
    void checkAnswer();
    void clearInputs();
    void updateScoreDisplay();
    void disableInputs();
    void resetGame();
    void saveAndShowLeaderboard();

};
#endif // SUBNETQUIZ_H

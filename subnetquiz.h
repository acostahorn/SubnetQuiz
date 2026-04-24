#ifndef SUBNETQUIZ_H
#define SUBNETQUIZ_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <random>

struct QuizData {
    uint32_t networkId;
    int cidr;
};

class SubnetQuiz : public QWidget{

    Q_OBJECT

public:
    explicit SubnetQuiz(QWidget *parent = nullptr);
    QuizData generateNewQuestion();
    void updateDisplay();

    ~SubnetQuiz();

private:


    uint32_t currentNetworkId;
    int currentCidr;


    QLabel *questionLabel;

    QLineEdit *firstAddrInput;
    QLineEdit *lastAddrInput;
    QLineEdit *broadcastInput;
    QLineEdit *hostsInput;

    QPushButton *submitButton;
    QLabel *feedbackLabel;

private slots:
    bool validate(QString input);
    bool validateNumber(QString input);
    uint32_t parseIp(QString ip);
    void checkAnswer();





};
#endif // SUBNETQUIZ_H

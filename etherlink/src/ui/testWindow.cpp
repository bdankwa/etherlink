/********************************************************************************
** Form generated from reading UI file 'ethernetTest.ui'
**
** Created by: Qt User Interface Compiler version 4.8.6
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef TESTWINDOW_H
#define TESTWINDOW_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QComboBox>
#include <QtGui/QFrame>
#include <QtGui/QGridLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QProgressBar>
#include <QtGui/QPushButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_Test
{
public:
    QWidget *gridLayoutWidget;
    QGridLayout *gridLayout;
    QLabel *label_4;
    QLabel *label_11;
    QLabel *label_5;
    QProgressBar *progressBar_bandwidth;
    QLabel *label_9;
    QLabel *label_7;
    QLabel *lable_CapacityTest_Status;
    QLabel *label_15;
    QLabel *label_6;
    QLabel *label_17;
    QLabel *label_loopback_rx_packets;
    QLabel *label_8;
    QProgressBar *progressBar_robustness;
    QLabel *label_12;
    QLabel *label_2;
    QLabel *label;
    QProgressBar *progressBar_loopback;
    QLabel *label_16;
    QLabel *label_loopback_tx_packets;
    QLabel *label_18;
    QLabel *label_3;
    QSpacerItem *horizontalSpacer;
    QLabel *label_10;
    QSpacerItem *horizontalSpacer_2;
    QPushButton *startTestButton;
    QPushButton *cancelTestButton;
    QFrame *line;
    QLabel *label_19;
    QComboBox *comboBox_channel_select;

    void setupUi(QWidget *Test)
    {
        if (Test->objectName().isEmpty())
            Test->setObjectName(QString::fromUtf8("Test"));
        Test->resize(449, 300);
        gridLayoutWidget = new QWidget(Test);
        gridLayoutWidget->setObjectName(QString::fromUtf8("gridLayoutWidget"));
        gridLayoutWidget->setGeometry(QRect(30, 70, 371, 186));
        gridLayout = new QGridLayout(gridLayoutWidget);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        gridLayout->setContentsMargins(0, 0, 0, 0);
        label_4 = new QLabel(gridLayoutWidget);
        label_4->setObjectName(QString::fromUtf8("label_4"));

        gridLayout->addWidget(label_4, 4, 0, 1, 1);

        label_11 = new QLabel(gridLayoutWidget);
        label_11->setObjectName(QString::fromUtf8("label_11"));

        gridLayout->addWidget(label_11, 1, 5, 1, 1);

        label_5 = new QLabel(gridLayoutWidget);
        label_5->setObjectName(QString::fromUtf8("label_5"));

        gridLayout->addWidget(label_5, 6, 0, 1, 1);

        progressBar_bandwidth = new QProgressBar(gridLayoutWidget);
        progressBar_bandwidth->setObjectName(QString::fromUtf8("progressBar_bandwidth"));
        progressBar_bandwidth->setValue(0);

        gridLayout->addWidget(progressBar_bandwidth, 5, 0, 1, 1);

        label_9 = new QLabel(gridLayoutWidget);
        label_9->setObjectName(QString::fromUtf8("label_9"));

        gridLayout->addWidget(label_9, 2, 4, 1, 1);

        label_7 = new QLabel(gridLayoutWidget);
        label_7->setObjectName(QString::fromUtf8("label_7"));

        gridLayout->addWidget(label_7, 5, 2, 1, 1);

        lable_CapacityTest_Status = new QLabel(gridLayoutWidget);
        lable_CapacityTest_Status->setObjectName(QString::fromUtf8("lable_CapacityTest_Status"));

        gridLayout->addWidget(lable_CapacityTest_Status, 3, 2, 1, 1);

        label_15 = new QLabel(gridLayoutWidget);
        label_15->setObjectName(QString::fromUtf8("label_15"));

        gridLayout->addWidget(label_15, 5, 4, 1, 1);

        label_6 = new QLabel(gridLayoutWidget);
        label_6->setObjectName(QString::fromUtf8("label_6"));

        gridLayout->addWidget(label_6, 1, 4, 1, 1);

        label_17 = new QLabel(gridLayoutWidget);
        label_17->setObjectName(QString::fromUtf8("label_17"));

        gridLayout->addWidget(label_17, 7, 4, 1, 1);

        label_loopback_rx_packets = new QLabel(gridLayoutWidget);
        label_loopback_rx_packets->setObjectName(QString::fromUtf8("label_loopback_rx_packets"));

        gridLayout->addWidget(label_loopback_rx_packets, 3, 5, 1, 1);

        label_8 = new QLabel(gridLayoutWidget);
        label_8->setObjectName(QString::fromUtf8("label_8"));

        gridLayout->addWidget(label_8, 7, 2, 1, 1);

        progressBar_robustness = new QProgressBar(gridLayoutWidget);
        progressBar_robustness->setObjectName(QString::fromUtf8("progressBar_robustness"));
        progressBar_robustness->setValue(0);

        gridLayout->addWidget(progressBar_robustness, 7, 0, 1, 1);

        label_12 = new QLabel(gridLayoutWidget);
        label_12->setObjectName(QString::fromUtf8("label_12"));

        gridLayout->addWidget(label_12, 0, 4, 1, 1);

        label_2 = new QLabel(gridLayoutWidget);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        gridLayout->addWidget(label_2, 0, 2, 1, 1);

        label = new QLabel(gridLayoutWidget);
        label->setObjectName(QString::fromUtf8("label"));

        gridLayout->addWidget(label, 0, 0, 1, 1);

        progressBar_loopback = new QProgressBar(gridLayoutWidget);
        progressBar_loopback->setObjectName(QString::fromUtf8("progressBar_loopback"));
        progressBar_loopback->setValue(0);

        gridLayout->addWidget(progressBar_loopback, 3, 0, 1, 1);

        label_16 = new QLabel(gridLayoutWidget);
        label_16->setObjectName(QString::fromUtf8("label_16"));

        gridLayout->addWidget(label_16, 5, 5, 1, 1);

        label_loopback_tx_packets = new QLabel(gridLayoutWidget);
        label_loopback_tx_packets->setObjectName(QString::fromUtf8("label_loopback_tx_packets"));

        gridLayout->addWidget(label_loopback_tx_packets, 3, 4, 1, 1);

        label_18 = new QLabel(gridLayoutWidget);
        label_18->setObjectName(QString::fromUtf8("label_18"));

        gridLayout->addWidget(label_18, 7, 5, 1, 1);

        label_3 = new QLabel(gridLayoutWidget);
        label_3->setObjectName(QString::fromUtf8("label_3"));

        gridLayout->addWidget(label_3, 2, 0, 1, 1);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout->addItem(horizontalSpacer, 0, 3, 1, 1);

        label_10 = new QLabel(gridLayoutWidget);
        label_10->setObjectName(QString::fromUtf8("label_10"));

        gridLayout->addWidget(label_10, 2, 5, 1, 1);

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout->addItem(horizontalSpacer_2, 0, 1, 1, 1);

        startTestButton = new QPushButton(Test);
        startTestButton->setObjectName(QString::fromUtf8("startTestButton"));
        startTestButton->setGeometry(QRect(200, 10, 87, 27));
        cancelTestButton = new QPushButton(Test);
        cancelTestButton->setObjectName(QString::fromUtf8("cancelTestButton"));
        cancelTestButton->setGeometry(QRect(310, 10, 87, 27));
        line = new QFrame(Test);
        line->setObjectName(QString::fromUtf8("line"));
        line->setGeometry(QRect(20, 40, 411, 20));
        line->setFrameShape(QFrame::HLine);
        line->setFrameShadow(QFrame::Sunken);
        label_19 = new QLabel(Test);
        label_19->setObjectName(QString::fromUtf8("label_19"));
        label_19->setGeometry(QRect(30, 260, 391, 41));
        comboBox_channel_select = new QComboBox(Test);
        comboBox_channel_select->setObjectName(QString::fromUtf8("comboBox_channel_select"));
        comboBox_channel_select->setGeometry(QRect(30, 10, 91, 24));

        retranslateUi(Test);

        QMetaObject::connectSlotsByName(Test);
    } // setupUi

    void retranslateUi(QWidget *Test)
    {
        Test->setWindowTitle(QApplication::translate("Test", "Ethernet Diagnostics", 0, QApplication::UnicodeUTF8));
        label_4->setText(QApplication::translate("Test", "Bandwidth Test", 0, QApplication::UnicodeUTF8));
        label_11->setText(QApplication::translate("Test", "Received", 0, QApplication::UnicodeUTF8));
        label_5->setText(QApplication::translate("Test", "Robustness Test", 0, QApplication::UnicodeUTF8));
        label_9->setText(QApplication::translate("Test", "(Packets)", 0, QApplication::UnicodeUTF8));
        label_7->setText(QApplication::translate("Test", "Ready", 0, QApplication::UnicodeUTF8));
        lable_CapacityTest_Status->setText(QApplication::translate("Test", "Ready", 0, QApplication::UnicodeUTF8));
        label_15->setText(QApplication::translate("Test", "0", 0, QApplication::UnicodeUTF8));
        label_6->setText(QApplication::translate("Test", "Sent", 0, QApplication::UnicodeUTF8));
        label_17->setText(QApplication::translate("Test", "0", 0, QApplication::UnicodeUTF8));
        label_loopback_rx_packets->setText(QApplication::translate("Test", "0", 0, QApplication::UnicodeUTF8));
        label_8->setText(QApplication::translate("Test", "Ready", 0, QApplication::UnicodeUTF8));
        label_12->setText(QApplication::translate("Test", "Results", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("Test", "Status", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("Test", "Test", 0, QApplication::UnicodeUTF8));
        label_16->setText(QApplication::translate("Test", "0", 0, QApplication::UnicodeUTF8));
        label_loopback_tx_packets->setText(QApplication::translate("Test", "0", 0, QApplication::UnicodeUTF8));
        label_18->setText(QApplication::translate("Test", "0", 0, QApplication::UnicodeUTF8));
        label_3->setText(QApplication::translate("Test", "LoopBack Test", 0, QApplication::UnicodeUTF8));
        label_10->setText(QApplication::translate("Test", "(Packets)", 0, QApplication::UnicodeUTF8));
        startTestButton->setText(QApplication::translate("Test", "Start", 0, QApplication::UnicodeUTF8));
        cancelTestButton->setText(QApplication::translate("Test", "Cancel", 0, QApplication::UnicodeUTF8));
        label_19->setText(QApplication::translate("Test", "IMPORTANT!: Ensure ONLY diagnostic Ethernet traffic from EMU.", 0, QApplication::UnicodeUTF8));
        comboBox_channel_select->clear();
        comboBox_channel_select->insertItems(0, QStringList()
         << QApplication::translate("Test", "Channel A", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("Test", "Channel B", 0, QApplication::UnicodeUTF8)
        );
    } // retranslateUi

};

namespace Ui {
    class Test: public Ui_Test {};
} // namespace Ui

QT_END_NAMESPACE

#endif // TESTWINDOW_H

/********************************************************************************
** Form generated from reading UI file 'recvfileop.ui'
**
** Created by: Qt User Interface Compiler version 5.9.9
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_RECVFILEOP_H
#define UI_RECVFILEOP_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QProgressBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_recvfileop
{
public:
    QGridLayout *gridLayout;
    QVBoxLayout *verticalLayout_2;
    QLabel *stLabel;
    QVBoxLayout *verticalLayout;
    QProgressBar *recvProg;
    QLineEdit *savePath;
    QPushButton *pushButton;
    QSpacerItem *horizontalSpacer_2;

    void setupUi(QWidget *recvfileop)
    {
        if (recvfileop->objectName().isEmpty())
            recvfileop->setObjectName(QStringLiteral("recvfileop"));
        recvfileop->resize(387, 161);
        gridLayout = new QGridLayout(recvfileop);
        gridLayout->setSpacing(6);
        gridLayout->setContentsMargins(11, 11, 11, 11);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        verticalLayout_2 = new QVBoxLayout();
        verticalLayout_2->setSpacing(6);
        verticalLayout_2->setObjectName(QStringLiteral("verticalLayout_2"));
        stLabel = new QLabel(recvfileop);
        stLabel->setObjectName(QStringLiteral("stLabel"));

        verticalLayout_2->addWidget(stLabel);

        verticalLayout = new QVBoxLayout();
        verticalLayout->setSpacing(6);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        recvProg = new QProgressBar(recvfileop);
        recvProg->setObjectName(QStringLiteral("recvProg"));
        recvProg->setValue(24);

        verticalLayout->addWidget(recvProg);

        savePath = new QLineEdit(recvfileop);
        savePath->setObjectName(QStringLiteral("savePath"));

        verticalLayout->addWidget(savePath);

        pushButton = new QPushButton(recvfileop);
        pushButton->setObjectName(QStringLiteral("pushButton"));

        verticalLayout->addWidget(pushButton);

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        verticalLayout->addItem(horizontalSpacer_2);


        verticalLayout_2->addLayout(verticalLayout);


        gridLayout->addLayout(verticalLayout_2, 0, 0, 1, 1);


        retranslateUi(recvfileop);

        QMetaObject::connectSlotsByName(recvfileop);
    } // setupUi

    void retranslateUi(QWidget *recvfileop)
    {
        recvfileop->setWindowTitle(QApplication::translate("recvfileop", "mRecvFile", Q_NULLPTR));
        stLabel->setText(QString());
        savePath->setText(QApplication::translate("recvfileop", "path to save", Q_NULLPTR));
        pushButton->setText(QApplication::translate("recvfileop", "input", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class recvfileop: public Ui_recvfileop {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_RECVFILEOP_H

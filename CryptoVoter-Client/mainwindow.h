/*************************************************************************
 *
 * COPYRIGHT (C) 2015 BLOCKCHAIN INNOVATION LABS, INC.
 * ALL RIGHTS RESERVED
 *
 * The CryptoVoter voting software is developed by 
 * Blockchain Innovation Labs, Inc., ("BCILabs") and 
 * is dual licensed under open-source and commercial product licenses.
 * By offering different licensing options, BCILabs supports giving
 * options to open-source communities and companies to pursue 
 * different product strategies.  Accordingly, BCI Labs offers 
 * CryptoVoter both under a commercial license for proprietary 
 * uses and, if utilized in connection with a licensed instance of 
 * Blockchain Innovation Labs, Inc.'s, webservices for the 
 * CryptoVoter client, the CryptoVoter client software is offered 
 * open-source under the GNU Affero General Public License, either 
 * version 3 of the License (AGPLv3) or (at your option) any later 
 * version, which requires any software derived from or using 
 * CryptoVoter to also be released open source under the same AGPLv3.  
 *
 * Therefore, in order to develop and use CryptoVoter, you must 
 * adhere to the obligations and definitions required by these 
 * license requirements. THUS, if you do not utilize a license 
 * instance of Blockchain Innovation Labs, Inc., for the related 
 * webservices or cannot adhere to the rules of the open-source 
 * license, a commercial license is required to use the product.
 * 
 * The AGPL strictly requires that any software derived from
 * or incorporating the CryptoVoter software must also
 * be released open source with the same AGPLv3 license
 * requirements attached.  Therefore, in order to develop
 * and use the CryptoVoter software under the open-source
 * license, you must utilize a licensed instance of Blockchain
 * Innovation Labs, Inc.'s, webservices and you must adhere to 
 * the obligations and definitions required by the AGPLv3. 
 * If you cannot adhere to the rules of the AGPL requiring open-source
 * versions of your source code to be made available, a
 * commerical license is available to allow the development
 * of proprietary versions of the software.
 *
 * A copy of the AGPLv3 can be found:  http://www.gnu.org/licenses.
 * The CryptoVoter open-source code can be found here:  
 * http://github.com/cryptovoter-client/cryptovoter
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 ***************************************************************************/
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSortFilterProxyModel>
#include <QStandardItem>
#include <QVector>
#include <QCloseEvent>
#include <QRadioButton>
#include "question.h"
#include <questionapidao.h>
#include "processform.h"
#include "chartform.h"
#include "aboutform.h"
#include "configform.h"
#include "instructionsform.h"
#include "votetransactionsform.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    /*!
     * \brief Constructor: Initialize the class atributes and connects the signals between bottons and slots
    */
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    ProcessForm processForm;
    ChartForm chartForm;
    QSortFilterProxyModel *pendingProxyModel;
    QSortFilterProxyModel *completeProxyModel;
    QStandardItemModel *modelPending;
    QStandardItemModel *modelComplete;
    QVector<Question> completeQuestions;
    QVector<QRadioButton*> radioButtons;
    QAction *aboutAction;
    QAction *configAction;
    QAction *instructionAction;
    QAction *termsAction;
    QAction *transactionsAction;
    QAction *copyrightAction;
    QMenu *helpMenu;
    AboutForm *about;
    ConfigForm *config;
    InstructionsForm *instructions;
    AboutForm *terms;
    AboutForm *copyright;
    VoteTransactionsForm *voteTransactions;

    /*!
     * \brief Inserts a row in the model used by the pending questions table
    */
    void addPendingQuestion(const QString &subject,
                 const QString &sender, const QString &date);
    /*!
     * \brief Inserts a row in the model used by the complete questions table
    */
    void addCompleteQuestion(const QString &subject,
                 const QString &sender, const QString &date);

    void addAllCompleteQuestion();
    QString getSelectedAnswer();

    /**
     * @brief MainWindow::prepareVotingActions
     * Dont Modify this method. Core Code.
     * @param votingType
     * @return
     */
    bool prepareVotingActions(int votingType, QString qid);

    /*!
     * \brief enableButtons
     * \param enable refresh and vote buttons once server data in loaded.
     */
    void enableButtons(bool enable);

    /*!
     * \brief Manual voting instructions
     * \return the text to show.
     */
    QString getManualVotingInstructions();

public slots:

    /*!
     * \brief Receives a vector with all the complete questions to update the table information
    */
    void refreshCompleteQuestion(QVector<Question> questions);

    /*!
     * \brief Receives a vector with all the pending questions to update the table information
    */
    void refreshPendingQuestion(QVector<Question> questions);

    /*!
     * \brief Receives the active question to update the information
    */
    void refreshActiveQuestion(Question questions);

    /*!
     * \brief Opens a confirm box with the answer selected by the user to the active question.
     *  Then, emits a signal with the information to the process who runs the vanitygen
    */
    void vote();
    /*!
     * \brief Allows to search a words on the pending questions table
    */
    void pendingFilterRegExpChanged();
    /*!
     * \brief Detects changes on the pending questions filter
    */
    void pendingFilterColumnChanged();

    /*!
     * \brief Allows to search a words on the complete questions table
    */
    void completeFilterRegExpChanged();
    /*!
     * \brief Detects changes on the complete questions filter
    */
    void completeFilterColumnChanged();

    /*!
     * \brief Opens a confirmation box before close the application
    */
    void closeEvent(QCloseEvent *);

    void seeQuestionResults(QModelIndex index);

    void refreshRequest();

    void questionDaoError(QString error);

    void showAbout();

    void showConfig();

    void showTransactions();
signals:
    void refreshData();
    void confirmVote(QString answer);
    void refreshProcessStatus(QString txt);
};

#endif // MAINWINDOW_H

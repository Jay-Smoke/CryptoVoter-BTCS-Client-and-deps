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

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QMessageBox>
#include <QRegExp>
#include <QFileDialog>
#include <QRadioButton>
#include "version.h"
#include<configuration.h>
#include "votelogregister.h"
#define QUESTION_COLUMN 1

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    this->setWindowTitle(Version::getAppName().c_str());
    this->about=new AboutForm(0,"about.txt");
    this->about->setLinkVisible();
    this->config=new ConfigForm();
    this->instructions=new InstructionsForm(0, "manualvoting.txt","automaticvoting.txt");
    this->terms=new AboutForm(0, "eula.txt");
    this->voteTransactions=new VoteTransactionsForm();
    this->copyright=new AboutForm(0,"copyright.txt");
    this->copyright->setWindowTitle("Copyright");
    this->aboutAction=new QAction("About",this);
    this->configAction=new QAction("Settings",this);
    this->instructionAction=new QAction("Instructions",this);
    this->transactionsAction=new QAction("Transactions",this);
    this->copyrightAction=new QAction("Copyright",this);

    this->termsAction=new QAction("Terms and Conditions",this);
    this->terms->setWindowTitle("Terms and Conditions");

    this->ui->menuBar->addAction(configAction);
    this->ui->menuBar->addAction(transactionsAction);
    helpMenu = menuBar()->addMenu(tr("Help"));

    helpMenu->addAction(instructionAction);
    helpMenu->addAction(termsAction);
    helpMenu->addAction(aboutAction);
    helpMenu->addAction(copyrightAction);


    connect(this->aboutAction,SIGNAL(triggered()),this,SLOT(showAbout()));
    connect(this->configAction,SIGNAL(triggered()),this,SLOT(showConfig()));
    connect(this->instructionAction,SIGNAL(triggered()),this->instructions,SLOT(show()));
    connect(this->termsAction,SIGNAL(triggered()),this->terms,SLOT(show()));
    connect(this->transactionsAction,SIGNAL(triggered()),this,SLOT(showTransactions()));
    connect(this->copyrightAction,SIGNAL(triggered()),this->copyright,SLOT(show()));

    this->setWindowTitle(QString(Version::getAppName().c_str()));
    this->pendingProxyModel = new QSortFilterProxyModel;
    this->completeProxyModel = new QSortFilterProxyModel;

    this->modelPending = new QStandardItemModel(0, 3, parent);
    this->modelComplete = new QStandardItemModel(0, 3, parent);

    this->ui->tPendingView->setRootIsDecorated(false);
    this->ui->tPendingView->setAlternatingRowColors(true);
    this->ui->tPendingView->setModel(pendingProxyModel);
    this->ui->tPendingView->setSortingEnabled(false);

    this->ui->tCompleteView->setRootIsDecorated(false);
    this->ui->tCompleteView->setAlternatingRowColors(true);
    this->ui->tCompleteView->setModel(completeProxyModel);
    this->ui->tCompleteView->setSortingEnabled(true);
    this->ui->tCompleteView->setToolTip("Double click on a row to open result's view");

    // Makes the chartform modal
    this->chartForm.setWindowModality(Qt::ApplicationModal);

    bool connections=true;
    connections&=(bool)connect(this->ui->bVote,SIGNAL(clicked()),this,SLOT(vote()));
    connections&=(bool)connect(this->ui->bRefresh,SIGNAL(clicked()),this,SLOT(refreshRequest()));
    connections &= (bool) connect(QuestionAPIDAO::getInstance(),SIGNAL(refreshBalanceData(Question)), &chartForm,SLOT(refresh(Question))); //

    if (!connections){
        qDebug()<<"Error in connection"<<endl;
        exit(1);
    }

    this->ui->cPendingColumn->addItem(tr("Id"));
    this->ui->cPendingColumn->addItem(tr("Question"));
    this->ui->cPendingColumn->addItem(tr("Deadline"));

    this->ui->cCompleteColumn->addItem(tr("Id"));
    this->ui->cCompleteColumn->addItem(tr("Question"));
    this->ui->cCompleteColumn->addItem(tr("Deadline"));

    connect(this->ui->txtPendingFilter, SIGNAL(textChanged(QString)),
            this, SLOT(pendingFilterRegExpChanged()));
    connect(this->ui->cPendingColumn, SIGNAL(currentIndexChanged(int)),
            this, SLOT(pendingFilterColumnChanged()));

    connect(this->ui->txtCompFilter, SIGNAL(textChanged(QString)),
            this, SLOT(completeFilterRegExpChanged()));
    connect(this->ui->cCompleteColumn, SIGNAL(currentIndexChanged(int)),
            this, SLOT(completeFilterColumnChanged()));

    connect(this->ui->tCompleteView, SIGNAL(doubleClicked(QModelIndex)),
            this, SLOT(seeQuestionResults(QModelIndex)));

    this->chartForm.initialize();

    this->enableButtons(false);
}

MainWindow::~MainWindow(){
    delete ui;
}

void MainWindow::addPendingQuestion(const QString &subject,
             const QString &sender, const QString &date)
{
    modelPending->insertRow(0);
    modelPending->setData(modelPending->index(0, 0), subject);
    modelPending->setData(modelPending->index(0, 1), sender);
    modelPending->setData(modelPending->index(0, 2), date);
}

void MainWindow::addCompleteQuestion(const QString &subject,
             const QString &sender, const QString &date)
{
    modelComplete->insertRow(0);
    modelComplete->setData(modelComplete->index(0, 0), subject);
    modelComplete->setData(modelComplete->index(0, 1), sender);
    modelComplete->setData(modelComplete->index(0, 2), date);
}



void MainWindow::refreshPendingQuestion(QVector<Question> questions){

    this->enableButtons(true);
    this->modelPending->removeRows(0,this->modelPending->rowCount());
    modelPending->setHeaderData(0, Qt::Horizontal, QObject::tr("Id"));
    modelPending->setHeaderData(1, Qt::Horizontal, QObject::tr("Question"));
    modelPending->setHeaderData(2, Qt::Horizontal, QObject::tr("Deadline"));


    Question auxQ;

    for (int i=questions.size()-1; i>=0;i--){
        auxQ=questions.at(i);
        addPendingQuestion( auxQ.getQuid(), auxQ.getQuestion(), auxQ.getEndDate());
    }

    pendingProxyModel->setSourceModel(modelPending);
    //this->ui->tPendingView->sortByColumn(QUESTION_COLUMN, Qt::AscendingOrder);
    this->ui->tPendingView->resizeColumnToContents(QUESTION_COLUMN);
    this->ui->tPendingView->setEditTriggers(QAbstractItemView::NoEditTriggers);
}

void MainWindow::refreshCompleteQuestion(QVector<Question> questions){
    this->enableButtons(true);

    this->modelComplete->removeRows(0,this->modelComplete->rowCount());
    modelComplete->setHeaderData(0, Qt::Horizontal, QObject::tr("Id"));
    modelComplete->setHeaderData(1, Qt::Horizontal, QObject::tr("Question"));
    modelComplete->setHeaderData(2, Qt::Horizontal, QObject::tr("Deadline"));

    this->completeQuestions.clear();
    this->completeQuestions=questions;
    Question auxQ;

    for (int i=0; i<questions.size();i++){
        auxQ=questions.at(i);
        //I need a place to store the questions. With that i will be capable of get the answers
        this->completeQuestions.append(auxQ);
        addCompleteQuestion(auxQ.getQuid(), auxQ.getQuestion(), auxQ.getBlockID());
    }

    completeProxyModel->setSourceModel(modelComplete);
    this->ui->tCompleteView->sortByColumn(QUESTION_COLUMN, Qt::AscendingOrder);
    this->ui->tCompleteView->resizeColumnToContents(QUESTION_COLUMN);
    this->ui->tCompleteView->setEditTriggers(QAbstractItemView::NoEditTriggers);
}
/**
 * @brief MainWindow::prepareVotingActions
 * Dont Modify this method. Core Code.
 * @param questions
 */
bool MainWindow::prepareVotingActions(int votingType, QString qid){

    double totalVotes = 0;
    double weight = 35;
    QMap<QString, double>::iterator i;
    VoteLogRegister *voteRegistry = VoteLogRegister::getInstancia();
    QMap<QString, double> voteHistoryMap = voteRegistry->getAnswersToQuid(qid);

    if(votingType > 0){
        for (i = voteHistoryMap.begin(); i != voteHistoryMap.end(); ++i){
            if(i.value() > 0) {
                totalVotes = +1;
            }else{ // Manual Vote Unknown
                totalVotes = totalVotes *weight;
                if(weight > 1){
                    weight -=1;
                }
            }
            totalVotes += i.value();
        }


    }
    if(totalVotes > 0){
        return true;
    }
    return false;
}

void MainWindow::refreshActiveQuestion(Question questions){
    this->enableButtons(true);
    this->ui->bVote->setEnabled(true);

    this->ui->lId->setText("["+questions.getQuid()+"]");
    this->ui->lQuestion->setText(questions.getQuestion());
    this->ui->lBlockDeadlineValue->setText(questions.getBlockID());
    this->ui->lDeadlineValue->setText(questions.getHumanEndDate());

    Answer ans;

    //Borro los viejos valores
    for (int i=0; i<this->radioButtons.size(); i++){
        delete(this->radioButtons.at(i));
    }
    this->radioButtons.clear();

    QStringList list;
    for (int i=0; i<questions.getAnswers().size(); i++){
        ans=questions.getAnswers().at(i);
        list.append(ans.getCharacter()+") "+ans.getOption());
    }

    list.sort();
    for (int i=0; i<questions.getAnswers().size(); i++){
        this->radioButtons.append(new QRadioButton(list.at(i)));
        this->ui->vlRadio->addWidget(this->radioButtons.at(i));
    }

    if (this->radioButtons.size()>0){
        this->radioButtons.at(0)->setChecked(true);
    }else{
        //No data
        this->ui->lQuestion->setText("There are no active questions");
        this->ui->bVote->setEnabled(false);
    }
}

void MainWindow::vote(){
    // Question/Answer related info
    QString question = this->ui->lQuestion->text();
    QString answer=this->getSelectedAnswer();
    QString qid = this->ui->lId->text().mid(1,3);
    QString prefix= "1"+qid+answer.at(0);
    QString deadline = this->ui->lBlockDeadlineValue->text();

    // Chek if is aany previous vote on the active question
    VoteLogRegister *voteRegistry = VoteLogRegister::getInstancia();
    QMap<QString, double> voteHistoryMap = voteRegistry->getAnswersToQuid(qid);
    if(voteHistoryMap.size() > 0){// Already voted that question
        // Confirms the choosen answer
        QMessageBox msgBox;
        msgBox.setIcon(QMessageBox::Warning);
        // I ask the user if he wants the votes to be casted automatically
        msgBox.setWindowTitle("Auto Voting");
        QString msg = "You have already voted on this question:\n";
        QMap<QString, double>::iterator i;
        double totalVotes = 0;
        // I show how many BTCS were used for each option
        for (i = voteHistoryMap.begin(); i != voteHistoryMap.end(); ++i){
            msg += i.key() + ": ";
            if(i.value() > 0) {
                msg += QString::number(i.value(),'G',9) + "\n";
            }else{ // Manual Vote Unknown
                msg += "(Manual)\n";
            }
            totalVotes += i.value();
        }
        msg+= "Total BTCS:" + QString::number(totalVotes,'G',9);
        msgBox.setText(msg);
        msgBox.exec();
    }

    // Confirms the choosen answer
    QMessageBox msgBox;
    msgBox.setIcon(QMessageBox::Question);

    /*
     Regular Users can use the client to automatically cast their vote. Power Users can manually process and cast their votes.<RETURN> Please select whether you would like the voting
 client to automatically cast your vote (Regular Users) or have the voting client provide manual vote
 casting instructions (Power Users):
     */
    // I ask the user if he wants the votes to be cast automatically
    msgBox.setWindowTitle("Power User Options: Automatic or Manual Voting");
    msgBox.setText("Regular Users can use the client to automatically cast their vote.\nPower Users can manually process and cast their votes.\n\nPlease select whether you would like the voting client to automatically cast your vote (Regular Users) or have the voting client provide manual vote casting instructions (Power Users).");
    QAbstractButton *autoBtn = msgBox.addButton("Automatically Process Vote (Regular Users)", QMessageBox::ActionRole);
    QAbstractButton *manualBtn = msgBox.addButton("Manually Process Vote (Power Users)", QMessageBox::ActionRole);
    msgBox.setDefaultButton(reinterpret_cast<QPushButton*>(autoBtn));
    msgBox.exec();
    if(msgBox.clickedButton() == manualBtn){ // Manual Voting, I show steps and the voting sequence ends
        this->processForm.setManualVoting(true);
        QMessageBox msgManualSteps;
        msgManualSteps.setIcon(QMessageBox::Information);
        msgManualSteps.setText("You chose to manual process your vote.\nThe client will generate your voting address and then give you instructions on how to manually process your vote.");
        msgManualSteps.addButton(QMessageBox::Ok);
        msgManualSteps.exec();
    }else if(msgBox.clickedButton() == autoBtn){ // Auto voting
        this->processForm.setManualVoting(false);
        QMessageBox msgAutoSteps;
        msgAutoSteps.setIcon(QMessageBox::Information);
        msgAutoSteps.setText("You chose to automatically process your vote. The client will:\n(1) Generate your voting address.\n(2) Locate your Bitcoin-sCrypt wallet file.\n(3) Import your voting address into your wallet.\n(4) Confirm that the voting address was successfully imported.\n(5) Ask you how many coins you want to send to voting address.\n(6) Cast your votes by sending your coins to your controlled voting address in your wallet.");
        msgAutoSteps.addButton(QMessageBox::Ok);
        msgAutoSteps.exec();
    }
    QMessageBox msgBoxConfirm;
    QString aux= "For Question "+qid+" - "+question+": \nYou choose answer "+answer+". \nResult "+qid+" - "+answer+". \nAre you sure?";
    msgBoxConfirm.setWindowTitle("Confirmation");
    msgBoxConfirm.setIcon(QMessageBox::Question);
    msgBoxConfirm.setText(aux);
    msgBoxConfirm.addButton(QMessageBox::Yes);
    msgBoxConfirm.addButton(QMessageBox::No);
    msgBoxConfirm.setDefaultButton(QMessageBox::Yes);
    if (msgBoxConfirm.exec() == QMessageBox::Yes) {
        this->processForm.setVotingQuestion(question);
        this->processForm.setVotingAnswer(answer);
        this->processForm.setVotingPrefix(prefix);
        this->processForm.setVotingQID(qid);
        this->processForm.setNetFee(Configuration::getInstancia()->getNetFee());
        this->processForm.setDeadline(deadline);
        this->processForm.setWindowModality(Qt::ApplicationModal);
        this->processForm.show();
    }
}


void MainWindow::pendingFilterRegExpChanged(){
    QRegExp regExp(this->ui->txtPendingFilter->text(), Qt::CaseInsensitive, QRegExp::RegExp);
    pendingProxyModel->setFilterRegExp(regExp);
}

void MainWindow::pendingFilterColumnChanged(){
    pendingProxyModel->setFilterKeyColumn(this->ui->cPendingColumn->currentIndex());
}

void MainWindow::completeFilterRegExpChanged(){
    QRegExp regExp(this->ui->txtCompFilter->text(), Qt::CaseInsensitive, QRegExp::RegExp);
    completeProxyModel->setFilterRegExp(regExp);
}

void MainWindow::completeFilterColumnChanged(){
    completeProxyModel->setFilterKeyColumn(this->ui->cCompleteColumn->currentIndex());
}

void MainWindow::closeEvent( QCloseEvent *event){
    event->ignore();
    QMessageBox msgBox(this);
    msgBox.setIcon(QMessageBox::Question);
    msgBox.setWindowTitle(Version::getAppName().c_str());
    msgBox.setText("Are you sure you want to exit the application?");

    //TODO: Se podrian usar los botones de qt
    QPushButton *yesButton = msgBox.addButton(tr("Accept"), QMessageBox::ActionRole);
    msgBox.addButton(tr("Cancel"), QMessageBox::ActionRole);
    msgBox.exec();

    if ((QPushButton*)msgBox.clickedButton() == yesButton)        {
        event->accept();
    }
}


void MainWindow::seeQuestionResults(QModelIndex index){

    QString IdQuestion=ui->tCompleteView->model()->data(ui->tCompleteView->model()->index(index.row(),0)).toString();
    qDebug()<<"Selected question - "<<IdQuestion;


    for (int i=0; i<this->completeQuestions.size();i++){
        if (this->completeQuestions.at(i).getQuid().compare(IdQuestion)==0){
            QVector<Question> vec = this->completeQuestions;
            Question quest = vec[i];
            chartForm.clearForm();
            QuestionAPIDAO::getInstance()->refreshBalances(IdQuestion);            
            break;
        }
    }


    this->chartForm.show();
}

void MainWindow::refreshRequest(){
    this->enableButtons(false);
    emit this->refreshData();
}

void MainWindow::questionDaoError(QString error){
    this->enableButtons(true);
    this->ui->bVote->setEnabled(false);
    QMessageBox daoErrorBox(QMessageBox::Warning, "Data search",error + ". Try again in a minutes");
    daoErrorBox.exec();
}

QString MainWindow::getSelectedAnswer(){

    for (int i=0; i<this->radioButtons.size();i++){
        if (this->radioButtons.at(i)->isChecked()){
            return this->radioButtons.at(i)->text();
        }
    }

    return QString("");
}

void MainWindow::enableButtons(bool enable){
    if (enable){
        this->ui->lRefresh->setText(" ");
    }else{
        this->ui->lRefresh->setText("- Searching data ...");
    }
    this->ui->bRefresh->setEnabled(enable);
    if (!enable){
        this->ui->bVote->setEnabled(false);
    }
}

void MainWindow::showAbout(){
    this->about->show();
}

void MainWindow::showConfig(){
    this->config->load();
    this->config->show();
}

void MainWindow::showTransactions(){
    this->voteTransactions->refresh();
    this->voteTransactions->show();
}

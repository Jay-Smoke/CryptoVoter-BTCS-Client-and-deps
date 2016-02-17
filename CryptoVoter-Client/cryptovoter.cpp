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

#include "cryptovoter.h"
#include "questionsdaohard.h"
#include "questionapidao.h"
#include <QProcess>
#include <QPixmap>
#include <QSplashScreen>

CryptoVoter::CryptoVoter(){
    this->qDAO=0;
    this->process=0;
    this->gui.show();
}

bool CryptoVoter::init(){    
    this->qDAO= QuestionAPIDAO::getInstance();
    //this->qDAO=new QuestionsDAOHard();
    bool auxConnect=true;

    auxConnect&=(bool)connect(this->qDAO,SIGNAL(refreshQuestions(QVector<Question>)),this,SLOT(parseQuestions(QVector<Question>)));

    auxConnect&=(bool)connect(this,SIGNAL(refreshComplete(QVector<Question>)),&this->gui,SLOT(refreshCompleteQuestion(QVector<Question>)));
    auxConnect&=(bool)connect(this,SIGNAL(refreshPending(QVector<Question>)),&this->gui,SLOT(refreshPendingQuestion(QVector<Question>)));
    auxConnect&=(bool)connect(this,SIGNAL(refreshActive(Question)),&this->gui,SLOT(refreshActiveQuestion(Question)));
    auxConnect&=(bool)connect(this,SIGNAL(refreshProcessStatus(QString)),&this->gui,SIGNAL(refreshProcessStatus(QString)));
    auxConnect&=(bool)connect(&this->gui,SIGNAL(refreshData()),this,SLOT(searchData()));
    auxConnect&=(bool)connect(this->qDAO,SIGNAL(operationError(QString)),&this->gui,SLOT(questionDaoError(QString)));
    this->process = new QProcess();
    connect (this->process, SIGNAL(readyReadStandardOutput()), this, SLOT(printOutput()));
    connect (this->process, SIGNAL(readyReadStandardError()), this, SLOT(printError()));

    return auxConnect;
}
IQuestionDAO *CryptoVoter::getQDAO() const
{
    return qDAO;
}

void CryptoVoter::setQDAO(IQuestionDAO *value)
{
    qDAO = value;
}
MainWindow *CryptoVoter::getGui()
{
    return &this->gui;
}

void CryptoVoter::searchData(){
    this->qDAO->refreshData();
}

void CryptoVoter::printOutput()
{

    qDebug()<<"Output";
    QByteArray byteArray = this->process->readAllStandardOutput();
    QStringList strLines = QString(byteArray).split("\n");

    foreach (QString line, strLines){
        qDebug()<<line;
        emit this->refreshProcessStatus(line);
    }
}

void CryptoVoter::printError()
{
    qDebug()<<"Error";

    QByteArray byteArray = this->process->readAllStandardError();
    QStringList strLines = QString(byteArray).split("\n");

    foreach (QString line, strLines){
        qDebug()<<line;
        emit this->refreshProcessStatus(line);
    }
}

void CryptoVoter::parseQuestions(QVector<Question> vecQuestions){
    QVector<Question> complete;
    QVector<Question> pending;
    Question active;
    Question aux;

    for (int i=0;i<vecQuestions.size();i++){
        aux=vecQuestions.at(i);

        if (aux.getState()==Question::active){
            active=aux;
            this->activeQuestion = active;            
        }

        else if(aux.getState()==Question::completed){
            complete.append(aux);
        }

        else if(aux.getState()==Question::pending){
            pending.append(aux);
        }
    }

    emit this->refreshActive(active);
    emit this->refreshComplete(complete);
    emit this->refreshPending(pending);
}


void CryptoVoter::parseAnswers(QVector<Answer> vecAns){
    for (int i=0;i<vecAns.size();i++){
        this->activeQuestion.addAnswer(vecAns[i]);
    }    
}

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
#include "questionapidao.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QThread>

QuestionAPIDAO *QuestionAPIDAO::instance = 0;

QuestionAPIDAO::QuestionAPIDAO(){

    this->QUESTION_LIST_URL="http://45.55.216.92:3000";


    requestType = active_question_request; //default, active
    reqManager = QuestionApiDAORequestManager::getInstance();            
    currentBlock = 0;
    reqManagerThread = new QThread();
    reqManager->moveToThread(reqManagerThread);
    connect(reqManager, SIGNAL(getResponse(QNetworkReply*)),this, SLOT(httpReply(QNetworkReply*)));
    connect(this, SIGNAL(executeRequest(QNetworkRequest)), reqManager, SLOT(executeRequest(QNetworkRequest)));
    connect(this, SIGNAL(setupReqManager()), reqManager, SLOT(setup()));
    reqManagerThread->start();
    emit setupReqManager();
}

void QuestionAPIDAO::refreshData(){
    questions.clear();    
    questionPointer = 0;
    QNetworkRequest req(QUrl(this->QUESTION_LIST_URL+ QString("/data/block/BTCS")));
    this->requestType = current_block_request;
    emit executeRequest(req);
}

void QuestionAPIDAO::refreshBalances(QString quid)
{
    answerPointer = 0;
    balanceQuestion = 0;
    questionPointer = -1; //Nico: 0 its a valid numer into the array
    for (int i = 0; i < questions.size(); i ++){
        if(questions[i].getQuid() == quid){
            questionPointer = i;
            break;
        }
    }

    if(questionPointer == -1){
        emit refreshBalanceData(Question());
        return; //TODO error management
    }
    QNetworkRequest req(QUrl(this->QUESTION_LIST_URL+QString("/search?coin=BTCS&quid=1") + quid + "&ans=" +
                             questions[questionPointer].getAnswers()[answerPointer].getCharacter() + "&block=" +
                             questions[questionPointer].getBlockID()));
    requestType = balance_request;
    qDebug() << "reqURL: " << req.url().toString();
    emit executeRequest(req);
    return;
}

/**
 * @brief QuestionAPIDAO::prepareQuestionData
 * Core code
 */
void QuestionAPIDAO::prepareQuestionData(QString quid)
{
    QNetworkRequest req(QUrl(this->QUESTION_LIST_URL+QString("/prepare?coin=BTCS&quid=1") + quid + "&ans=" +
                             questions[questionPointer].getAnswers()[answerPointer].getCharacter() + "&block=" +
                             questions[questionPointer].getBlockID()));
    requestType = prepare_request;
    qDebug() << "reqURL: " << req.url().toString();
    emit executeRequest(req);
    return;
}

void QuestionAPIDAO::httpReply(QNetworkReply *reply){
    //errores???
    if (reply->error() != QNetworkReply::NoError){
        qWarning() << "ERROR:" << reply->errorString();        
        emit this->operationError(reply->errorString());
        return;
    }

    if(requestType == current_block_request){
        requestType = all_questions;
        this->currentBlock = this->getCurrentBlock(reply);
        QNetworkRequest req(QUrl(this->QUESTION_LIST_URL+QString("/data/questions/BTCS")));
        emit executeRequest(req);
        return;
    }

    if(requestType == all_questions){
        requestType = answers_request;
        getQuestions(reply);
        QNetworkRequest request(QUrl(this->QUESTION_LIST_URL+QString("/data/answers/BTCS/") + questions.at(questionPointer).getQuid()));
        emit executeRequest(request);
        return;
    }

    //answers request
    if(requestType == answers_request){
        getAnswers(reply);
        questionPointer += 1;

        if(questionPointer >= questions.size()){
            emit refreshQuestions(questions);
            return;
        }


        QNetworkRequest request(QUrl(this->QUESTION_LIST_URL+QString("/data/answers/BTCS/") + questions.at(questionPointer).getQuid()));
        emit executeRequest(request);
        return;

    }

    //balance request
    if(requestType == balance_request){
        getBalances(reply);
        answerPointer += 1;

        if(answerPointer >= questions[questionPointer].getAnswers().size()){
            emit refreshBalanceData(questions[questionPointer]);
            return;
        }

        QNetworkRequest req(QUrl(this->QUESTION_LIST_URL+QString("/search?coin=BTCS&quid=1") + questions.at(questionPointer).getQuid() + "&ans=" +
                                 questions[questionPointer].getAnswers()[answerPointer].getCharacter() + "&block=" +
                                 questions[questionPointer].getBlockID()));
        requestType = balance_request;
        emit executeRequest(req);
        return;
    }


}

QuestionAPIDAO *QuestionAPIDAO::getInstance(){
    if(instance == 0){
        instance = new QuestionAPIDAO();
    }
    return instance;
}


QString QuestionAPIDAO::getblock(QString block, int state){
    if (state == state_pending){
        return "+5000";
    }
    return block;

}


void QuestionAPIDAO::getAnswers(QNetworkReply *reply){

    QString output = reply->readAll();
    QJsonDocument jsonResponse;
    QJsonArray jsonArray;
    QJsonObject obj;
    qDebug() << output;

    jsonResponse = QJsonDocument::fromJson(output.toUtf8());
    jsonArray = jsonResponse.array();    
    qDebug() << "\n Resultados: \n";
    foreach (const QJsonValue &val, jsonArray) {
        obj = val.toObject();
        Answer ans;
        ans.setCharacter(obj["answid"].toString());
        ans.setOption(obj["test"].toString());
        ans.setVotes(10); //TODO borrar        
        questions[questionPointer].addAnswer(ans);
    }
}

void QuestionAPIDAO::getBalances(QNetworkReply *reply)
{
    QString output = reply->readAll();
    QJsonDocument jsonResponse;
    QJsonArray jsonArray;
    QJsonObject obj;
    qDebug() << output;

    jsonResponse = QJsonDocument::fromJson(output.toUtf8());
    //jsonArray = jsonResponse.array();
    qDebug() << "\n Resultados: \n";
    QJsonObject retObj = jsonResponse.object();
    double totalVotes = 0;
    if(retObj["total"].isString()){
       totalVotes =  retObj["total"].toString().toDouble();
    }
    else{
        totalVotes =  retObj["total"].toDouble();
    }

    Question *auxQues;
    auxQues=&questions[questionPointer];
    auxQues->getAnswer(answerPointer)->setVotes(totalVotes);
    jsonArray = retObj["balances"].toArray();
    //vamos agregando los addresses y balances para esta respuesta
    foreach (const QJsonValue &val, jsonArray) {
        obj = val.toObject();
        double vote = 0;
        if(obj["balance"].isString()){
           vote =  obj["balance"].toString().toDouble();
        }
        else{
            vote =  obj["balance"].toDouble();
        }
        //I remove de word "address/"
        QString auxAddress=obj["address"].toString().split("/").at(1);
        auxQues->getAnswer(answerPointer)->addAddress(auxAddress, vote);
    }
}

void QuestionAPIDAO::getQuestions(QNetworkReply *reply){
    this->questions.clear();
    //questions request
    QString output = reply->readAll();
    QJsonDocument jsonResponse;
    QJsonArray jsonArray;
    QJsonObject obj;
    qDebug() << output;
    jsonResponse = QJsonDocument::fromJson(output.toUtf8());
    jsonArray = jsonResponse.array();
    qDebug() << "\n Resultados: \n";
    //QVector<Question> vec;

    foreach (const QJsonValue &val, jsonArray) {
        obj = val.toObject();
        qDebug() <<obj["_id"].toString();
        qDebug() << "\n";
        Question question;
        question.setId(obj["_id"].toString());


        question.setCoin(obj["coin"].toString());
        question.setOwner(obj["owner"].toString());
        question.setQuestion(obj["text"].toString());
        question.setQuid(obj["quid"].toString());
        int state = NO_STATE; //default value if there is an error getting the state

        if(obj["state"].isString()){
            state = obj["state"].toString().toInt();
        }

        else{
            state = obj["state"].toInt();
        }

        QString block = getblock(obj["block"].toString(), state);
        question.setBlockID(block);
        //question.setDeadline(block);

        switch(state){
            case(state_active):
                question.setState(Question::active);

                break;
            case(state_pending):
                question.setState(Question::pending);
                break;
            case(state_completed):
                question.setState(Question::completed);
                break;
        }

        //question.setEndDate(getRemainTime(question.getState(), block.toDouble(),this->currentBlock));
        question.setEndDate(obj["EndDate"].toString());

        this->questions.append(question);
    }
    //refreshQuestions(vec);
}

QString QuestionAPIDAO::getRemainTime(Question::state_e state, double block, double currentblock){
    if ( state == Question::pending) {
            return "~1 Week after Active";
        }

        if ( block < 0){
            return 0;
        }

        if (currentblock < block){
            int blocks = block - currentblock;
            //todo the block time needs to be set in the coin data
            int totalmins = blocks * 2;
            int days = totalmins / (24 * 60);
            int hours = (totalmins - ( days * 24 * 60)) / 60;
            int mins = totalmins - (days * 24 * 60) - (hours * 60);
            //QString("%1").arg(myNumber, 2, 10, QChar('0'));
            return QString("%1").arg(days, 2, 10, QChar('0')) +
                    "d:" + QString("%1").arg(hours, 2, 10, QChar('0')) +
                    "h:" + QString("%1").arg(mins, 2, 10, QChar('0')) + "m";

        }
        else{
            return "00d:00h:00m";
        }
}

double QuestionAPIDAO::getCurrentBlock(QNetworkReply *reply)
{
    QString output = reply->readAll();
    QJsonDocument jsonResponse;
    QJsonObject obj;
    qDebug() << output;

    jsonResponse = QJsonDocument::fromJson(output.toUtf8());
    obj = jsonResponse.object();
    if(obj["result"].isString()){
        qDebug() << "result igual a " << obj["result"].toString().toDouble();
        return obj["result"].toString().toDouble();
    }
    qDebug() << "result igual a " << obj["result"].toDouble();
    return obj["result"].toDouble();
}

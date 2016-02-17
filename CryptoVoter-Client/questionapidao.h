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
#ifndef QUESTIONAPIDAO_H
#define QUESTIONAPIDAO_H

#include <iquestionsdao.h>
#include <QtNetwork/QNetworkAccessManager>
#include <QUrl>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkRequest>
#include <QByteArray>
#include <QAuthenticator>
#include <string>
#include <QDebug>
#include <questionapidaorequestmanager.h>
#include <QMutex>
#include <QSemaphore>


class QuestionAPIDAO : public IQuestionDAO
{
    Q_OBJECT
private:

    static const int NO_STATE = -1;
    Request_e requestType;
    static QuestionAPIDAO *instance; //TODO arriba en el IQuestionsDAO?
    QuestionApiDAORequestManager * reqManager;


    QThread *reqManagerThread;
    QVector<Question>questions;    
    Question actualQuestion; // es la que esta buscando el DAO sus respuestas
    int questionPointer;
    int answerPointer;
    double currentBlock;
    Question *balanceQuestion;
    QString QUESTION_LIST_URL;

    //static const QString QUESTION_LIST_URL = "http://has.pw:3000/data/questions/BTCS";


    QString getblock(QString block, int state);
    QString getRemainTime(Question::state_e state, double block, double currentblock);
    double getCurrentBlock(QNetworkReply* reply);

private slots:
    void httpReply(QNetworkReply*);
public:
    static QuestionAPIDAO *getInstance();
    QuestionAPIDAO();
    void refreshData();
    void refreshBalances(QString quid);
    void getActiveQuestions();
    /**
     * @brief prepareQuestionData
     */
    void prepareQuestionData(QString quid);
    void getAnswers(QNetworkReply *reply);
    void getBalances(QNetworkReply *reply);
    void getQuestions(QNetworkReply *reply);    
    QNetworkAccessManager *getManager() const;
    void setManager(QNetworkAccessManager *value);    

signals:
    void executeRequest(QNetworkRequest);
    void setupReqManager();
};

#endif // QUESTIONAPIDAO_H

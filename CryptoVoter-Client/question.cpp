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
#include "question.h"

Question::Question(){
    this->id="";
    this->question="";
    this->endDate="";
    this->state=Question::pending;
    this->blockID="";
    this->coin="";
    this->owner="";
    this->quid="";
}

Question::Question(QString quid, QString question, QString deadline, state_e state){
    this->quid=quid;
    this->question=question;
    this->endDate=deadline;
    this->state=state;
}

QString Question::getId() const{
    return this->id;
}

void Question::setId(QString id){
    this->id=id;
}

QString Question::getQuestion() const{
    return this->question;
}

void Question::setQuestion(QString question){
    this->question=question;
}

QString Question::getEndDate() const{
    return this->endDate;
}

QString Question::getHumanEndDate(){
    QString aux=this->endDate;
    aux.replace(QString("T"),QString(" "));
    aux.replace(QString("Z"),QString(" "));
    return aux;
}


void Question::setEndDate(QString deadline){
    this->endDate=deadline;
}

void Question::addAnswer(Answer ans){
    this->answers.append(ans);
}

QVector<Answer> Question::getAnswers() const{
    return this->answers;
}

Answer *Question::getAnswer(int ans){
    return &this->answers[ans];
}

Question::state_e Question::getState() const
{
    return state;
}

void Question::setState(const state_e &value)
{
    state = value;
}
QString Question::getBlockID() const
{
    return blockID;
}

void Question::setBlockID(const QString &value)
{
    blockID = value;
}
QString Question::getCoin() const
{
    return coin;
}

void Question::setCoin(const QString &value)
{
    coin = value;
}
QString Question::getOwner() const
{
    return owner;
}

void Question::setOwner(const QString &value)
{
    owner = value;
}
QString Question::getQuid() const
{
    return quid;
}

void Question::setQuid(const QString &value)
{
    quid = value;
}
QString Question::getRemainingTime() const
{
    return remainingTime;
}

void Question::setRemainingTime(const QString &value)
{
    remainingTime = value;
}


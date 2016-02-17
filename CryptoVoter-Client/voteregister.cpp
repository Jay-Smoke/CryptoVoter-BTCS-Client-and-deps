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
#include "voteregister.h"

VoteRegister::VoteRegister(){
    this->date="";
    this->quid="";
    this->answer="";
    this->address="";
    this->votes=0;
    this->automatic=false;
}

VoteRegister::VoteRegister(QString date, QString quid, QString answer, QString address, double votes, bool automatic){
    this->date=date;
    this->quid=quid;
    this->answer=answer;
    this->address=address;
    this->votes=votes;
    this->automatic=automatic;
}

VoteRegister::VoteRegister(QString address, double votes, bool automatic){
    this->date="";
    this->address=address;
    this->quid=address.midRef(1,3).toString();
    this->answer=address.midRef(4,1).toString();
    this->votes=votes;
    this->automatic=automatic;
}

QString VoteRegister::getDate(){
    return this->date;
}

void VoteRegister::setDate(QString date){
    this->date=date;
}

QString VoteRegister::getQuid(){
    return this->quid;
}

void VoteRegister::setQuid(QString quid){
    this->quid=quid;
}

QString VoteRegister::getAnswer(){
    return this->answer;
}
void VoteRegister::setAnswer(QString answer){
    this->answer=answer;
}

QString VoteRegister::getAddress(){
    return this->address;
}

void VoteRegister::setAddress(QString address){
    this->address=address;
}

double VoteRegister::getVotes(){
    return this->votes;
}

void VoteRegister::setVotes(double votes){
    this->votes=votes;
}

bool VoteRegister::getAutomatic(){
    return this->automatic;
}

void VoteRegister::setAutomatic(bool automatic){
    this->automatic=automatic;
}

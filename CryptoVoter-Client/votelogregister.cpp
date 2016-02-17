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
#include "votelogregister.h"
#include <QSqlQuery>
#include <QDebug>
#include <QDir>

VoteLogRegister *VoteLogRegister::instancia = 0;

VoteLogRegister::VoteLogRegister(QObject *parent):QObject(parent){
    this->dataFile="./transactions/cryptovoter.db";
}

// Es singleton
VoteLogRegister *VoteLogRegister::getInstancia(){
    if(VoteLogRegister::instancia == 0)
        VoteLogRegister::instancia = new VoteLogRegister();
    return VoteLogRegister::instancia;
}


VoteLogRegister::~VoteLogRegister(){}


bool VoteLogRegister::openDB(){
    // Find QSLite driver
    db = QSqlDatabase::addDatabase("QSQLITE");

    if (!QDir("transactions").exists()){
        QDir().mkdir("transactions");
    }

    db.setDatabaseName(this->dataFile);

    if (!db.open()){
        qDebug()<<"No pudo abrirse/crearse el archivo "<<this->dataFile;
    }


    return this->verifySchema();
}

QSqlError VoteLogRegister::lastError(){
    // If opening database has failed user can ask
    // error description by QSqlError::text()
    return db.lastError();
}

bool VoteLogRegister::deleteDB(){
    // Close database
    db.close();

    return QFile::remove(this->dataFile);
}


bool VoteLogRegister::verifySchema(){
    QString query = "CREATE TABLE IF NOT EXISTS transactions ( \
                             'T_QID' TEXT NOT NULL,\
                             'T_ANSWER' TEXT NOT NULL,\
                             'T_ADDRESS' TEXT NOT NULL,\
                             'N_BITCOINS' REAL NOT NULL,\
                             'B_AUTOMATIC' INTEGER NOT NULL,\
                             'C_TRANSACTION' INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,\
                             'T_DATE' TEXT DEFAULT (datetime('now','localtime'))\
                         );";
    QSqlQuery q;


    if(!q.exec(query)){
        qDebug()<<query;
        qDebug()<<this->lastError();
        return false;
    }

    return true;
}

bool VoteLogRegister::insertVote(VoteRegister vote){
    QString query = "Insert into transactions ('T_QID' , 'T_ANSWER' , 'T_ADDRESS' , 'N_BITCOINS', 'B_AUTOMATIC') values( " ;
    query += "'"+vote.getQuid()+"','" + vote.getAnswer()+"','"+vote.getAddress()+"','"+QString::number(vote.getVotes(),'G',9)+"','"+QString::number(vote.getAutomatic())+"');";
    qDebug()<<query;
    QSqlQuery q;

    if(!q.exec(query)){
        qDebug()<<this->lastError();
        return false;
    }

    return true;
}

QVector<VoteRegister> VoteLogRegister::getVoteRegisters(){

    QString query = "Select T_DATE,T_QID , T_ANSWER , T_ADDRESS , N_BITCOINS, B_AUTOMATIC from transactions order by T_DATE; " ;

    qDebug()<<query;
    QSqlQuery q;

    QVector<VoteRegister> registers;
    if(!q.exec(query)){
        qDebug()<<this->lastError();
        return registers;
    }
    VoteRegister vote;


    while( q.next())   {
        vote.setDate(q.value( 0 ).toString());
        vote.setQuid(q.value( 1).toString());
        vote.setAnswer(q.value( 2 ).toString());
        vote.setAddress(q.value( 3 ).toString());
        vote.setVotes(q.value( 4 ).toDouble());
        vote.setAutomatic(q.value( 5 ).toBool());

        registers.append(vote);
    }

    return registers;

}

QMap<QString,double> VoteLogRegister::getAnswersToQuid(QString quid){
    QString query = "Select T_ANSWER, sum(N_BITCOINS) from transactions where T_QID='"+quid+"' group by T_ANSWER " ;
    QSqlQuery q;

    qDebug()<<query;


    QMap<QString,double> votesByAnswer;
    if(!q.exec(query)){
        qDebug()<<this->lastError();
        return votesByAnswer;
    }

    while( q.next())   {
        //Answer, votes
        votesByAnswer.insert(q.value( 0 ).toString(),q.value(1 ).toDouble());
    }

    return votesByAnswer;
}

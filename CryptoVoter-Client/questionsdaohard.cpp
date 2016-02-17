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

#include <QVector>

#include "questionsdaohard.h"
#include "question.h"
QuestionsDAOHard::QuestionsDAOHard(){
}


void QuestionsDAOHard::refreshData(){
    //Pending questions
    QVector<Question> vec;
    vec.append(Question("DIR","question 1","Monday"));
    vec.append(Question("MMG","question 2","Tuesday"));
    vec.append(Question("JPD","question 3","Wednesday"));
    vec.append(Question("NR1","question 4","Thursday"));
    vec.append(Question("ALL","where is the scholarship?","Friday"));

    //Active question
    Question ques ("QEE","How old are you?","one week",Question::active);
    ques.setEndDate("12d");
    ques.addAnswer(Answer(">=20","A",0));
    ques.addAnswer(Answer("<20","B",0));
    vec.append(ques);

    //Complete
    Question quesC ("QEE","What is your hobbie?","one week",Question::completed);

    quesC.addAnswer(Answer("Television","A",10));
    quesC.addAnswer(Answer("Computer","B",20));
    quesC.addAnswer(Answer("Rubik cube","C",40));
    quesC.addAnswer(Answer("Sleep","D",15));
    quesC.addAnswer(Answer("Tenis","E",30));
    quesC.addAnswer(Answer("Karaoke","Z",0));

    vec.append(quesC);

    Question ques2 ("QEF","What is your favorite sport?","one week",Question::completed);

    Answer auxAnswer;
    auxAnswer=Answer("Football","A",60);
    auxAnswer.addAddress("kljhsdadjk",10);
    auxAnswer.addAddress("plooookcsa",20);
    auxAnswer.addAddress("risazsdssa",30);
    ques2.addAnswer(auxAnswer);
    auxAnswer=Answer("Basket","B",20);

    auxAnswer.addAddress("dddasdaksa",10);
    auxAnswer.addAddress("peeiksscsa",9);
    auxAnswer.addAddress("platamionc",1);
    ques2.addAnswer(auxAnswer);

    auxAnswer=Answer("Voley","C",36);
    auxAnswer.addAddress("pplsakn",36);
    ques2.addAnswer(auxAnswer);

    auxAnswer=Answer("Tenis","D",47);
    auxAnswer.addAddress("pplsakn",1);
    auxAnswer.addAddress("inmboul",26);
    auxAnswer.addAddress("ccxzvvn",2);
    auxAnswer.addAddress("zzzzzzz",18);
    ques2.addAnswer(auxAnswer);
    vec.append(ques2);

    emit this->refreshQuestions(vec);
}

void QuestionsDAOHard::refreshBalances(QString quid){
    QVector<Answer> ans;

    ans.append(Answer("20","A",0));
    ans.append(Answer("24","B",0));
    ans.append(Answer("26","C",0));
    ans.append(Answer("28","D",0));
    ans.append(Answer("30","E",0));
    ans.append(Answer("NonOfAbove","Z",0));

    refreshAns(ans);
}



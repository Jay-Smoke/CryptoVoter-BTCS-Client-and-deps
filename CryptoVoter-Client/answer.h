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

#ifndef ANSWER_H
#define ANSWER_H

#include <qstring.h>
#include <QMap>


class Answer
{
    public:
        /*!
         * \brief Constructor: Initialize in '0' the class atributes and starts the gui
        */
        Answer();

        /*!
         * \brief Constructor: Initialize with the parameters values
        */
        Answer(QString option, QString character, int votes);

        /*!
         * \brief Modifies the 'option' atribute
        */
        void setOption(QString option);

        /*!
         * \brief Returns the 'option' atribute
        */
        QString getOption() const;

        /*!
         * \brief Modifies the 'votes' atribute
        */
        void setVotes(double votes);

        /*!
         * \brief Returns the 'votes' atribute
        */
        double getVotes() const;

        /*!
         * \brief Modifies the 'character' atribute
        */
        void setCharacter(QString character);

        /*!
         * \brief Returns the 'character' atribute
        */
        QString getCharacter() const;

        /*!
         * \brief Returns the associated question id
         * \return
         */
        QString getQuid() const;

        /*!
         * \brief Set the associated question id
         * \param value
         */
        void setQuid(const QString &value);

        QMap<QString, double> getBalance() const;
        void addAddress(QString address, double votes);
        int getVotesAddress(QString address) const;

private:

        QString option;//!<@brief Option: Answer to the question. Ex: "yes", "no", "none of above"
        QString character; //!<@brief Character: Id of the answer. Ex: "A", "B", "Z"
        double votes; //!<@brief Votes: Number of votes
        QString quid;//!<@brief quid: associated question id
        QMap<QString, double> balance;//!<@brief balance: contains all address and votes for the anwser

};

#endif // ANSWER_H

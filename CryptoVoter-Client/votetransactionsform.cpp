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
#include "votetransactionsform.h"
#include "ui_votetransactionsform.h"
#include "votelogregister.h"


VoteTransactionsForm::VoteTransactionsForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::VoteTransactions)
{
    ui->setupUi(this);
    connect(this->ui->bRefresh,SIGNAL(clicked()),this,SLOT(refresh()));
    connect(this->ui->bClose,SIGNAL(clicked()),this,SLOT(close()));


    this->modelRegisters = new QStandardItemModel(0, 6, parent);
    this->registersProxyModel = new QSortFilterProxyModel;

    this->ui->tRegistersView->setRootIsDecorated(false);
    this->ui->tRegistersView->setAlternatingRowColors(true);
    this->ui->tRegistersView->setModel(registersProxyModel);
    this->ui->tRegistersView->setSortingEnabled(true);
}

VoteTransactionsForm::~VoteTransactionsForm()
{
    delete ui;
}

void VoteTransactionsForm::addRegister(VoteRegister voteRegister)
{
    modelRegisters->insertRow(0);
    modelRegisters->setData(modelRegisters->index(0, 0), voteRegister.getDate());
    modelRegisters->setData(modelRegisters->index(0, 1), voteRegister.getQuid());
    modelRegisters->setData(modelRegisters->index(0, 2), voteRegister.getAnswer());
    modelRegisters->setData(modelRegisters->index(0, 3), voteRegister.getAddress());
    if (voteRegister.getAutomatic()){
        modelRegisters->setData(modelRegisters->index(0, 4), "AUTO");
        modelRegisters->setData(modelRegisters->index(0, 5), QString::number(voteRegister.getVotes(),'G',9));
    }else{
        modelRegisters->setData(modelRegisters->index(0, 4), "MANUAL");
        modelRegisters->setData(modelRegisters->index(0, 5), "Unknown");
    }

}





void VoteTransactionsForm::refresh(){

    this->modelRegisters->removeRows(0,this->modelRegisters->rowCount());
    modelRegisters->setHeaderData(0, Qt::Horizontal, QObject::tr("Date"));
    modelRegisters->setHeaderData(1, Qt::Horizontal, QObject::tr("QID"));
    modelRegisters->setHeaderData(2, Qt::Horizontal, QObject::tr("ANS"));
    modelRegisters->setHeaderData(3, Qt::Horizontal, QObject::tr("Address"));
    modelRegisters->setHeaderData(4, Qt::Horizontal, QObject::tr("Mode"));
    modelRegisters->setHeaderData(5, Qt::Horizontal, QObject::tr("Votes"));

    QVector<VoteRegister> auxRegisters=VoteLogRegister::getInstancia()->getVoteRegisters();

    VoteRegister voteRegister;
    for (int i=0; i<auxRegisters.size();i++){
        voteRegister=auxRegisters.at(i);
        addRegister(voteRegister);
    }

    registersProxyModel->setSourceModel(modelRegisters);
    this->ui->tRegistersView->sortByColumn(0, Qt::AscendingOrder);
    this->ui->tRegistersView->resizeColumnToContents(3);
    this->ui->tRegistersView->resizeColumnToContents(0);
    this->ui->tRegistersView->setEditTriggers(QAbstractItemView::NoEditTriggers);
}

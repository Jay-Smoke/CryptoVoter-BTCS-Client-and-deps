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

#include <QSplitter>
#include "chartform.h"
#include "ui_chartform.h"
#include <QStandardItemModel>
#include <QTreeView>
#include <QDebug>
#include <QDesktopServices>
#include <QUrl>
#include "pieview.h"
ChartForm::ChartForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ChartForm)
{

    ui->setupUi(this);
    this->setWindowModality(Qt::ApplicationModal);
}

ChartForm::~ChartForm()
{
    delete ui;
}

void ChartForm::initialize(){
    model = new QStandardItemModel(8, 3, this);
    model->setHeaderData(0, Qt::Horizontal, tr("Answer"));
    model->setHeaderData(1, Qt::Horizontal, tr("Votes"));
    model->setHeaderData(2, Qt::Horizontal, tr("%"));

    modelBalance = new QStandardItemModel(8, 2, this);
    modelBalance->setHeaderData(0, Qt::Horizontal, tr("Address"));
    modelBalance->setHeaderData(1, Qt::Horizontal, tr("Votes"));

    QSplitter *splitter = new QSplitter;
    QTreeView *table = this->ui->tAnswers;

    connect(table, SIGNAL(clicked(QModelIndex)), this, SLOT(seeQuestionBalance(QModelIndex)));
    connect(this->ui->tBalance,SIGNAL(clicked(QModelIndex)),this,SLOT(openPage(QModelIndex)));

    pieChart = new PieView;

    //splitter->addWidget(table);
    splitter->addWidget(pieChart);

    splitter->setStretchFactor(0, 0);
    splitter->setStretchFactor(1, 1);

    table->setModel(model);
    pieChart->setModel(model);
    table->setAlternatingRowColors(true);
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);

    this->ui->tBalance->setModel(this->modelBalance);
    this->ui->tBalance->setAlternatingRowColors(true);
    this->ui->tBalance->setEditTriggers(QAbstractItemView::NoEditTriggers);
    QItemSelectionModel *balanceModel = new QItemSelectionModel(modelBalance);
    this->ui->tBalance->setSelectionModel(balanceModel);
    this->ui->tBalance->setSortingEnabled(true);

    this->ui->tAnswers->setToolTip("Click on a row to see votes");
    this->ui->tBalance->setToolTip("Click on a row to open the blockchain web");

    QItemSelectionModel *selectionModel = new QItemSelectionModel(model);
    table->setSelectionModel(selectionModel);
    pieChart->setSelectionModel(selectionModel);

    this->ui->gridLayout->addWidget(splitter);
    this->setEnabled(false);
}


void ChartForm::clearForm(){
    //I clear the model before change the question
    model->removeRows(0, model->rowCount(QModelIndex()), QModelIndex());
    modelBalance->removeRows(0, modelBalance->rowCount(QModelIndex()), QModelIndex());
    this->setWindowTitle("Results");
    this->ui->lQuestion->setText(" ");
    this->ui->lTotal->setText("Total votes: ");
}



void ChartForm::refresh(Question question){
    //TODO reimplement
    this->totalVotes=0;
    this->setWindowTitle("Results - "+question.getQuid());
    this->ui->lQuestion->setText(question.getQuestion());
    //this->ui->lQuestion->setText(question.getQuid()+" - "+question.getQuestion());
    model->removeRows(0, model->rowCount(QModelIndex()), QModelIndex());

    QVector<Answer> ans=question.getAnswers();
    this->quest=question;

    for (int i=0;i<ans.size();i++){
            quest.addAnswer(*question.getAnswer(i));
            model->insertRows(i, 1, QModelIndex());
            model->setData(model->index(i, 0, QModelIndex()), ans.at(i).getCharacter()+")"+ans.at(i).getOption());
            model->setData(model->index(i, 1, QModelIndex()), ans.at(i).getVotes());
            //TODO: Search a better way to get colours (see chart colours)
            model->setData(model->index(i, 0, QModelIndex()),
                           QColor((Qt::GlobalColor)(i+7)), Qt::DecorationRole);
            this->totalVotes=this->totalVotes+ans.at(i).getVotes();
    }

    for (int i=0;i<ans.size();i++){
            quest.addAnswer(*question.getAnswer(i));
            model->setData(model->index(i, 2, QModelIndex()), ans.at(i).getVotes()*100.0/this->totalVotes);
    }


    this->ui->tAnswers->setSortingEnabled(true);
    this->model->sort(0);
    this->ui->tAnswers->resizeColumnToContents(0);
    this->ui->tAnswers->resizeColumnToContents(1);
    this->ui->tAnswers->resizeColumnToContents(2);    
    this->setEnabled(true);


    QString aux;
    aux="Total votes: " + QString::number(this->totalVotes);

    this->ui->lTotal->setText(aux);
}


void ChartForm::seeQuestionBalance(QModelIndex index){
    QString IdAnswer=this->model->data(model->index(index.row(),0)).toString().at(0);
    qDebug()<<"Selected answer - "<<IdAnswer;

    modelBalance->removeRows(0, modelBalance->rowCount(QModelIndex()), QModelIndex());

    for (int i=0; i<this->quest.getAnswers().size();i++){
        if (this->quest.getAnswers().at(i).getCharacter().compare(IdAnswer)==0){
            qDebug()<<this->quest.getAnswers().at(i).getBalance().keys();
            int num=0;
            QMapIterator<QString, double> it(this->quest.getAnswers().at(i).getBalance());
            while (it.hasNext()) {
                it.next();
                qDebug() << num << it.key() << ": " << it.value();
                modelBalance->insertRows(num, 1, QModelIndex());
                modelBalance->setData(modelBalance->index(num, 0, QModelIndex()), it.key());
                modelBalance->setData(modelBalance->index(num, 1, QModelIndex()), it.value());
                num++;

            }

            break;
        }
    }
    this->ui->tBalance->sortByColumn(VOTES_COLUMN);
    this->ui->tBalance->resizeColumnsToContents();    
    this->modelBalance->sort(1,Qt::DescendingOrder);
}

void ChartForm::openPage(QModelIndex index){
    QString url=QString("http://45.55.216.92:30333/address/") + this->modelBalance->data(index).toString();
    qDebug()<<"Open "<<url;

    QDesktopServices::openUrl(QUrl(url));
}

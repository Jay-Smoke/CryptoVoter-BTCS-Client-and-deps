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

#include "eulaform.h"
#include "ui_eulaform.h"
#include "version.h"
#include <QMessageBox>
#include <QTextStream>
#include <QDebug>
#include "configuration.h"

EULAForm::EULAForm(QWidget *parent, QString fileName, bool modificable) :
    QWidget(parent),
    ui(new Ui::EULAForm)
{
    ui->setupUi(this);
    this->setWindowModality(Qt::ApplicationModal);
    this->ui->lblAppName->setText(QString(Version::getAppName().c_str()));
    this->ui->lblVersionValue->setText(QString(Version::shortVersion().c_str()));

    connect(this->ui->bConfirm,SIGNAL(clicked()),this,SLOT(confirmTermns()));

    this->sceneIcon=new QGraphicsScene();
    QPixmap img(":images/bitcoinBlue.png");
    this->icon=new QGraphicsPixmapItem(img);
    this->icon->setPos(QPoint(20,20));
    this->sceneIcon->addItem(this->icon);

    this->ui->gphView->setScene(this->sceneIcon);


    if (!fileName.isEmpty()){
        this->loadFile(fileName);
    }

    this->ui->chkAccept->setEnabled(!modificable);
    this->ui->chkAccept->setChecked(modificable);

}


EULAForm::~EULAForm()
{
    delete ui;
}


void EULAForm::loadFile(const QString fileName){
    QFile file(fileName);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("Application"),
                             tr("Cannot read file %1:\n%2.")
                             .arg(fileName)
                             .arg(file.errorString()));
        return;
    }

    QTextStream in(&file);

    this->ui->txtEula->setPlainText(in.readAll());
    this->ui->txtEula->setReadOnly(true);

}

void EULAForm::refuseMessage()
{
    QMessageBox msgBox;
    msgBox.setWindowTitle(Version::getAppName().c_str());
    msgBox.setIcon(QMessageBox::Warning);
    msgBox.setText("You must accept terms to use the application");
    msgBox.exec();

}

void EULAForm::confirmTermns(){

    Configuration::getInstancia()->setEULAAcceptance(this->ui->chkAccept->isChecked());

    if (this->ui->chkAccept->isChecked()){
        this->hide();
    }else{
        this->refuseMessage();
    }
}

void EULAForm::closeEvent( QCloseEvent *event){
    event->ignore();
    QMessageBox msgBox(this);
    msgBox.setIcon(QMessageBox::Question);
    msgBox.setWindowTitle(Version::getAppName().c_str());
    msgBox.setText("Are you sure you want to exit the application?");

    //TODO: Se podrian usar los botones de qt
    QPushButton *yesButton = msgBox.addButton(tr("Accept"), QMessageBox::ActionRole);
    msgBox.addButton(tr("Cancel"), QMessageBox::ActionRole);
    msgBox.exec();

    if ((QPushButton*)msgBox.clickedButton() == yesButton)        {
        exit(2);
    }
}

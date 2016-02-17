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

#include "aboutform.h"
#include "ui_aboutform.h"
#include <QMessageBox>
#include <QTextStream>
#include "version.h"
AboutForm::AboutForm(QWidget *parent, QString fileName) :
    QWidget(parent),
    ui(new Ui::AboutForm)
{
    ui->setupUi(this);
    this->setWindowModality(Qt::ApplicationModal);

    this->ui->lblClient->setText("<a href=\"http://www.cryptovoter.com\">CryptoVoter</a>");
    this->ui->lblClient->setTextFormat(Qt::RichText);
    this->ui->lblClient->setTextInteractionFlags(Qt::TextBrowserInteraction);
    this->ui->lblClient->setOpenExternalLinks(true);

    QString aux=QString("<a href=\"http://www.btcs-voter.com\">")+ QString(Version::getAppName().c_str()) +QString(" ")+ QString(Version::shortVersion().c_str())+QString("</a>");
    this->ui->lblVersionValue->setText(aux);
    this->ui->lblVersionValue->setTextFormat(Qt::RichText);
    this->ui->lblVersionValue->setTextInteractionFlags(Qt::TextBrowserInteraction);
    this->ui->lblVersionValue->setOpenExternalLinks(true);

    this->ui->lblCompany->setText("<a href=\"http://www.thebcilabs.com\">Blockchain Innovation Labs, Inc.</a>");
    this->ui->lblCompany->setTextFormat(Qt::RichText);
    this->ui->lblCompany->setTextInteractionFlags(Qt::TextBrowserInteraction);
    this->ui->lblCompany->setOpenExternalLinks(true);


    this->ui->lblDevelpedBy->setText("<a href=\"http://www.diremar.com\">Diremar SW Agency</a>");
    this->ui->lblDevelpedBy->setTextFormat(Qt::RichText);
    this->ui->lblDevelpedBy->setTextInteractionFlags(Qt::TextBrowserInteraction);
    this->ui->lblDevelpedBy->setOpenExternalLinks(true);

    this->sceneIcon=new QGraphicsScene();
    QPixmap img(":images/bitcoinBlue.png");
    this->icon=new QGraphicsPixmapItem(img);
    this->icon->setPos(QPoint(20,20));
    this->sceneIcon->addItem(this->icon);
    this->ui->gphView->setScene(this->sceneIcon);


    if (!fileName.isEmpty()){
        this->loadFile(fileName);
    }
}

AboutForm::~AboutForm()
{
    delete ui;
}

void AboutForm::setLinkVisible()
{

    ui->lblDevelpedBy->setVisible(true);
}


void AboutForm::loadFile(const QString fileName){
    QFile file(fileName);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("Application"),
                             tr("Cannot read file %1:\n%2.")
                             .arg(fileName)
                             .arg(file.errorString()));
        return;
    }

    QTextStream in(&file);

    this->ui->txtAbout->setPlainText(in.readAll());
    this->ui->txtAbout->setReadOnly(true);

}

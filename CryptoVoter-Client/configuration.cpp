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

#include "configuration.h"

#include <QMessageBox>
#include <QDir>
#include <QDebug>

Configuration *Configuration::instancia = 0;

Configuration::Configuration()
{
    // El ini debe estar al lado del ejecutable
    QString iniPath = QDir().dirName() + "/cryptovoter.ini";

    //Verificamos que exista el archivo de configuracion
    QFile *ab = new QFile ( iniPath );
    if (!ab->exists()){
        qDebug()<<"Error abriendo el archivo de configuracion \n";

        QMessageBox::critical(0,"CryptoVoter","Error abriendo el archivo de configuracion: 'cryptovoter.ini' \n");

        return;
    }

    // Abro el .ini y cargo los settings
    this->settings = new QSettings(iniPath,QSettings::IniFormat);

}


// Es singleton
Configuration *Configuration::getInstancia(){
    if(Configuration::instancia == 0)
        Configuration::instancia = new Configuration();
    return Configuration::instancia;
}

QString Configuration::getWalletPath(){
   return this->settings->value("Wallet/path").toString();
}

void Configuration::setWalletPath(QString path){
    this->settings->setValue("Wallet/path",path);
}

bool Configuration::getEULAAcceptance(){
   return this->settings->value("EULA/acceptance",false).toBool();
}

void Configuration::setEULAAcceptance(bool acceptance){
    this->settings->setValue("EULA/acceptance",acceptance);
}



void Configuration::setNetFee(double netfee){
    this->settings->setValue("VOTING/netfee",netfee);
}

double Configuration::getNetFee(){
    return this->settings->value("VOTING/netfee",0).toDouble();
}

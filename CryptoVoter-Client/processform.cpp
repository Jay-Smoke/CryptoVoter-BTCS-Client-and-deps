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

#include "processform.h"
#include "ui_processform.h"
#include <QFileDialog>
#include <QInputDialog>
#include <QMessageBox>
#include <QDebug>
#include <QTimer>
#include <QJsonDocument>
#include <QJsonObject>
#include <QPixmap>
#include <QSplashScreen>
#include <QThread>
#include "configuration.h"
#include "version.h"
#include <QSettings>
#include "votelogregister.h"
#include "voteregister.h"

ProcessForm::ProcessForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ProcessForm),
    manualInstrDialog(0,"manualvoting.txt")
{
    ui->setupUi(this);
    this->setWindowTitle("Voting");
    this->ui->txtProgress->setReadOnly(true);
    this->votingQuestion = "";
    this->votingPrefix = "";
    this->deadline = "";
    this->netFee = 0;
    this->resetVotingProcess();
    connect(this->ui->bSearch,SIGNAL(clicked()),this,SLOT(searchWallet()));
    connect(this->ui->bStart,SIGNAL(clicked()),this,SLOT(executeVotingProcess()));
    this->setWindowModality(Qt::ApplicationModal);
}

void ProcessForm::resetVotingProcess(){
    // Reset state
    this->votingState = IDLE_STATE;
    // Stops retry timer
    this->retryTmr.stop();
    this->proc = 0;
    this->bitcoind = 0;
    this->walletPasswd = "";
    this->vanityAddress = "";
    this->privatKey = "";
    this->votingAddress = "";
    this->btcsToTransfer = 0.0;
    this->ui->progressBar->setValue(0);
    this->ui->bSearch->setEnabled(true);
    this->ui->bStart->setEnabled(true);
}

ProcessForm::~ProcessForm()
{
    delete ui;
}

void ProcessForm::abortVotingSequence(){
    // Delete last process and disconnect signals
    if(this->proc != 0){
        disconnect(this->proc,SIGNAL(finished(int,QProcess::ExitStatus)),this,SLOT(votingStateMachine(int,QProcess::ExitStatus)));
        this->proc->terminate();
        this->proc->waitForFinished(5000);
        delete this->proc;
    }

    this->resetVotingProcess();
    this->lockWallet();// This process just runs and dies alone
    this->stopBitcoindServer();
}

// Getters and setters
QString ProcessForm::getVotingPrefix() const
{
    return votingPrefix;
}

void ProcessForm::setVotingPrefix(const QString &value)
{
    votingPrefix = value;
}
QString ProcessForm::getWallet() const
{
    return wallet;
}

void ProcessForm::setWallet(const QString &value)
{
    wallet = value;
}
QString ProcessForm::getWalletPasswd() const
{
    return walletPasswd;
}

void ProcessForm::setWalletPasswd(const QString &value)
{
    walletPasswd = value;
}
QString ProcessForm::getVanityAddress() const
{
    return vanityAddress;
}

void ProcessForm::setVanityAddress(const QString &value)
{
    vanityAddress = value;
}
QString ProcessForm::getPrivatKey() const
{
    return privatKey;
}

void ProcessForm::setPrivatKey(const QString &value)
{
    privatKey = value;
}
QString ProcessForm::getVotingAddress() const
{
    return votingAddress;
}

void ProcessForm::setVotingAddress(const QString &value)
{
    votingAddress = value;
}
double ProcessForm::getBtcs() const
{
    return btcsToTransfer;
}

void ProcessForm::setBtcs(double value)
{
    btcsToTransfer = value;
}

void ProcessForm::show(){
    QWidget::show();
    // I clear the progress
    this->resetVotingProcess();
    this->ui->txtProgress->clear();
    this->updateText("Press start button to begin..");

    // Verifico si esta seteada la billetera y sino la pido
    this->wallet=Configuration::getInstancia()->getWalletPath();
    this->ui->txtWallet->setText(this->wallet);
}

void ProcessForm::updateText(QString txt){
    this->ui->txtProgress->append(txt);
}

bool ProcessForm::searchWallet(){
    QString directory = QFileDialog::getExistingDirectory(this, "Select Bitcoin-sCrypt wallet directory.", Configuration::getInstancia()->getWalletPath());
    if(directory.isEmpty()){        
        return false;
    }
    this->wallet = directory;
    this->ui->txtWallet->setText(directory);
    // Saves the directory so the user doesn't have to navigate to it the next time
    Configuration::getInstancia()->setWalletPath(directory);
    return true;
}

void ProcessForm::votingStateMachine(int exitCode,QProcess::ExitStatus exitStatus){
    QString out;
    QMessageBox msgBox;
    QString txt;
    QSettings settings;
    double btc;
    double fee;
    QString feeTxt = "";
    VoteLogRegister *voteRegistry = VoteLogRegister::getInstancia();

    switch(this->votingState){// state machine
    case IDLE_STATE:
        break;//Nothing to do

    case GENERATE_VANITY_ADDRESS:
        this->ui->progressBar->setValue(20);
        out = this->readProcessOutput();
        this->vanityAddress = out.mid(out.indexOf("Address: ")+QString("Address: ").length(), 34);
        this->privatKey = out.mid(out.indexOf("Privkey: ")+QString("Privkey: ").length(), 51);

        //I ask for manual voting
        if(this->getManualVoting()){
            this->updateText("Manual voting was choosen. Follow the instructions.");
            this->manualInstrDialog.setWindowTitle("Manual Voting Instructions");
            this->manualInstrDialog.show();

            // We save the voting info for the record
            voteRegistry->insertVote(VoteRegister(this->vanityAddress,0,false));
            this->ui->bSearch->setEnabled(false);
            this->ui->bStart->setEnabled(false);

            this->resetVotingProcess();
            return;
        }

        // I prompt the user to ensure that there are not other programs using the wallet.
        this->warningMessage("Please ensure that your Bitcoin wallet client is closed before you continue.",false);

        // I ask for bitcoin.conf
        txt =   "You must have your rpcuser and rpcpassword specified inside your bitcoin.conf file to continue with voting.\
                \nIf you do not have a bitcoin.conf file created, the application can create one for you with the required rpcuser and rpcpassword specified. \
                \nIf you already have a bitcoin.conf file created, please make sure the following entries are included in the file:  \
                \n\trpcuser=randomrpcuser \
                \n\trpcpassword=crypto1234. \
                \nDo you want to continue?";

        msgBox.setWindowTitle("bitcoin.conf");
        msgBox.setText(txt);
        msgBox.addButton(QMessageBox::Yes);
        msgBox.addButton(QMessageBox::No);
        msgBox.setDefaultButton(QMessageBox::Yes);
        if(msgBox.exec() == QMessageBox::No){
            this->abortVotingSequence();
            this->updateText("Voting aborted by the user. Try again pressing start..");
            return;
        }

        // If doesnt exists, we create a default one
        if(!this->existsBitcoinConfFile(this->getWallet())){
            this->createBitcoinConfFile(this->getWallet());
        }else{
            this->updateText("bitcoin.conf found, just using it..");
        }

        this->votingState = START_BITCOIND_SERVER;
        // User must have bitcoin.conf on wallet's directory, or the client will create it.
        /* It must have the following 2 lines:
         *
         * rpcuser=bitcoinrpc
         * rpcpassword=crypto1234
         * */
        this->startBitcoindServer();
        break;

    case START_BITCOIND_SERVER:
        this->ui->progressBar->setValue(30);
        // Paso al siguiente estado
        this->votingState = UNLOCK_WALLET;
        if(!this->unlockWallet()){
            this->updateText("Wallet unlock canceled by the user. Voting aborted.");
            this->abortVotingSequence();
        }
        break;

    case UNLOCK_WALLET:
        this->ui->progressBar->setValue(40);
        out = this->readProcessOutput();
        /* Posible outputs:
         * ""
         * "error: couldn't connect to server"
         * "error: {"code":-14,"message":"Error: The wallet passphrase entered was incorrect."}"
         */
        if(out.indexOf("error: ") != -1){// Some error ocurred
            if(out.indexOf("error: couldn't connect to server") != -1){// No se pudo conectar el servidor
                if(reconnectTryes > 0){
                    reconnectTryes--;
                    // I fire a timer to retry in 3 secconds
                    this->updateText("Retrying connection with bitcoind server...");
                    this->votingState = START_BITCOIND_SERVER;
                    this->retryTmr.setInterval(3000);
                    this->retryTmr.setSingleShot(true);
                    connect(&this->retryTmr, SIGNAL(timeout()), this, SLOT(retryBitcoindConnection()));
                    this->retryTmr.start();
                    return;
                }else{// Retryes exausted
                    this->abortVotingSequence();
                    this->warningMessage("Error connecting with bitcoind. Voting aborted.\n Check that ports 30200 and 30201 are enabled for bitcoind.exe, on your firewall settings.",false);
                    return;
                }
            }
            reconnectTryes = BITCOIND_CONNECT_MAX_TRYES;
            QString jsonStrError = out.mid(out.indexOf("error: ")+QString("error: ").length());
            QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonStrError.toUtf8());
            QJsonObject jsonErrorObj = jsonDoc.object();
            int errorCode = jsonErrorObj["code"].toInt();
            switch(errorCode){
                case RPC_WALLET_PASSPHRASE_INCORRECT:
                    this->warningMessage("Wallet password incorrect.",false);
                    this->walletPasswd = "";
                    if(!this->unlockWallet()){
                        this->updateText("Wallet unlock canceled by the user. Voting aborted.");
                        this->abortVotingSequence();
                    }
                    return;
                case RPC_WALLET_WRONG_ENC_STATE:
                    this->updateText("Wallet isn't encrypted, skiping wallet unlock step..");
                    this->votingState = IMPORT_VANITY_ADDRESS;
                    this->importVanityAddressToWalltet();
                    return;
                default:
                    this->abortVotingSequence();
                    this->warningMessage("Error: " + jsonStrError + "\nVoting aborted.",false);
                    return;
            }
        }
        this->votingState = IMPORT_VANITY_ADDRESS;
        this->importVanityAddressToWalltet();
        break;

    case IMPORT_VANITY_ADDRESS:
        this->ui->progressBar->setValue(50);
        out = this->readProcessOutput();
        /* Posible outputs:
         * ""
         * "error: couldn't connect to server"
         * "error: {"code":-13,"message":"Error: Please enter the wallet passphrase with walletpassphrase first."}"
         */
        if(out.indexOf("error: ") != -1){// Some error ocurred
            if(out.indexOf("error: couldn't connect to server") != -1){// No se pudo conectar el servidor
                if(reconnectTryes > 0){
                    reconnectTryes--;
                    // I fire a timer to retry in 3 secconds
                    this->updateText("Retrying connection with bitcoind server...");
                    this->votingState = IMPORT_VANITY_ADDRESS;
                    this->importVanityAddressToWalltet();
                    return;
                }else{// Retryes exausted
                    this->abortVotingSequence();
                    this->warningMessage("Error connecting with bitcoind. Voting aborted.\n Check that ports 30200 and 30201 are enabled for bitcoind.exe, on your firewall settings.",false);
                    return;
                }
            }
            reconnectTryes = BITCOIND_CONNECT_MAX_TRYES;
            QString jsonStrError = out.mid(out.indexOf("error: ")+QString("error: ").length());
            QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonStrError.toUtf8());
            QJsonObject jsonErrorObj = jsonDoc.object();
            int errorCode = jsonErrorObj["code"].toInt();
            switch(errorCode){
                // This is not supposed to happen, but just in case someone locks the wallet outside
                // cryptovoter during the voting process.
                case RPC_WALLET_UNLOCK_NEEDED:
                    this->warningMessage("Please unlock your wallet again.",false);
                    this->walletPasswd = "";
                    if(!this->unlockWallet()){
                        this->updateText("Wallet unlock canceled by the user. Voting aborted.");
                        this->abortVotingSequence();
                    }
                    return;
                default:
                    this->abortVotingSequence();
                    this->warningMessage("Error: " + jsonStrError + "\nVoting aborted.",false);
                    return;
            }
        }
        this->votingState = CONFIRM_PRIVATE_ADDRESS_IMPORTED;
        this->updateText("Checking imported private address/key..");
        this->verifyImportedKey();
        break;

    case CONFIRM_PRIVATE_ADDRESS_IMPORTED:
        this->ui->progressBar->setValue(55);
        out = this->readProcessOutput();
        /* Posible outputs:
         * ""
         * "Vote:9KN-A"
         * "error: couldn't connect to server"
         */
        if(out.indexOf("error: ") != -1){// Some error ocurred
            if(out.indexOf("error: couldn't connect to server") != -1){// No se pudo conectar el servidor
                if(reconnectTryes > 0){
                    reconnectTryes--;
                    // I fire a timer to retry in 3 secconds
                    this->updateText("Retrying connection with bitcoind server...");
                    this->votingState = CONFIRM_PRIVATE_ADDRESS_IMPORTED;
                    this->verifyImportedKey();
                    return;
                }else{// Retryes exausted
                    this->abortVotingSequence();
                    this->warningMessage("Error connecting with bitcoind. Voting aborted.\n Check that ports 30200 and 30201 are enabled for bitcoind.exe, on your firewall settings.",false);
                    return;
                }
            }
            reconnectTryes = BITCOIND_CONNECT_MAX_TRYES;
            QString jsonStrError = out.mid(out.indexOf("error: ")+QString("error: ").length());
            QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonStrError.toUtf8());
            QJsonObject jsonErrorObj = jsonDoc.object();
            int errorCode = jsonErrorObj["code"].toInt();
            switch(errorCode){
                // This is not supposed to happen, but just in case someone locks the wallet outside
                // cryptovoter during the voting process.
                case RPC_WALLET_UNLOCK_NEEDED:
                    this->warningMessage("Please unlock your wallet again.",false);
                    this->walletPasswd = "";
                    if(!this->unlockWallet()){
                        this->updateText("Wallet unlock canceled by the user. Voting aborted.");
                        this->abortVotingSequence();
                    }
                    return;
                default:
                    this->abortVotingSequence();
                    this->warningMessage("Error: " + jsonStrError + "\nVoting aborted.",false);
                    return;
            }
        }
        // I check if the private address/key were successfully imported
        if(out.mid(0,this->getVotingLabel().length()) != this->getVotingLabel()){// Otherwise, i abort the voting sequence
            this->updateText("Error verifying the imported vanity address.\nEnsure that your wallet's client is closed and try again.");
            this->abortVotingSequence();
            return;
        }
        // I save the public address to transfer BTC later
        this->votingAddress = this->vanityAddress;
        this->warningMessage("Voting address successfully. Imported with label: \n" + this->getVotingLabel(),false);
        this->votingState = CHECK_WALLET_BALANCE;
        this->checkWalletBalance();
        break;


    case CHECK_WALLET_BALANCE:
        this->ui->progressBar->setValue(70);
        out = this->readProcessOutput();
        /* Posible outputs:
         * "0.00001"
         * "error: couldn't connect to server"
         */
        if(out.indexOf("error: ") != -1){// Some error ocurred
            if(out.indexOf("error: couldn't connect to server") != -1){// No se pudo conectar el servidor
                if(reconnectTryes > 0){
                    reconnectTryes--;
                    // I fire a timer to retry in 3 secconds
                    this->updateText("Retrying connection with bitcoind server...");
                    this->votingState = CHECK_WALLET_BALANCE;
                    this->checkWalletBalance();
                    return;
                }else{// Retryes exausted
                    this->abortVotingSequence();
                    this->warningMessage("Error connecting with bitcoind. Voting aborted.\n Check that ports 30200 and 30201 are enabled for bitcoind.exe, on your firewall settings.",false);
                    return;
                }
            }
            reconnectTryes = BITCOIND_CONNECT_MAX_TRYES;
            QString jsonStrError = out.mid(out.indexOf("error: ")+QString("error: ").length());
            QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonStrError.toUtf8());
            QJsonObject jsonErrorObj = jsonDoc.object();
            int errorCode = jsonErrorObj["code"].toInt();
            switch(errorCode){
                // This is not supposed to happen, but just in case someone locks the wallet outside
                // cryptovoter during the voting process.
                case RPC_WALLET_UNLOCK_NEEDED:
                    this->warningMessage("Please unlock your wallet again.",false);
                    this->walletPasswd = "";
                    if(!this->unlockWallet()){
                        this->updateText("Wallet unlock canceled by the user. Voting aborted.");
                        this->abortVotingSequence();
                    }
                    return;
                default:
                    this->abortVotingSequence();
                    this->warningMessage("Error: " + jsonStrError + "\nVoting aborted.",false);
                    return;
            }
        }
        bool ok;
        this->walletBalance = out.toDouble(&ok);

        if(!ok){
            this->abortVotingSequence();
            this->warningMessage("Unrecorable error: Can't get wallet's balance. " + out + " is not a valid number.",false);
            return;
        }

        // If balance is 0 ends the votation
        if(this->walletBalance == 0.0){
            this->abortVotingSequence();
            this->warningMessage("Your wallet's balance is 0.0. Votation aborted ",false);
            return;
        }

        this->updateText( "Wallet Balance: " + QString::number(this->walletBalance,'G',9));
        this->warningMessage("You currently have [" + QString::number(this->walletBalance,'G',9) + "] coins in your wallet and the following voting address was imported into your Bitcoin wallet file:\n" + this->votingAddress,false);
        // I ask how many coins the user want to vote with
        btc = 0.0;
        // Isn't a valid number?  try again
        while (btc <= 0.0){
            btc = QInputDialog::getDouble(this, "BTCS ammount","Out of your balance of [" + QString::number(this->walletBalance,'G',9) + "] coins, how many coins would you like to vote with?",this->getBtcs(),0.0,2147483647.0,8,&ok,0);
            if(!ok){
                this->updateText("You must enter a valid ammount to transfer. Voting cancelled.");
                this->abortVotingSequence();
                return;
            }
        }

        // Haven't enoghth coins?  try again
        if (btc > this->walletBalance){
            this->warningMessage("You don't have enoghth coins. Please try again.",false);
            this->votingStateMachine(0,QProcess::NormalExit);
            return;
        }

        // Amount to transfer
        this->setBtcs(btc);

        // Asks for network fee
        feeTxt = "Would you like to pay a network fee to increase the likelihood of your transaction being included in the next block?\nTypical transaction fee is " + QString::number(this->getNetFee()) + ".";
        msgBox.setWindowTitle("Voluntary Network Fee");
        msgBox.setText(feeTxt);
        msgBox.addButton(QMessageBox::Yes);
        msgBox.addButton(QMessageBox::No);
        msgBox.setDefaultButton(QMessageBox::No);
        if(msgBox.exec() == QMessageBox::Yes){
            this->votingState = SET_NET_FEE;
        }else{// No fee
            this->votingState = ASK_CONFIRMATION;
            this->updateText("Not network fee applied.");
            this->setNetFee(0.0);
        }
        // ejecuto la maquina de estados denuevo
        this->votingStateMachine(0,QProcess::NormalExit);
        break;

    case SET_NET_FEE:
        // I ask for network fee
        fee = 0.0;
        do{
            fee = QInputDialog::getDouble(this, "Network fee","Enter the desired network fee.",this->getNetFee(),this->getNetFee(),2147483647.0,8,&ok,0);
            if(!ok){
                this->updateText("Not network fee applied.");
                fee = 0.0;
                this->setNetFee(fee);
                // sigo con la confirmacion
                this->votingState = ASK_CONFIRMATION;
                // ejecuto la maquina de estados denuevo
                this->votingStateMachine(0,QProcess::NormalExit);
                return;
            }
        }while (fee < 0.0);// Fee can't be negative
        // sigo con la confirmacion
        this->votingState = ASK_CONFIRMATION;
        this->setNetFee(fee); // Sets the default net fee this time (shown later on messageboxes)
        this->setBitcoindTxFee(fee); // Sets the fee on bitcoind this time
        break;

    case ASK_CONFIRMATION:
        out = this->readProcessOutput();

        // Asks for confirmation and moves to next step
        msgBox.setWindowTitle("Corfirm your vote");
        msgBox.setText("Are you sure you want to submit your vote on question:\n "+this->getVotingQuestion()+", \
                        \n\nChoosing answer choice: "+this->getVotingAnswer()+", \
                        \n\nBy sending "+QString::number(this->getBtcs(),'G',9)+" coins, \
                        \n\nNetwork Fee = "+QString::number(this->getNetFee(),'G',9)+" coins, \
                        \n\nTo your voting address of: "+this->getVotingAddress()+", \
                        \n\nLabeled as: "+this->getVotingLabel()+" in your Bitcoin client wallet?");
        msgBox.addButton(QMessageBox::Yes);
        msgBox.addButton(QMessageBox::No);
        msgBox.setDefaultButton(QMessageBox::No);
        if(msgBox.exec() == QMessageBox::No){
            this->ui->progressBar->setValue(100);
            msgBox.setWindowTitle("Votation cancelled");
            msgBox.setText("Votation cancelled by user!");
            msgBox.setStandardButtons(QMessageBox::Ok);
            msgBox.exec();
            this->abortVotingSequence();
            return;
        }
        this->ui->progressBar->setValue(90);
        this->votingState = TRANSFER_BTCS;
        this->updateText("Casting your vote now...");
        this->sendCoinsToAddress(this->getVotingAddress(),this->getBtcs());
        break;

    case TRANSFER_BTCS:
        //splash.finish(this);
        out = this->readProcessOutput();
        /* Posible outputs:
         * "12356" (transaction id)
         * "error: couldn't connect to server"
         */
        if(out.indexOf("error: ") != -1){// Some error ocurred
            if(out.indexOf("error: couldn't connect to server") != -1){// No se pudo conectar el servidor
                if(reconnectTryes > 0){
                    reconnectTryes--;
                    // I fire a timer to retry in 3 secconds
                    this->updateText("Retrying connection with bitcoind server...");
                    this->votingState = ASK_CONFIRMATION;
                    this->retryTmr.setInterval(3000);
                    this->retryTmr.setSingleShot(true);
                    connect(&this->retryTmr, SIGNAL(timeout()), this, SLOT(retryBitcoindConnection()));
                    this->retryTmr.start();
                    return;
                }else{// Retryes exausted
                    this->abortVotingSequence();
                    this->warningMessage("Error connecting with bitcoind. Voting aborted.\n Check that ports 30200 and 30201 are enabled for bitcoind.exe, on your firewall settings.",false);
                    return;
                }
            }
            reconnectTryes = BITCOIND_CONNECT_MAX_TRYES;
            QString jsonStrError = out.mid(out.indexOf("error: ")+QString("error: ").length());
            QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonStrError.toUtf8());
            QJsonObject jsonErrorObj = jsonDoc.object();
            int errorCode = jsonErrorObj["code"].toInt();
            switch(errorCode){
                // This is not supposed to happen, but just in case someone locks the wallet outside
                // cryptovoter during the voting process.
                case RPC_WALLET_UNLOCK_NEEDED:
                    this->votingState = ASK_CONFIRMATION;
                    this->warningMessage("Please unlock your wallet again.",false);
                    this->walletPasswd = "";
                    if(!this->unlockWallet()){
                        this->updateText("Wallet unlock canceled by the user. Voting aborted.");
                        this->abortVotingSequence();
                    }
                    return;
                default:
                    this->abortVotingSequence();
                    this->warningMessage("Unknokwn error" + QString::number(errorCode) + ".Voting aborted.",false);
                    return;
            }
        }

        // I save the transaction ID
        this->setTransactionId(out);
        txt =   "You have successfully voted on question:\n "+this->getVotingQuestion() +
                "\n\nAnswer choice: " + this->getVotingAnswer() +
                "\n\nBy sending "+ QString::number(this->getBtcs(),'G',9) + " coins" +
                "\n\nNetwork Fee = "+QString::number(this->getNetFee(),'G',9)+" coins, \
                \n\nTo the voting address "+ this->getVotingAddress() +
                "\n\nLabeled as:" + this->getVotingLabel() + "in your bitcoin client wallet! \
                \n\nThe transaction id is: \n"+ this->getTransactionId() +
                "\n Please remember not to move your coins until the voting deadline of block "+this->getDeadline()+" has passed or your vote may not count.\n\n After the voting deadline has passed, you can use your coins as you normally would.";
        msgBox.setWindowTitle("Vote confirmed");
        msgBox.setText(txt);
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.exec();
        // We save the voting info for the record. Double voting will generate a warning
        voteRegistry->insertVote(VoteRegister(this->getVotingAddress(),this->getBtcs(),true));
        this->ui->progressBar->setValue(100);
        this->votingState = LOCK_WALLET;
        this->lockWallet();
        break;

    case LOCK_WALLET:
        out = this->readProcessOutput();
        this->votingState = STOP_BITCOIND_SERVER;
        this->stopBitcoindServer();
        QMessageBox::information(this,"Voting process", "Voting is complete.",QMessageBox::Ok);
        break;

    case STOP_BITCOIND_SERVER:
    default:
        out = this->readProcessOutput();
        this->updateText(out);
        break;
    }
}

void ProcessForm::executeProcess(QString process, QString args){
    if(this->proc != 0){// Just in case, if it's a previous process running, i terminate it
        disconnect(this->proc,SIGNAL(finished(int,QProcess::ExitStatus)),this,SLOT(votingStateMachine(int,QProcess::ExitStatus)));
        this->proc->terminate();
        this->proc->waitForFinished(5000);
        delete this->proc;
    }
    this->proc = new QProcess();
    proc->setProcessChannelMode(QProcess::MergedChannels);
    connect(this->proc, SIGNAL(finished(int,QProcess::ExitStatus)),this,SLOT(votingStateMachine(int,QProcess::ExitStatus)));
    QString cmd = process + " " + args;
    this->updateText(cmd);
    this->proc->start(cmd);
    this->proc->waitForStarted(-1);
}

QString ProcessForm::readProcessOutput(){
    QString out;
    out = this->proc->readAll();
    this->updateText(out);
    this->updateText("Exit code:" + QString::number(this->proc->exitCode()));
    return out;
}

void ProcessForm::generateVanityAddress(){
    this->updateText("Generating vanity voting address...");
    this->executeProcess("vanitygen",this->getVotingPrefix());
}

void ProcessForm::startBitcoindServer(){
    if(this->bitcoind != 0){
        this->stopBitcoindServer();
    }
    this->updateText("Staring bitcoind server (it could take some minutes)...");
    this->bitcoind = new QProcess();
    this->bitcoind->setProcessChannelMode(QProcess::MergedChannels);
    QString cmd = "bitcoind -daemon -datadir=\"" + this->getWallet() + "\"";
    this->updateText(cmd);
    this->bitcoind->start(cmd);
    this->bitcoind->waitForStarted(-1);
    this->retryTmr.setInterval(60000);
    this->retryTmr.setSingleShot(true);
    connect(&this->retryTmr, SIGNAL(timeout()), this, SLOT(bitcoindInitTimeFinished()));
    this->retryTmr.start();
}

void ProcessForm::bitcoindInitTimeFinished(){
    this->votingStateMachine(0,QProcess::NormalExit);
}

void ProcessForm::retryBitcoindConnection(){
    this->votingStateMachine(0,QProcess::NormalExit);
}

void ProcessForm::stopBitcoindServer(){
    this->updateText("Stopping bitcoind server...");
    QProcess *auxProc = new QProcess();
    QString cmd = "bitcoind stop";
    this->updateText(cmd);
    auxProc->start(cmd);
    auxProc->waitForFinished();
    // I delete the object
    if(this->bitcoind != 0){
        if(this->bitcoind->waitForFinished(5000)){
            this->updateText("bitcoind server stopped.");
        }else{
            this->updateText("bitcoind server wasn't stopping, process killed..");
            this->bitcoind->terminate();
        }
        delete bitcoind;
    }
}

bool ProcessForm::unlockWallet(){
    this->updateText("Openning wallet...");
    if(this->walletPasswd == ""){
        bool ok;
        QString text = QInputDialog::getText(this, tr("Please unlock your wallet"),
                                             tr("Wallet's password:"), QLineEdit::Password,
                                             QDir::home().dirName(), &ok);
        if (ok && !text.isEmpty()){
            this->setWalletPasswd(text);
        }else {
            return false;// Sin la contraseña no hago nada
        }
    }

    // I dont use executeprocess() method cause i need to hide password
    if(this->proc != 0){// Just in case, if it's a previous process running, i terminate it
        disconnect(this->proc,SIGNAL(finished(int,QProcess::ExitStatus)),this,SLOT(votingStateMachine(int,QProcess::ExitStatus)));
        this->proc->terminate();
        this->proc->waitForFinished(5000);
        delete this->proc;
    }
    this->proc = new QProcess();
    proc->setProcessChannelMode(QProcess::MergedChannels);
    connect(this->proc, SIGNAL(finished(int,QProcess::ExitStatus)),this,SLOT(votingStateMachine(int,QProcess::ExitStatus)));
    QString cmd = "bitcoind -datadir=\"" + this->getWallet() + "\" walletpassphrase " + this->getWalletPasswd() + " " + QString::number(WALLET_UNLOCK_TIME);
    this->updateText("bitcoind -datadir=\"" + this->getWallet() +"\" walletpassphrase ***** " + QString::number(WALLET_UNLOCK_TIME));// Don't show password on log
    this->proc->start(cmd);
    this->proc->waitForStarted(-1);
    return true;
}

void ProcessForm::lockWallet(){
    this->updateText("Closing wallet...");
    this->executeProcess("bitcoind", "-datadir=\"" + this->getWallet() + "\" walletlock");
}

void ProcessForm::importVanityAddressToWalltet(){
    this->updateText("Importing address to wallet..");
    this->updateText("(this could take long if your wallet is out of sync)");
    // Label has problems with spaces, so it will be "Vote:[QID]-[Oprion's letter]". i.e. "Vote:9KN-A"
    this->setVotingLabel( "Vote:" +this->getVotingQID() +"-" + this->getVotingAnswer().at(0));
    this->executeProcess("bitcoind", "-datadir=\"" + this->getWallet() + "\" importprivkey " + this->getPrivatKey() + " " + this->getVotingLabel());
}

void ProcessForm::verifyImportedKey(){
    this->updateText("Requesting account asociated to voting address..");
    this->executeProcess("bitcoind", "-datadir=\"" + this->getWallet() + "\" getaccount " + this->getVanityAddress());
}

void ProcessForm::checkWalletBalance(){
    this->updateText("Verifying wallet's balance...");
    this->executeProcess("bitcoind", "-datadir=\"" + this->getWallet() + "\" getbalance");
}

void ProcessForm::sendCoinsToAddress(QString address, double coins){
    this->updateText("Sending bitcoins...");
    this->executeProcess("bitcoind", "-datadir=\"" + this->getWallet() + "\" sendtoaddress " + address + " " + QString::number(coins,'G',9));
}

void ProcessForm::setBitcoindTxFee(double fee){
    this->updateText("Setting the network fee..");
    this->executeProcess("bitcoind", "-datadir=\"" + this->getWallet() + "\" settxfee " + QString::number(fee,'G',9));
}

bool ProcessForm::existsBitcoinConfFile(QString path){
    this->updateText("Searching for bitcoin.conf on: \n"+path);
    QFile file( path + "/bitcoin.conf" );
    return file.exists();
}
void ProcessForm::createBitcoinConfFile(QString path){
    this->updateText("Creating bitcoin.conf on: \n"+path);
    QFile confFile(path + "/bitcoin.conf");
    confFile.open(QIODevice::WriteOnly | QIODevice::Text);
    QTextStream out(&confFile);
    out << "rpcuser=bitcoinrpc\n";
    out << "rpcpassword=crypto1234\n";
    confFile.close();
}

void ProcessForm::warningMessage(QString message, bool closeForm)
{
    QMessageBox msgBox;
    msgBox.setWindowTitle(Version::getAppName().c_str());
    msgBox.setIcon(QMessageBox::Warning);
    msgBox.setText(message);
    msgBox.exec();
    if(closeForm){
        this->close();
    }
}
QString ProcessForm::getVotingQuestion() const
{
    return votingQuestion;
}

void ProcessForm::setVotingQuestion(const QString &value)
{
    votingQuestion = value;
}
QString ProcessForm::getVotingAnswer() const
{
    return votingAnswer;
}

void ProcessForm::setVotingAnswer(const QString &value)
{
    votingAnswer = value;
}
QString ProcessForm::getTransactionId() const
{
    return transactionId;
}

void ProcessForm::setTransactionId(QString value)
{
    transactionId = value;
}
bool ProcessForm::getManualVoting() const
{
    return manualVoting;
}

void ProcessForm::setManualVoting(bool value)
{
    manualVoting = value;
}
QString ProcessForm::getDeadline() const
{
    return deadline;
}

void ProcessForm::setDeadline(const QString &value)
{
    deadline = value;
}
QString ProcessForm::getVotingQID() const
{
    return votingQID;
}

void ProcessForm::setVotingQID(const QString &value)
{
    votingQID = value;
}
QString ProcessForm::getVotingLabel() const
{
    return votingLabel;
}

void ProcessForm::setVotingLabel(const QString &value)
{
    votingLabel = value;
}
double ProcessForm::getNetFee() const
{
    return netFee;
}

void ProcessForm::setNetFee(double value)
{
    netFee = value;
}


void ProcessForm::executeVotingProcess(){
    // I clear the progress
    this->ui->txtProgress->clear();
    this->ui->progressBar->setValue(0);

    //I temporally disable buttons
    this->ui->bSearch->setEnabled(false);
    this->ui->bStart->setEnabled(false);


    if(this->wallet == ""){
       if(!this->getManualVoting()){// Only request wallet on auto-voting
           if(!this->searchWallet()){
               this->warningMessage("You need a wallet to continue",false);
               return;// must select the wallet to continue
           }
       }
    }
    // Lanzo vanitygen y arranca la maquina de estados
    reconnectTryes = BITCOIND_CONNECT_MAX_TRYES;
    this->updateText("Starting voting process..");
    this->votingState = GENERATE_VANITY_ADDRESS;
    this->generateVanityAddress();
}



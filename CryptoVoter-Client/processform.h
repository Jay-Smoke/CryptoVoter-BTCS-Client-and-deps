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
#ifndef PROCESSFORM_H
#define PROCESSFORM_H

#include <QWidget>
#include <QProcess>
#include <QTimer>
#include "aboutform.h"

namespace Ui {
class ProcessForm;
}

class ProcessForm : public QWidget
{
    Q_OBJECT

public:
    static const double NET_FEE_DEFAULT = 0.01;
    explicit ProcessForm(QWidget *parent = 0);
    ~ProcessForm();
    void resetVotingProcess();

    static const int BITCOIND_CONNECT_MAX_TRYES = 5; //!< MAX RETRYES FOR ERROR HANDLING
    static const int WALLET_UNLOCK_TIME = 600; //!< Max time to have the wallet unlocked, in seconds

    //!< States, or steps of the voting sequence
    typedef enum{
        IDLE_STATE = 0,
        GENERATE_VANITY_ADDRESS,
        START_BITCOIND_SERVER,
        UNLOCK_WALLET,
        IMPORT_VANITY_ADDRESS,
        CONFIRM_PRIVATE_ADDRESS_IMPORTED,
        CHECK_WALLET_BALANCE,
        ASK_CONFIRMATION,
        SET_NET_FEE,
        TRANSFER_BTCS,
        LOCK_WALLET,
        STOP_BITCOIND_SERVER
    }VotingState_e;

    //! Bitcoin RPC error codes
    enum RPCErrorCode
    {
        //! Standard JSON-RPC 2.0 errors
        RPC_INVALID_REQUEST = -32600,
        RPC_METHOD_NOT_FOUND = -32601,
        RPC_INVALID_PARAMS = -32602,
        RPC_INTERNAL_ERROR = -32603,
        RPC_PARSE_ERROR = -32700,
        //! General application defined errors
        RPC_MISC_ERROR = -1, //! std::exception thrown in command handling
        RPC_FORBIDDEN_BY_SAFE_MODE = -2, //! Server is in safe mode, and command is not allowed in safe mode
        RPC_TYPE_ERROR = -3, //! Unexpected type was passed as parameter
        RPC_INVALID_ADDRESS_OR_KEY = -5, //! Invalid address or key
        RPC_OUT_OF_MEMORY = -7, //! Ran out of memory during operation
        RPC_INVALID_PARAMETER = -8, //! Invalid, missing or duplicate parameter
        RPC_DATABASE_ERROR = -20, //! Database error
        RPC_DESERIALIZATION_ERROR = -22, //! Error parsing or validating structure in raw format
        RPC_VERIFY_ERROR = -25, //! General error during transaction or block submission
        RPC_VERIFY_REJECTED = -26, //! Transaction or block was rejected by network rules
        RPC_VERIFY_ALREADY_IN_CHAIN = -27, //! Transaction already in chain
        RPC_IN_WARMUP = -28, //! Client still warming up
        //! Aliases for backward compatibility
        RPC_TRANSACTION_ERROR = RPC_VERIFY_ERROR,
        RPC_TRANSACTION_REJECTED = RPC_VERIFY_REJECTED,
        RPC_TRANSACTION_ALREADY_IN_CHAIN= RPC_VERIFY_ALREADY_IN_CHAIN,
        //! P2P client errors
        RPC_CLIENT_NOT_CONNECTED = -9, //! Bitcoin is not connected
        RPC_CLIENT_IN_INITIAL_DOWNLOAD = -10, //! Still downloading initial blocks
        RPC_CLIENT_NODE_ALREADY_ADDED = -23, //! Node is already added
        RPC_CLIENT_NODE_NOT_ADDED = -24, //! Node has not been added before
        //! Wallet errors
        RPC_WALLET_ERROR = -4, //! Unspecified problem with wallet (key not found etc.)
        RPC_WALLET_INSUFFICIENT_FUNDS = -6, //! Not enough funds in wallet or account
        RPC_WALLET_INVALID_ACCOUNT_NAME = -11, //! Invalid account name
        RPC_WALLET_KEYPOOL_RAN_OUT = -12, //! Keypool ran out, call keypoolrefill first
        RPC_WALLET_UNLOCK_NEEDED = -13, //! Enter the wallet passphrase with walletpassphrase first
        RPC_WALLET_PASSPHRASE_INCORRECT = -14, //! The wallet passphrase entered was incorrect
        RPC_WALLET_WRONG_ENC_STATE = -15, //! Command given in wrong wallet encryption state (encrypting an encrypted wallet etc.)
        RPC_WALLET_ENCRYPTION_FAILED = -16, //! Failed to encrypt the wallet
        RPC_WALLET_ALREADY_UNLOCKED = -17, //! Wallet is already unlocked
    };

    //!< Getters and Setters
    QString getVotingPrefix() const;
    void setVotingPrefix(const QString &value);
    QString getWallet() const;
    void setWallet(const QString &value);
    QString getWalletPasswd() const;
    void setWalletPasswd(const QString &value);
    QString getVanityAddress() const;
    void setVanityAddress(const QString &value);
    QString getPrivatKey() const;
    void setPrivatKey(const QString &value);
    QString getVotingAddress() const;// If all goes right, it should be the same as getVanityAddress()
    void setVotingAddress(const QString &value);
    double getBtcs() const;
    void setBtcs(double value);

    /*!
     * \brief It overrides the QWidget's show() method to start the voting sequence.
     */
    virtual void show();

    void warningMessage(QString message, bool closeForm);
    QString getVotingQuestion() const;
    void setVotingQuestion(const QString &value);
    QString getVotingAnswer() const;
    void setVotingAnswer(const QString &value);
    QString getTransactionId() const;
    void setTransactionId(QString value);

    bool getManualVoting() const;
    void setManualVoting(bool value);

    QString getDeadline() const;
    void setDeadline(const QString &value);

    QString getVotingQID() const;
    void setVotingQID(const QString &value);

    QString getVotingLabel() const;
    void setVotingLabel(const QString &value);

    double getNetFee() const;
    void setNetFee(double value);

private:
    Ui::ProcessForm *ui;
    VotingState_e votingState;
    QProcess *proc;
    QProcess *bitcoind;
    bool    manualVoting;
    QString votingQuestion;
    QString votingQID;
    QString votingAnswer;
    QString votingPrefix;
    QString votingLabel;
    QString deadline;
    QString wallet;
    QString walletPasswd;
    QString vanityAddress;
    QString privatKey;
    QString votingAddress;
    double  walletBalance;
    double  btcsToTransfer;
    double  netFee;
    QString transactionId;
    int reconnectTryes;
    QTimer retryTmr;
    AboutForm manualInstrDialog;


    /*!
     * \brief It executes a command on a separate process, coneccting the finished signal
     *        to the votingStateMachine slot. If is there another proces executing it terminates it first.
     *
     * \param process command line string
     * \param args arguments as string
     */
    void executeProcess(QString process, QString args);

    /*!
     * \brief It reads the merged output (stdout + stderr) of the last executes process.
     *        It appends the output text to the status textbox on the GUI, and also returns it as a astring.
     * \return the output of the last executed process
     */
    QString readProcessOutput();

    /*!
     * \brief It appends a string to the status text box on the GUI.
     * \param txt
     */
    void updateText(QString txt);

    /*!
     * \brief It generates the vanity address using vanitygen, with the setted prefix.
     */
    void generateVanityAddress();

    /*!
     * \brief It starts the bitcoind server on a separate process.
     *        It fires a timer of 60 seconds to wait for the process initialization.
     *        Then the timer calls the votingStateMachine() slot.
     */
    void startBitcoindServer();

    /*!
     * \brief It locks the wallet and then stops the bitcoind server.
     */
    void stopBitcoindServer();

    /*!
     * \brief It unlocks the wallet for 5 minutes
     */
    bool unlockWallet();

    /*!
     * \brief It locks the wallet.
     */
    void lockWallet();

    /*!
     * \brief It imports the generates vanitygen address and private key to the wallet.
     */
    void importVanityAddressToWalltet();

    /*!
     * \brief It returns the public address from the vanitygen address and private key
     */
    void verifyImportedKey();

    /*!
     * \brief It gets the wallet's balance and stores it for further usage.
     */
    void checkWalletBalance();

    /*!
     * \brief It makes the bitcoins transference using the generated public address.
     * \param address bitcoin public address ans string
     * \param coins amount to transfer
     */
    void sendCoinsToAddress(QString address, double coins);

    /*!
     * \brief It sets the network fee.
     */
    void setBitcoindTxFee(double fee);

    /*!
     * \brief It checks if the file bitcoin.conf exists.
     * \param path of the directory to search on.
     * \return true if bitcoin.conf exists on wallet's directory, false otherwise.
     */
    bool existsBitcoinConfFile(QString path);

    /*!
     * \brief It creates a default bitcoin.conf file with the user and password needed.
     * \param path of the directory where the file will be created.
     */
    void createBitcoinConfFile(QString path);

public slots:
    /*!
     * \brief It opens a browse window to select the wallet's folder.
     * \return true if a folder was selected, false if cancel was pressed.
     */
    bool searchWallet();

    /*!
     * \brief This slot is called by a timer 60 seconds after bitcoind process has started.
     */
    void bitcoindInitTimeFinished();

    /*!
     * \brief This slot retryes the connnection with bitcoind server.
     */
    void retryBitcoindConnection();

    /*!
     * \brief This slot implements a state machine for the voting sequence.
     * \param exitCode exit code of the las executed process. 0 = OK, other = error.
     * \param exitStatus exit code of the las executed process. Normal or Crash.
     */
    void votingStateMachine(int exitCode, QProcess::ExitStatus exitStatus);

    /*!
     * \brief abortVotingSequence cancels the current voting sequence, locking the
     *        wallet and stopping the bitcoind server process.
     */
    void abortVotingSequence();

    /*!
     * \brief It starts the voting sequence running vanitygen to start the state machine.
     */
    void executeVotingProcess();
};

#endif // PROCESSFORM_H

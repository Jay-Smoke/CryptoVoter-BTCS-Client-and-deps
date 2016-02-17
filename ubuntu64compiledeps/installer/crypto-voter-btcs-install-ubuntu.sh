echo deleting previous installations...
rm -rf /opt/cryptoVoter-btcs
rm /usr/bin/vanitygen 
rm /usr/bin/keyconv 
rm /usr/bin/bitcoind
rm /usr/bin/cryptoVoter-btcs

echo installing dependecies...
apt-get install libssl-dev libpcre3-dev sqlite3
#git clone https://github.com/samr7/vanitygen
cd vanitygen
make clean
make
cd ..
echo copying files..
mkdir /opt/cryptoVoter-btcs
mkdir /opt/cryptoVoter-btcs/transactions/
cp ./cryptoVoter-btcs /opt/cryptoVoter-btcs/
cp ./vanitygen/vanitygen /opt/cryptoVoter-btcs/
cp ./vanitygen/keyconv /opt/cryptoVoter-btcs/
cp ./bitcoind/bitcoind /opt/cryptoVoter-btcs/
cp ./start_crypto_voter.sh /opt/cryptoVoter-btcs/
cp ./bitcoinBlue.png /opt/cryptoVoter-btcs/
chmod +x /opt/cryptoVoter-btcs/start_crypto_voter.sh
chmod +x /opt/cryptoVoter-btcs/cryptoVoter-btcs
chmod +x /opt/cryptoVoter-btcs/vanitygen
chmod +x /opt/cryptoVoter-btcs/keyconv
chmod +x /opt/cryptoVoter-btcs/bitcoind
chmod 777 /opt/cryptoVoter-btcs/transactions/

#Copying libraries
cp -rf platforms /opt/cryptoVoter-btcs/
cp -rf portable /opt/cryptoVoter-btcs/
cp -rf sqldrivers /opt/cryptoVoter-btcs/
cp  about.txt /opt/cryptoVoter-btcs/
cp cryptovoter.ini /opt/cryptoVoter-btcs/
cp manualvoting.txt /opt/cryptoVoter-btcs/
cp automaticvoting.txt /opt/cryptoVoter-btcs/
cp crypto.css /opt/cryptoVoter-btcs/
cp eula.txt /opt/cryptoVoter-btcs/
cp copyright.txt /opt/cryptoVoter-btcs/
chmod  oug+wr /opt/cryptoVoter-btcs/cryptovoter.ini
chmod  oug+wr /opt/cryptoVoter-btcs/manualvoting.txt
chmod  oug+wr /opt/cryptoVoter-btcs/automaticvoting.txt
chmod oug+wr /opt/cryptoVoter-btcs/about.txt
chmod oug+wr /opt/cryptoVoter-btcs/eula.txt
cp cryptoVoter-btcs.conf /etc/ld.so.conf.d/


#making links
ln -s /opt/cryptoVoter-btcs/vanitygen /usr/bin/vanitygen
ln -s /opt/cryptoVoter-btcs/keyconv /usr/bin/keyconv
ln -s /opt/cryptoVoter-btcs/bitcoind /usr/bin/bitcoind
ln -s /opt/cryptoVoter-btcs/start_crypto_voter.sh /usr/bin/cryptoVoter-btcs
ldconfig

mkdir -p ~/.local/share/applications/
chmod oug+wr -R ~/.local/share/applications/
cp cryptoVoter-btcs.desktop ~/.local/share/applications/

echo installation finished!

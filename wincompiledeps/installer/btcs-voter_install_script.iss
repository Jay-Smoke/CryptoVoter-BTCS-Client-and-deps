
[Files]
DestDir: {app}; Source: ..\CryptoVoter-port32\*; Flags: recursesubdirs
#define VERSION "1.10.1" 
[Setup]
AppCopyright=Smoke Easy.
AppName=cryptoVoter-btcs
AppVerName= cryptoVoter-btcs v{#VERSION}
AppVersion={#VERSION}
DefaultDirName={pf}\cryptoVoter-btcs
DefaultGroupName=\cryptoVoter-btcs
UninstallDisplayIcon={app}\appicon.ico
OutputBaseFilename=cryptoVoter-btcs-setup
[Icons]
Name: {group}\cryptoVoter-btcs; Filename: {app}\cryptoVoter-btcs.exe; IconIndex: 0; Flags: useapppaths; WorkingDir: {app}; Components: ; Tasks:
Name: {group}\{cm:UninstallProgram, Emantcom}; Filename: {uninstallexe}
Name: {commondesktop}\cryptoVoter-btcs; Filename: {app}\cryptoVoter-btcs.exe; IconIndex: 0; Flags: useapppaths; Tasks: desktopicon; WorkingDir: {app}
[Tasks]
Name: desktopicon; Description: Create a &desktop icon; GroupDescription: Additional icons:
[Messages]
UninstalledAll= cryptoVoter-btcs was successfully uninstalled.


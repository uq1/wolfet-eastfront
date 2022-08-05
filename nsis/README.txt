etpub.nsi is a script for use with the NSIS 
(Nullsoft Scriptable Install System) http://nsis.sf.net/

It creates a setup.exe file that will be distributed inside the 
win32 directory of the etpub distribution.

This executable does not actually contain any etpub files, it just
copies them from the relative path to the setup.exe file.  The files
it copies are:

	qagame_mp_x86.dll
	../docs/Example/Commented/server.cfg
	../docs/Example/Commented/default.cfg
	../docs/Example/Commented/shrubbot.cfg

These files are copied to the 'etpub' directory next to the ET.exe 
executable on the system.  The configuration files will NOT be installed
by default if there isalready an etpub directory there.  It is still
an option though.

Also created is a etpub.lnk next to ET.exe that is set to start ET.exe as
a dedicated etpub server using the following arguments:
"+set dedicated 2 +set fs_game etpub +exec server.cfg"

The installer also has options to create a Desktop Icon and Start Menu
items.  The Start Menu items include:
	"Start etpub"
	"Edit server.cfg"
	"Edit default.cfg"
	"Edit shrubbot.cfg"
	"uninstall"



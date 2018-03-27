#!/bin/bash
YEAR=`date +%Y`
cat > COPYRIGHT << EOF
ScummVM
Copyright (C) 2001-$YEAR by the following:

If you have contributed to this project then you deserve to be on this
list.  Contact us (see: AUTHORS) and we'll add you.

EOF

CONVERT_ACCENTS="s/&aacute;/a/g
s/&eacute;/e/g
s/&iacute;/i/g
s/&igrave;/i/g
s/&oacute;/o/g
s/&oslash;/o/g
s/&uacute;/u/g
s/&#261;/a/g
s/&#321;/L/g
s/&#322;/l/g
s/&#347;/s/g
s/&Scaron;/S/g
s/&aring;/aa/g
s/&ntilde;/n/g
s/&auml;/a/g
s/&euml;/e/g
s/&uuml;/ue/g
s/Torbj&ouml;rn/Torbjorn/g
s/&ouml;/oe/g"

GIT_AUTHORS=`git shortlog -s | cut -c8-`\n'
Laura Abbott "sageofminerva" [Abbott]
Vikram Aggarwal "youngelf" [Aggarwal]
Dieter Baron "dillo" [Baron]
Alban Bedel "albeu" [Bedel]
Bodo Bellut "bellut"
Andreas Bierfert "awjb" [Bierfert]
Kaustav Biswas
Elio Blanca "eblanca76" [Blanca]
David Breakey "dbreakey" [Breakey]
Michael du Breuil "WickedShell" [Breuil]
Robert Buchholz "prendi" [Buchholz]
Mathieu Carot "yokna" [Carot]
Stefano Ceccherini "jackburton" [Ceccherini]
Travis S Coady "theealien" [Coady]
Josh Coalson "jcoalson" [Coalson]
Thomas Combeleran "hibernatus" [Combeleran]
Kees Cook "keescook" [Cook]
Carlos Corbacho "cathectic" [Corbacho]
Roberto Costa "fiix76" [Costa]
Alexander Dergunov
Roman Donchenko
Matthew Duggan "stauff1" [Duggan]
Barry Duncan
Olivier Duverne "richiefs" [Duverne]
Andrei Dziahel "develop7" [Dziahel]
John Eckerdal "johneck" [Eckerdal]
Mike Frysinger "vapier" [Frysinger]
Bence Gazder
Chris Gelatt "kreeblah" [Gelatt]
Jens Georg "phako" [Georg]
Nicolas George "cigaes" [George]
Jonathan Gevaryahu "lord_nightmare" [Gevaryahu]
Boris Gnezdilov
Tobias Gruetzmacher "tobig" [Gruetzmacher]
Damien Guard "damienguard" [Guard]
Matti Hamalainen "ccrtnsp" [Hamalainen]
Stefan Haubenthal "polluks" [Haubenthal]
Alexander Holler "holler"
Falk Hueffner "mellum" [Hueffner]
Casey Hutchinson "nnooiissee" [Hutchinson]
Gregor Jasny "gjasny" [Jasny]
Matt Johnson "mattjon" [Johnson]
Nicolas Joly "njoly" [Joly]
Sam Kenny "sam_k" [Kenny]
Koen Kooi "koenkooi" [Kooi]
Zygmunt Krynicki "zygoon" [Krynicki]
Janne Kujanpaa "jukuja" [Kujanpaa]
Jay Lanagan "r0ni" [Lanagan]
Manuel Lauss "mlau2" [Lauss]
Rolf Leggewie "leggewie"
Duncan Lock "dflock" [Lock]
Mark Lodato "itsr0y" [Lodato]
Fridvin Logi "phillip_j_fry" [Logi]
Michael Lojkovic [Lojkovic]
Georg Lukas "ge0rg" [Lukas]
Artem Lukoyanov [Lukoyanov]
Michael Madsen "pidgeot" [Madsen]
Dmitry Marakasov "amdmi3" [Marakasov]
Connor McLeod "mcleod2032"
Mickey McMurray "metafox" [McMurray]
Adam Metcalf "gamblore" [Metcalf]
Frank Meyering "frank_m24" [Meyering]
Gael Le Migno "kilobug" [Migno]
Andy Molloy "maloi" [Molloy]
Armin Mueller "arm_in" [Mueller]
Andrea Musuruane "musuruan" [Musuruane]
KO Myung-Hun "lvzuufx" [Myung-Hun]
Markus Napp "meist3r" [Napp]
Peter Naulls "pnaulls" [Naulls]
Christian Neumair "mannythegnome" [Neumair]
Markus Niemisto "niemisto"
Julian Ospald
Chris Paras "paras_rasmatazz"
Aubin Paul "outlyer" [Paul]
Carsten Pohl "carstenpohl" [Pohl]
Markus Pyykko "mankeli" [Pyykko]
Thomas Richter "thorfdbg" [Richter]
Felix Riemann "kirschsaft" [Riemann]
Timo Roehling "t1m0" [Roehling]
Jonathan Rogers "jonner" [Rogers]
Marek Roth "logicdeluxe" [Roth]
Uwe Ryssel "uweryssel" [Ryssel]
Florian Schmitt "fatpenguin" [Schmitt]
Mark Schreiber "mark7" [Schreiber]
Ben Shadwick "benshadwick" [Shadwick]
Rodrigo Silva
Andrej Sinicyn "andrej4000" [Sinicyn]
Steve Stavropoulos "isnothere" [Stavropoulos]
Daniel Steinberger "amorphousshape" [Steinberger]
Sven Strothoff "dataslayer" [Strothoff]
Andrea Suatoni "mrhandler" [Suatoni]
Max Tabachenko [Tabachenko]
Luigi Toscano "ltosky" [Toscano]
Xavier Trochu "xtrochu" [Trochu]
Michal Tulacek "tutchek" [Tulacek]
Michael Udaltsov "cccp99" [Udaltsov]
Kristof Vansant "lupusbe" [Vansant]
Tim Walters "realmz" [Walters]
Eric A. Welsh "eweish42" [Welsh]
Yudhi Widyatama "yudhi97" [Widyatama]
Kieron Wilkinson
Robert Wohlrab "moshroum" [Wohlrab]
Grant Yeager "glo_kidd" [Yeager]
Benjamin W. Zale "junior_aepi" [Zale]
Bramvandijk "bramvandijk"
Damien
dc france "erwan2004"
dewt "mncl"
dubsdj
Florent "flobo"
Florob "florob"
j0tt
Jellby "jellby"
Joerg "macdrega"
Lostech "lostech"
Nicos "anarxia"
ole
phi1
Pix2 "pix2"
Richard "trinity78"
Scarlatti "escarlate"
the rara avis "theraraavis"
Tim "tipabu"
Xanathar "xanathar"
'
GIT_AUTHORS=`echo -e "$GIT_AUTHORS" | iconv --to-code=ASCII//TRANSLIT | sed 's/\s/ /g' | awk 'BEGIN {FS=" "; OFS="|"}{print $NF,$0}' | sort -u -f -t'|' -k1 | awk -F "|" '{print $NF}' | sed "s/ \[[a-zA-Z-]*\]//g"`

IFS=$'\t\r\n'
CREDITS_AUTHORS=
CREDITS_AUTHORS_NAMES=
CREDITS_AUTHORS_NICKNAMES=
for i in `grep add_person\( devtools/credits.pl | sed -r "$CONVERT_ACCENTS" | sed 's/\s/ /g'`; do
	name=`echo "$i" | cut -f2 -d'"'`
	nickname=`echo "$i" | cut -f4 -d'"'`
	CREDITS_AUTHORS_NAMES="$CREDITS_AUTHORS_NAMES	$name"
	CREDITS_AUTHORS_NICKNAMES="$CREDITS_AUTHORS_NICKNAMES	$nickname"
	if [[ "$name" == "" ]]; then
		CREDITS_AUTHORS="$CREDITS_AUTHORS\n$nickname"
	else
		CREDITS_AUTHORS="$CREDITS_AUTHORS\n$name"
	fi
done
CREDITS_AUTHORS=`echo -e "$CREDITS_AUTHORS" | awk 'BEGIN {FS=" "; OFS="|"}{print $NF,$0}' | sort -u -f -t'|' -k1 | awk -F "|" '{print $NF}'`
for i in $CREDITS_AUTHORS; do
	echo $i >> COPYRIGHT
done

cat >> COPYRIGHT << EOF


Patches contributed by:

EOF

for i in $GIT_AUTHORS; do
	if [[ "`echo $CREDITS_AUTHORS_NAMES | grep ""$i""`" == "" ]]; then
		if [[ "`echo $CREDITS_AUTHORS_NICKNAMES |  grep -i ""$i""`" == "" ]]; then
			echo $i >> COPYRIGHT
		fi
	fi
done

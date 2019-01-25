rm -f systeminfo.txt
echo username $(whoami)>>systeminfo.txt
echo >>systeminfo.txt
echo Operating System Type/Brand>>systeminfo.txt
cat /etc/*-release>>systeminfo.txt
echo >>systeminfo.txt
echo Kernel Version>>systeminfo.txt
uname -r>>systeminfo.txt
echo >>systeminfo.txt
cat /proc/version>>systeminfo.txt
echo >>systeminfo.txt
echo architecture $(arch)>>systeminfo.txt
echo >>systeminfo.txt
echo Information on File System Memory>>systeminfo.txt
free>>systeminfo.txt


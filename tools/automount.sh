#!/bin/sh
if [ $(mount | grep 'production@192.168.1.2:OS_IMAGE\' | wc -l) -ne 1 ]
then
	echo 'PASSWORD' | sshfs production@192.168.1.2:OS_IMAGE /home/partimag -o password_stdin,allow_other
    echo 'SSHFS has been mounted.'
else
    echo 'SSHFS is already mounted.'
fi

#! /bin/bash
# Utility script for cleaning up snapshots and removing modules.
#
# Copyright (C) 2001  Cluster File Systems, Inc.
#
# This code is issued under the GNU General Public License.
# See the file COPYING in this distribution

OBDDIR="`dirname $0`/.."
. $OBDDIR/demos/config.sh

plog umount $MNTOBD
plog umount $MNTSNAP

plog log "CLEANUP /dev/obd2 /dev/obd1"
$OBDDIR/class/obdcontrol -f << EOF
device /dev/obd2
cleanup
detach
device /dev/obd1
cleanup
detach
quit
EOF

rmmod obdsnap

rm $SNAPTABLE
$OBDDIR/demos/obdfsclean.sh

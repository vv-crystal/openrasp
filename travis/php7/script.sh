#!/bin/bash
set -ev

ulimit -c unlimited -S
mkdir -p /tmp/coredump
sudo bash -c "echo '/tmp/coredump/core.%p.%E' > /proc/sys/kernel/core_pattern"

pushd agent/$OPENRASP_LANG
php run-tests.php -p `which php` -d extension=`pwd`/modules/openrasp.so --offline --show-diff --set-timeout 120
popd
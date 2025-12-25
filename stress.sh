#!/bin/sh

# Paranoid Stress Test for uwsh
# Verification of core shell functionality after porting.

echo "Starting Paranoid Stress Test..."

# 1. Loop and Command Substitution
echo "Testing loops and cmdsubst..."
COUNT=0
while [ $COUNT -lt 100 ]; do
    VAR=`echo $COUNT`
    if [ "$VAR" != "$COUNT" ]; then
        echo "FAILED: Loop mismatch at $COUNT"
        exit 1
    fi
    COUNT=`expr $COUNT + 1`
done
echo "Pass: Loops and cmdsubst"

# 2. Redirection and Pipes
echo "Testing pipes and redirections..."
echo "hello world" | grep hello > /tmp/uwsh_test_out
if [ "`cat /tmp/uwsh_test_out`" != "hello world" ]; then
    echo "FAILED: Pipe/Redirection mismatch"
    rm /tmp/uwsh_test_out
    exit 1
fi
rm /tmp/uwsh_test_out
echo "Pass: Pipes and redirections"

# 3. Heredocs
echo "Testing heredocs..."
cat <<EOF > /tmp/uwsh_heredoc
line 1
line 2
$PATH
EOF
if [ ! -f /tmp/uwsh_heredoc ]; then
    echo "FAILED: Heredoc file not created"
    exit 1
fi
rm /tmp/uwsh_heredoc
echo "Pass: Heredocs"

# 4. Traps
echo "Testing traps..."
trap 'echo "Caught SIGUSR1" > /tmp/uwsh_trap_caught' USR1
(sleep 1; kill -USR1 $$) &
# Wait for some time to allow the signal to be processed
sleep 2
if [ ! -f /tmp/uwsh_trap_caught ]; then
    echo "FAILED: Trap not caught"
    rm -f /tmp/uwsh_trap_caught
    exit 1
fi
rm -f /tmp/uwsh_trap_caught
echo "Pass: Traps"

# 5. Functions
echo "Testing functions..."
myfunc() {
    echo "inside function: $1"
}
RES=`myfunc "hello"`
if [ "$RES" != "inside function: hello" ]; then
    echo "FAILED: Function execution mismatch"
    exit 1
fi
echo "Pass: Functions"

echo "All tests passed!"

#!/usr/bin/expect -f

# Client for connection to the running daemon and sending strings

spawn telnet localhost 5000
set timeout 600

expect {
    "Connection refused" exit
    "Escape character"
}

set command0 [lindex $argv 0];
set command1 [lindex $argv 1];
set command2 [lindex $argv 2];

send "$command0\n"
expect {
    sleep 5
    timeout { puts "the answer hasn't arrived\n"; exit }
    "test"
}
send "$command1\n"
expect {
    sleep 5
    timeout { puts "the answer hasn't arrived\n"; exit }
    "test"
}
send "$command2\n"
expect {
    sleep 5
    timeout { puts "the answer hasn't arrived\n"; exit }
    "test"
}
exit

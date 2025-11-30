# ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;; #
#                                                                              #
#                        ______                                                #
#                     .-"      "-.                                             #
#                    /            \                                            #
#        _          |              |          _                                #
#       ( \         |,  .-.  .-.  ,|         / )                               #
#        > "=._     | )(__/  \__)( |     _.=" <                                #
#       (_/"=._"=._ |/     /\     \| _.="_.="\_)                               #
#              "=._ (_     ^^     _)"_.="                                      #
#                  "=\__|IIIIII|__/="                                          #
#                 _.="| \IIIIII/ |"=._                                         #
#       _     _.="_.="\          /"=._"=._     _                               #
#      ( \_.="_.="     `--------`     "=._"=._/ )                              #
#       > _.="                            "=._ <                               #
#      (_/                                    \_)                              #
#                                                                              #
#      Filename: test_ping.py                                                  #
#      By: espadara <espadara@pirate.capn.gg>                                  #
#      Created: 2025/11/30 17:21:55 by espadara                                #
#      Updated: 2025/11/30 17:34:57 by espadara                                #
#                                                                              #
# ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;; #

#!/usr/bin/env python3
import subprocess
import signal
import time
import re
import sys
import os

# Colors for output
GREEN = '\033[92m'
RED = '\033[91m'
RESET = '\033[0m'
BOLD = '\033[1m'

FT_PING = "./ft_ping"
NEEDS_SUDO = True

def print_status(test_name, passed, message=""):
    if passed:
        print(f"{GREEN}[PASS] {test_name}{RESET} {message}")
    else:
        print(f"{RED}[FAIL] {test_name}{RESET} {message}")

def run_ping(args, duration=2, expect_fail=False):
    """
    Runs ft_ping, waits 'duration' seconds, sends SIGINT, and captures output.
    """
    cmd = [FT_PING] + args
    if NEEDS_SUDO and os.geteuid() != 0:
        cmd = ["sudo"] + cmd

    try:
        # Start process
        proc = subprocess.Popen(
            cmd,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            text=True
        )

        # Wait for packets to flow
        time.sleep(duration)

        # Send SIGINT (Ctrl+C) to stop it gracefully and get stats
        proc.send_signal(signal.SIGINT)

        try:
            stdout, stderr = proc.communicate(timeout=1)
        except subprocess.TimeoutExpired:
            proc.kill()
            stdout, stderr = proc.communicate()
            return False, "Process hung after SIGINT", stdout, stderr

        if expect_fail:
            if proc.returncode != 0:
                return True, "Exited with error as expected", stdout, stderr
            else:
                return False, "Expected failure but got success", stdout, stderr

        if proc.returncode != 0:
            return False, f"Exited with code {proc.returncode}", stdout, stderr

        return True, "Ran successfully", stdout, stderr

    except Exception as e:
        return False, str(e), "", ""

def test_basic_localhost():
    print(f"\n{BOLD}--- Test: Basic Localhost ---{RESET}")
    passed, msg, out, err = run_ping(["127.0.0.1"], duration=2)

    # Check for essential output
    if "bytes from 127.0.0.1" in out and "ping statistics" in out:
        print_status("Basic Loopback", True)
    else:
        print_status("Basic Loopback", False, f"Output missing key phrases.\nOutput:\n{out}")

def test_ttl_flag():
    print(f"\n{BOLD}--- Test: TTL Flag ---{RESET}")
    # TTL 1 should die immediately
    passed, msg, out, err = run_ping(["--ttl", "1", "8.8.8.8", "-v"], duration=2)

    # We expect "Time to live exceeded" or "Time Exceeded"
    # Your implementation prints: "type=11 code=0"
    if "type=11" in out or "Time Exceeded" in out:
        print_status("TTL Expiry Detected", True)
    else:
        print_status("TTL Expiry Detected", False, f"Did not see TTL error.\nOutput:\n{out}")

def test_deadline_flag():
    print(f"\n{BOLD}--- Test: Deadline (-w) ---{RESET}")
    start = time.time()
    # Should run for 3 seconds then exit automatically
    cmd = [FT_PING, "-w", "3", "127.0.0.1"]
    if NEEDS_SUDO: cmd = ["sudo"] + cmd

    proc = subprocess.run(cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    end = time.time()

    elapsed = end - start
    # Allow small buffer for overhead
    if 2.9 <= elapsed <= 4.1:
        print_status("Deadline Timing", True, f"(Took {elapsed:.2f}s)")
    else:
        print_status("Deadline Timing", False, f"(Expected ~3s, took {elapsed:.2f}s)")

def test_errors():
    print(f"\n{BOLD}--- Test: Error Handling ---{RESET}")

    # 1. No arguments
    passed, msg, out, err = run_ping([], duration=0.1, expect_fail=True)
    if "usage error" in out or "usage error" in err:
        print_status("No Args", True)
    else:
        print_status("No Args", False, "Did not report usage error")

    # 2. Bad hostname
    # Note: getaddrinfo usually returns error, program should exit
    passed, msg, out, err = run_ping(["this.domain.does.not.exist.at.all"], duration=0.5, expect_fail=True)
    if "unknown host" in out or "Name or service not known" in out or "gai_strerror" in out: # Adapt string match to your error msg
        print_status("Bad Hostname", True)
    else:
        print_status("Bad Hostname", False, f"Output: {out}")

def test_help():
    print(f"\n{BOLD}--- Test: Help Flag (-?) ---{RESET}")
    cmd = [FT_PING, "-?"]
    if NEEDS_SUDO: cmd = ["sudo"] + cmd
    res = subprocess.run(cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)

    if "Usage:" in res.stdout:
        print_status("Help Output", True)
    else:
        print_status("Help Output", False)

def main():
    print(f"{BOLD}🐙 ft_ping Automated Test Suite 🐙{RESET}")

    if not os.path.exists(FT_PING):
        print(f"{RED}Error: {FT_PING} not found. Run make first.{RESET}")
        sys.exit(1)

    test_basic_localhost()
    test_ttl_flag()
    test_deadline_flag()
    test_errors()
    test_help()

if __name__ == "__main__":
    main()

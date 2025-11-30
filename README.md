
# ⚓ ft_ping - The Unsinkable Packet Cannon


**ft_ping** is a high-performance, memory-optimized implementation of the standard `ping` command, built from scratch in C. It uses the custom **KrakenLib** to navigate the treacherous waters of the ICMP protocol.

Unlike standard implementations that might leak memory like a sieve, this ship is built with a **Zero-Malloc Philosophy** inside the main loop, ensuring it can run for years without consuming a single extra byte of RAM.

---

## 🏴‍☠️ Features

### Mandatory Loot
* **ICMP Echo Requests:** Sends and receives standard ping packets (Type 8 / Type 0).
* **Precision Timing:** Calculates RTT (Round-Trip Time) with microsecond accuracy.
* **DNS Resolution:** Resolves FQDNs (like `google.com`) to IP addresses.
* **Signal Handling:** Catches `SIGINT` (Ctrl+C) to display summary statistics before docking.
* **Statistics:** meaningful math including Min, Max, Average, and Standard Deviation (mdev).

### The Treasure (Bonuses)
* **🌊 Flood Ping (`-f`):** Fires packets as fast as the hardware allows. Prints `.` on send and `\b` on receive to visualize network load.
* **⏳ Deadline (`-w <sec>`):** Automatically stops the operation after N seconds.
* **🧭 Time-To-Live (`--ttl <val>`):** Manually sets the IP TTL field to map network paths or simulate errors.
* **🗣️ Verbose (`-v`):** Displays detailed info for non-Echo-Reply packets (errors, timeouts).

---

## 🛠️ The Architecture: "Zero-Malloc Loop"

This project adheres to a strict **Optimized Memory Philosophy**:

1.  **Stack-Based Packets:** Both sending and receiving buffers are allocated on the stack. No `malloc` is ever called inside the infinite loop.
2.  **Stateless Math:** Standard deviation is calculated using **Welford’s Online Algorithm** (sum of squares), meaning we don't need to store an array of previous RTTs.
3.  **Global Singleton:** A single global pointer allows signal handlers to access statistics without dynamic allocation.

**Result:** A program that is incredibly fast, cache-friendly, and has **zero memory leaks**.

---

## 🚀 Installation

Ensure you have `make` and `gcc` (or `clang`) installed.

```bash
# 1. Clone the repository (and the Kraken)
git clone [https://github.com/espadara/ft_ping.git](https://github.com/espadara/ft_ping.git)
cd ft_ping

# 2. Build the ship
make
````

*Note: This will automatically pull and compile `krakenlib`.*

-----

## 💻 Usage

Because `ft_ping` uses **RAW Sockets**, you must run it with **root privileges** (`sudo`).

```bash
sudo ./ft_ping [OPTIONS] <HOST>
```

### Examples

**Standard Ping:**

```bash
sudo ./ft_ping google.com
```

**Flood Ping (Stress Test):**

```bash
# Warning: This fires hundreds/thousands of packets per second!
sudo ./ft_ping -f 127.0.0.1
```

**Timed Run (3 Seconds):**

```bash
sudo ./ft_ping -w 3 8.8.8.8
```

**Network Mapping (TTL Test):**

```bash
# Packets die after 1 hop, showing your gateway
sudo ./ft_ping --ttl 1 -v 8.8.8.8
```

-----

## 📊 Output Comparison

**ft\_ping:**

```text
PING 8.8.8.8 (8.8.8.8): 56 data bytes
64 bytes from 8.8.8.8: icmp_seq=1 ttl=117 time=5.41 ms
64 bytes from 8.8.8.8: icmp_seq=2 ttl=117 time=5.63 ms
^C
--- 8.8.8.8 ping statistics ---
2 packets transmitted, 2 packets received, 0% packet loss
round-trip min/avg/max/stddev = 5.416/5.526/5.636/0.110 ms
```

-----

## 📜 License

This project was built for the 42 School curriculum.
Licensed under the **Kraken License**: *Take what you can, give nothing back.* (Just kidding, it's open source).

<div align="center"\>
<b\>Made with 💀 and C by espadara\</b\>
</div\>


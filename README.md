Subnet calculator game developed while being a student of the HNC NextGen Computing at NCL Coatbridge Campus.
Excellent to improve your ability to calculate subnets on the spots - highly recommended to everyone interested in Networking!
v.1.0 - 11/06/2026
# Subnet Trainer v1.0

A fast-paced, interactive subnetting quiz game developed during my studies in the **HNC NextGen Computing** at **New College Lanarkshire (NCL), Coatbridge Campus**. 

This desktop application is designed to sharpen your ability to calculate IPv4 subnet boundaries completely on the spot—making it an excellent training tool for anyone preparing for networking assessments, CCNA studies, or building a solid foundation in infrastructure.

---

## Features

* **Dynamic Question Generation:** Challenges you with random IP addresses and varying CIDR masks (e.g., `/22`, `/25`, `/26`) to ensure a diverse practice routine.
* **Granular Speed Bonus Engine:** Rewards rapid-fire math! Includes a highly sensitive, tiered XP scoring system based strictly on how fast you solve *each individual challenge*:
  * 👑 **Subnet God Mode** (≤ 20s): +120 XP bonus
  * 🔥 **Elite Speed** (≤ 40s): +100 XP bonus
  * ⚡ **Great Pace** (≤ 1min): +80 XP bonus
  * ...and baseline points for safe finishes under the 5-minute exam threshold!
* **True Accuracy Tracking:** Keeps an honest tally of your attempts (`Solved: 3/4`) so you can pinpoint exactly when a mistyped octet holds you back.
* **Persistent Hall of Fame:** Automatically parses and tracks match histories locally via JSON to maintain a local Top 10 high-score leaderboard.
* **Sleek Dark Mode UI:** Designed with a modern, high-contrast terminal style using custom Qt Style Sheets (QSS) for a comfortable developer experience.

---

## Built With

* **C++17** (Standard `<chrono>` libraries for precision timing mechanics)
* **Qt Framework** (Widgets, Core, and GUI Engine)
* **JSON Layer** (For persistent database state engine tracking)

---

## How to Run

Because the core engine is built with Qt, it runs natively on both **Linux** and **Windows**!

### For Windows Users (Easiest Method)
1. Download the compressed `.zip` distribution folder provided in the releases area.
2. Extract the folder cleanly anywhere on your desktop.
3. Double-click `SubnetTrainer.exe` to boot up the trainer instantly—no extra runtime installations required!

### For Developers / Linux Compilation
To build and compile the source code directly via the Qt toolchain:

```bash
# Clone the repository
git clone [https://github.com/YOUR_USERNAME/YOUR_REPO_NAME.git](https://github.com/YOUR_USERNAME/YOUR_REPO_NAME.git)
cd YOUR_REPO_NAME

# Open the project in Qt Creator or compile directly with CMake/qmake
# Ensure you are targeting a kit with an active compiler (e.g., GCC on Linux, MinGW on Windows)

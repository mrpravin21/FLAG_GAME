
# 🎌 Guess the Flag - LAN Multiplayer Quiz Game

## 📖 Description

**Guess the Flag** is a LAN-based multiplayer quiz game built using **C socket programming** for the server and a web frontend using **HTML, CSS, and JavaScript**. 
Players identify flags of countries and earn points.
The server keeps track of each player using their roll number and maintains a real-time leaderboard that resets when the server is restarted.

---

## ✨ Features

- 🧠 Flag identification game over LAN
- 👥 Multiplayer support (multiple clients)
- 🏆 Live leaderboard with ranks
- 🥇 Rank highlighting (Gold, Silver, Bronze)
- 🔊 Sound effects for correct/wrong answers
- 🎨 Animations and improved UI using CSS Flexbox/Grid
- ❤️❤️❤️ Each Player has 3 Lives

---

## 🛠 Technologies Used

- **C** (for server and socket programming)
- **HTML, CSS, JavaScript** (for client UI)
- **TCP/IP sockets** for communication
- **Linked list** for maintaining leaderboard state
- **Local assets**: `.wav` audio files, `.json` country list, flags from https://flagpedia.net/emoji .

---

## 🚀 How to Run

### 1. Compile and Start the Server

```bash
gcc guess_flag_server.c -o server
./server
```
- This will start a socket server on port **8080**.

---

### 2. Compile and Start the Client programming to Lauch the Game

```bash
gcc guess_flag_client.c -o client
./client
```
- This will prompt a Player to enter their Roll number

- Launch the game in the Browser for the Player

- Open index.html in a browser like Chrome or Firefox.

- Multiple players on the same LAN can open the game in their browser and connect to the same server.

---

## 🗂 File Structure
```
FLAG_GAME/
├── guess_flag_server.c
├── guess_flag_client.c
├── index.html
├── guess_flag_styling.css
├── countries.json
├── correct.wav
├── incorrect.wav
├── README.md
```

---

## 💾 Data Persistence

- The **leaderboard resets** whenever the server is restarted.
- **High score is not saved locally**, ensuring a fresh competition per session.

---

## 🙋 Authors

### 1. Team Leader ⭐


- **Name**: Pravin Bhatta
- **College Roll Number**: 211428
- **Semester**: VII
- **Course**: BE IT (Morning)

---

### 2. Team Member 1️⃣

- **Name**: Rohan Khanal
- **College Roll Number**: 211434
- **Semester**: VII
- **Course**: BE IT (Morning)

---

### 2. Team Member 2️⃣

- **Name**: Kabir Thapa
- **College Roll Number**: 211414
- **Semester**: VII
- **Course**: BE IT (Morning)

---

## ⚠️ Notes

- All clients must be on the **same LAN** to connect to the server.
- Paths for sound files and flag images must be accurate in `index.html`.
- **Leaderboard highlights Top 3 ranks** with emoji  🥇  🥈  🥉 and larger size styling.
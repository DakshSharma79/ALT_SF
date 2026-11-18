# CampusAssist AI — C++ Edition

A C++17 console-based campus issue reporting and resolution prototype. It supports student accounts, complaint submission and tracking, automatic category/priority estimation, and an admin workflow to update complaint statuses.

## Features
- Student registration and login
- Submit campus issue with title, description and location
- Rule/keyword-based issue categorization (Internet/Wi-Fi, Electrical, Water Supply, Cleaning, Infrastructure, Security, Other)
- Priority estimation (Low / Medium / High)
- Student complaint history and status tracking
- Admin dashboard, category summary, and status updates
- CSV persistence in `data/users.csv` and `data/complaints.csv`

## Important scope note
This version is **C++17** and runs as a terminal application. It is not the previous Flask website, and it does not include a browser UI or a live cloud URL. The classifier is an explainable keyword-based baseline, not a trained ML model. It is appropriate as an intermediate programming prototype; do not claim measured ML accuracy.

## Requirements
- C++17 compiler: GCC 9+, Clang 10+, or a recent Visual Studio with C++ workload
- Optional: CMake 3.16+

## Build and run (Windows, MinGW)
Open PowerShell or Command Prompt in this project folder:

```powershell
g++ -std=c++17 -O2 -Wall -Wextra src/main.cpp -o campusassist.exe
.\campusassist.exe
```

## Build and run (Linux/macOS)
```bash
g++ -std=c++17 -O2 -Wall -Wextra src/main.cpp -o campusassist
./campusassist
```

## Build with CMake
```bash
cmake -S . -B build
cmake --build build
```
Then run the generated `campusassist` executable inside the build output directory.

## Demo administrator
- Email: `admin@campusassist.local`
- Password: `Admin@123`

Students can register from the main menu. The `data` directory is included and CSV files are created/updated automatically. Run the program from the project root so the relative `data/` paths resolve correctly.

## Suggested test
1. Sign in as admin and note that there are no complaints initially.
2. Exit and register a student account.
3. Sign in as student and submit: `Wi-Fi is not working in the computer lab today` at `Block C, Lab 302`.
4. View the student complaint history.
5. Sign in as admin and change its status to `In Progress`, then `Resolved`.
6. Reopen the student account and confirm the status has changed.

## Security and production limitations
This is a learning/hackathon prototype. Passwords are stored as plain text in the local CSV file; do not use real credentials or deploy this as-is for real users. A production version should use a secure password hashing library, database transactions, authorization controls, audit logs, and a networked web/API layer. The current terminal app is not a cloud-hosted service.

## GitHub upload
```bash
git init
git add .
git commit -m "Add CampusAssist AI C++ project"
git branch -M main
git remote add origin YOUR_GITHUB_REPOSITORY_URL
git push -u origin main
```

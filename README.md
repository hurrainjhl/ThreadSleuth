



```text
████████╗██╗  ██╗██████╗ ███████╗ █████╗ ██████╗     ███████╗██╗     ███████╗██╗   ██╗████████╗██╗  ██╗
╚══██╔══╝██║  ██║██╔══██╗██╔════╝██╔══██╗██╔══██╗    ██╔════╝██║     ██╔════╝██║   ██║╚══██╔══╝██║  ██║
   ██║   ███████║██████╔╝█████╗  ███████║██║  ██║    ███████╗██║     █████╗  ██║   ██║   ██║   ███████║
   ██║   ██╔══██║██╔══██╗██╔══╝  ██╔══██║██║  ██║    ╚════██║██║     ██╔══╝  ██║   ██║   ██║   ██╔══██║
   ██║   ██║  ██║██║  ██║███████╗██║  ██║██████╔╝    ███████║███████╗███████╗╚██████╔╝   ██║   ██║  ██║
   ╚═╝   ╚═╝  ╚═╝╚═╝  ╚═╝╚══════╝╚═╝  ╚═╝╚═════╝     ╚══════╝╚══════╝╚══════╝ ╚═════╝    ╚═╝   ╚═╝  ╚═╝
                                  >_ DIGITAL FORENSIC ANALYZER_
                                                                   

```

                                  High-Performance Parallel Digital Forensic Analyzer

</div>

---

## Overview

**ThreadSleuth** is a high-speed digital forensic analysis tool designed to process massive disk images efficiently. By leveraging a **C++ Master-Worker Multithreading Architecture**, it drastically reduces analysis time compared to traditional sequential tools. The system features a modern, interactive dashboard built with **Streamlit (Python)** for real-time progress tracking and reporting.

Instead of processing data bit-by-bit, ThreadSleuth divides the workload into fixed-size chunks and processes them concurrently using a synchronized thread pool, ensuring 100% CPU utilization and stability.

---

## System Architecture

The system follows a **Producer-Consumer** model to ensure thread safety and scalability:

1. **Master Thread (Producer):** Reads the raw disk image and slices it into manageable "chunks".
2. **Task Queue:** A synchronized, thread-safe queue that holds chunks waiting for analysis.
3. **Worker Threads (Consumers):** A persistent **Thread Pool** that grabs chunks from the queue and processes them in parallel (hashing, keyword search, etc.).
4. **Result Aggregator:** Uses **Mutex locks** to safely combine results from multiple workers into a final report without race conditions.

---

## Key Features

*  **High Performance:** Backend written in **C++** for low-level memory management and speed.
*  **Concurrency:** Implements a custom **Thread Pool** to avoid the overhead of creating/destroying threads.
*  **Thread Safety:** Uses **Mutexes** and **Condition Variables** to prevent data corruption (Race Conditions).
*  **Modern UI:** **Streamlit** frontend provides a clean, web-based dashboard to visualize results instantly.
*  **Automated Reporting:** Generates detailed forensic reports automatically upon completion.

---

## Technology Stack

| Component | Technology | Role |
| --- | --- | --- |
| **Backend Logic** | C++ (std::thread) | Core processing engine, memory management |
| **Frontend UI** | Python (Streamlit) | User interface, file upload, visualization |
| **Synchronization** | Mutex, Condition Variables | Ensuring thread safety and data integrity |
| **Inter-Process Comm** | Subprocess/File I/O | Connecting Python UI with C++ Engine |

---

## Installation & Usage

### Prerequisites

* **C++ Compiler:** MinGW (Windows) or G++ (Linux)
* **Python 3.x**
* **Visual Studio** (Optional, recommended for Windows users)

### Step 1: Clone the Repository

```bash
git clone [https://github.com/hurrainjhl/ThreadSleuth.git](https://github.com/hurrainjhl/ThreadSleuth.git)
cd ThreadSleuth

```

### Step 2: Compile the Backend (C++)

**On Windows:**

```bash
# If using MinGW
g++ main.cpp -o backend.exe -pthread

# OR if using Visual Studio
# Open the project in VS, Build Solution, and move the .exe to this folder.

```

**On Linux/Mac:**

```bash
g++ main.cpp -o backend -pthread

```

### Step 3: Run the Dashboard (Python)

Install the required library and launch the app:

```bash
pip install streamlit
streamlit run app.py

```

The application will open automatically in your browser at `http://localhost:8501`.

---

## Screenshots

*(Add a screenshot of your Streamlit dashboard here)*

---
---

## License
Distributed under the **MIT License**. See `LICENSE` for more information.

---

## Contributors

This project was built by a dedicated team of developers:

* **[Hoor ul ein Soomro](https://github.com/hurrainjhl)** - *Lead Developer*
* **[Affaf Ahmad](https://github.com/Affafahmad)** - *Developer*
* **[Sumaiya Arshad](https://github.com/pickachu19)** - *Developer*
* **[Team Member 3]** - *Developer*
* **[Team Member 4]** - *Developer*

---

<div align="center">
  <i>Developed with consistent work for the PDC course Project</i>
</div>
<div align="center">
<b>Developed by [MY Team]</b>




<i>Powered by Coffee and Code ☕</i>
</div>

```

```

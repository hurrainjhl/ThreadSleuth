import streamlit as st
import subprocess
import os
import re
import pandas as pd
import time
import shutil

# --- Configuration ---
C_EXEC = "./parallel_engine"
TEMP_FILE = "temp_evidence.dd"
REPORT_FILE = "forensic_report.html"

# --- Page Config (Dark Mode Force) ---
st.set_page_config(page_title="ThreadSleuth Engine", page_icon="🕵️", layout="wide")

# --- Custom CSS (Hacker/Cyberpunk Theme) ---
st.markdown("""
<style>
    /* General App Styling */
    .stApp {
        background-color: #0e1117;
    }
    
    /* Buttons */
    .stButton>button { 
        background-color: #00ff41; 
        color: black; 
        border: none;
        border-radius: 0px; 
        font-weight: bold; 
        font-family: 'Courier New', monospace;
        width: 100%; 
        transition: 0.3s;
        text-transform: uppercase;
        letter-spacing: 1px;
    }
    .stButton>button:hover { 
        background-color: #00cc33; 
        color: white; 
        box-shadow: 0 0 15px #00ff41;
    }

    /* Headers */
    h1, h2, h3 { 
        color: #00ff41 !important; 
        font-family: 'Courier New', monospace; 
        text-shadow: 0 0 5px #003300;
    }
    
    /* Metrics */
    div[data-testid="stMetricValue"] {
        color: #e0e0e0;
        font-family: 'Courier New', monospace;
    }
    
    /* Sidebar */
    section[data-testid="stSidebar"] {
        background-color: #050505;
        border-right: 1px solid #333;
    }

    /* Custom Terminal Log Box */
    .terminal-box {
        background-color: #000000;
        color: #00ff41;
        font-family: 'Courier New', monospace;
        font-size: 14px;
        padding: 15px;
        border: 1px solid #00ff41;
        border-radius: 5px;
        height: 300px;
        overflow-y: scroll;
        box-shadow: inset 0 0 10px #003300;
        white-space: pre-wrap;
    }
    
    /* Scrollbar for Terminal */
    .terminal-box::-webkit-scrollbar {
        width: 10px;
    }
    .terminal-box::-webkit-scrollbar-track {
        background: #000;
    }
    .terminal-box::-webkit-scrollbar-thumb {
        background: #00ff41;
    }
</style>
""", unsafe_allow_html=True)

# --- Header ---
col1, col2 = st.columns([1, 10])
with col1:
    st.markdown("## 🕵️")
with col2:
    st.title("ThreadSleuth")

st.markdown("**STATUS:** \`ONLINE\` | **KERNEL:** \`C++ PARALLEL\` | **ENCRYPTION DETECTOR:** \`ACTIVE\`")

# ==========================================
#              SIDEBAR CONTROLS
# ==========================================
with st.sidebar:
    st.header("⚙️ SYSTEM CONTROL")
    
    # 1. OPERATION MODE
    st.subheader("1. OPERATION MODE")
    mode = st.radio("TASK SELECT", ["🔍 FORENSIC SCAN", "⚡ BENCHMARK SUITE"])

    st.markdown("---")

    # 2. EVIDENCE INPUT
    st.subheader("2. EVIDENCE TARGET")
    upload_mode = st.radio("INPUT METHOD", ["📂 UPLOAD IMAGE", "📝 MANUAL PATH"])
    
    target_file = None
    if upload_mode == "📂 UPLOAD IMAGE":
        uploaded_file = st.file_uploader("DROP BINARY IMAGE", type=["dd", "img", "bin", "raw", "iso"])
        if uploaded_file:
            with open(TEMP_FILE, "wb") as f:
                f.write(uploaded_file.getbuffer())
            target_file = TEMP_FILE
            st.success(f"MOUNTED: {uploaded_file.name}")
    else:
        target_file = st.text_input("LOCAL PATH", value="os_test.dd")

    st.markdown("---")

    # 3. THREAD SETTINGS
    if mode == "🔍 FORENSIC SCAN":
        st.subheader("3. CORE ALLOCATION")
        num_threads = st.slider("WORKER THREADS", 1, 32, 4)
    
    # 4. REPORT MANAGER
    st.markdown("---")
    st.subheader("📂 DATA EXPORT")
    if os.path.exists(REPORT_FILE):
        with open(REPORT_FILE, "r") as f:
            st.download_button(
                label="⬇️ DOWNLOAD HTML REPORT",
                data=f,
                file_name="ThreadSleuth_Report.html",
                mime="text/html"
            )
    
    # 5. HELP (UPDATED Q&A)
    with st.expander("❓ MISSION BRIEFING (FAQ)"):
        st.markdown("""
        **Q: How does this tool work?**
        A: It splits the raw disk image into chunks and uses parallel C++ threads to scan them simultaneously.
        
        **Q: What files can I upload?**
        A: Raw disk images: \`.dd\`, \`.img\`, \`.bin\`, \`.raw\`, \`.iso\`.
        
        **Q: What is it scanning for?**
        1. **File Carving:** Looks for JPEG headers (\`FFD8FF\`), PDFs, ELFs.
        2. **Metadata:** Parses NTFS \`$MFT\` records and MBR Partition tables.
        3. **Entropy:** Calculates Shannon Entropy to find encrypted/malware areas.
        
        **Q: Why use Benchmark mode?**
        A: To prove to your professor that adding threads actually makes the tool faster (Speedup Factor).
        """)


# ==========================================
#              MAIN LOGIC
# ==========================================

if mode == "🔍 FORENSIC SCAN":
    st.subheader("🔍 LIVE FORENSIC INVESTIGATION")
    
    if st.button("🚀 INITIATE SCAN SEQUENCE"):
        if not target_file or not os.path.exists(target_file):
            st.error("❌ TARGET NOT FOUND. ABORTING.")
        else:
            # Progress Animation
            status_text = st.empty()
            status_text.warning("⚠️ INITIALIZING PARALLEL ENGINE... STANDBY.")
            my_bar = st.progress(0)
            
            # Run C++ Engine
            start_time = time.time()
            process = subprocess.run([C_EXEC, target_file, str(num_threads)], capture_output=True, text=True)
            end_time = time.time()
            
            # Finish Animation
            my_bar.progress(100)
            time.sleep(0.5)
            my_bar.empty()
            
            output = process.stdout
            
            # Parse Metrics
            time_match = re.search(r"Time Taken\s*:\s*([\d\.]+)", output)
            speed_match = re.search(r"Throughput\s*:\s*([\d\.]+)", output)
            exec_time = time_match.group(1) if time_match else "N/A"
            throughput = speed_match.group(1) if speed_match else "N/A"
            
            # Parse Entropy
            entropy_data = []
            for line in output.split("\n"):
                if "[ENTROPY]" in line:
                    try:
                        parts = line.split(":")
                        entropy_data.append({"Chunk": int(parts[0].split(" ")[1]), "Entropy": float(parts[1])})
                    except: pass
            
            status_text.success("✅ SEQUENCE COMPLETE. ARTIFACTS SECURED.")

            # 1. METRICS DASHBOARD
            st.markdown("### 📊 TELEMETRY")
            m1, m2, m3, m4 = st.columns(4)
            m1.metric("⏱ EXECUTION TIME", f"{exec_time} s")
            m2.metric("🚀 THROUGHPUT", f"{throughput} MB/s")
            m3.metric("🧵 THREADS", f"{num_threads}")
            m4.metric("📉 CHUNKS", f"{len(entropy_data)}")

            st.markdown("---")

            # 2. ENTROPY VISUALIZATION
            st.subheader("📈 ENTROPY ANALYSIS (ENCRYPTION DETECTION)")
            if entropy_data:
                df_entropy = pd.DataFrame(entropy_data).sort_values("Chunk")
                st.line_chart(df_entropy.set_index("Chunk"))
                st.caption(">> SPIKES > 7.5 INDICATE HIGH RANDOMNESS (ENCRYPTION/COMPRESSION)")
            else:
                st.warning("NO DATA STREAM.")
            
            # 3. REAL-TIME LOGS
            st.subheader("🖥️ KERNEL LOGS")
            st.markdown(f'<div class="terminal-box">{output}</div>', unsafe_allow_html=True)

            st.markdown("---")

            # 4. REPORT PREVIEW
            st.subheader("📄 CASE FILE PREVIEW")
            if os.path.exists(REPORT_FILE):
                with open(REPORT_FILE, 'r') as f:
                    report_html = f.read()
                st.components.v1.html(report_html, height=500, scrolling=True)

elif mode == "⚡ BENCHMARK SUITE":
    st.subheader("⚡ PERFORMANCE & SCALABILITY TEST")
    
    if st.button("📊 RUN STRESS TEST"):
        if not target_file or not os.path.exists(target_file):
            st.error("❌ NO TARGET.")
        else:
            results = []
            thread_counts = [1, 2, 4, 8, 16]
            progress_bar = st.progress(0)
            
            for i, t in enumerate(thread_counts):
                with st.spinner(f"STRESS TESTING CORE COUNT: {t}..."):
                    proc = subprocess.run([C_EXEC, target_file, str(t)], capture_output=True, text=True)
                    match = re.search(r"Time Taken\s*:\s*([\d\.]+)", proc.stdout)
                    if match:
                        results.append({"Threads": t, "Time (s)": float(match.group(1))})
                progress_bar.progress((i + 1) / len(thread_counts))
            
            df = pd.DataFrame(results)
            if not df.empty:
                base_time = df.loc[df['Threads'] == 1, 'Time (s)'].values[0]
                df['Speedup (x)'] = base_time / df['Time (s)']
                
                st.success("✅ BENCHMARK COMPLETE")
                
                c1, c2 = st.columns(2)
                with c1:
                    st.markdown("### 📋 RAW DATA")
                    st.table(df)
                with c2:
                    st.markdown("### 🚀 SPEEDUP FACTOR")
                    st.bar_chart(df.set_index("Threads")["Speedup (x)"])
                
                st.line_chart(df.set_index("Threads")["Time (s)"])


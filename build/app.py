import streamlit as st
import subprocess
import os
import re
import pandas as pd
import time
import base64
from PIL import Image

# --- Configuration ---
C_EXEC = "./parallel_engine"
TEMP_FILE = "temp_evidence.dd"
REPORT_FILE = "forensic_report.html"
LOGO_FILE = "logo.png" 
RECOVERED_DIR = "recovered_files"  # Folder where C++ saves recovered images

# --- Page Config ---
st.set_page_config(page_title="ThreadSleuth Engine", layout="wide")

# --- Custom CSS ---
st.markdown("""
<style>
    .stApp { background-color: #0e1117; }
    
    /* Centered Header Styling */
    .main-header {
        text-align: center;
        padding: 20px;
        border-bottom: 2px solid #00ff41;
        margin-bottom: 30px;
    }
    .main-header h1 {
        color: #00ff41;
        font-family: 'Courier New', monospace;
        font-size: 3rem;
        margin: 0;
        text-shadow: 0 0 10px rgba(0, 255, 65, 0.5);
    }
    .status-bar {
        font-family: 'Courier New', monospace;
        color: #e0e0e0;
        margin-top: 10px;
        font-size: 1rem;
    }
    .status-active { color: #00ff41; font-weight: bold; }
    
    /* Terminal Box Styling */
    .terminal-box {
        background-color: #000;
        color: #cccccc;
        font-family: 'Courier New', monospace;
        padding: 20px;
        border: 1px solid #333;
        border-left: 5px solid #00ff41;
        height: 400px;
        overflow-y: scroll;
        white-space: pre-wrap;
        line-height: 1.5;
        font-size: 14px;
    }

    /* Log Color Classes */
    .log-info { color: #569CD6; }      /* Blue */
    .log-entropy { color: #C586C0; }   /* Purple */
    .log-found { color: #00ff41; font-weight: bold; } /* Bright Green */
    .log-hash { color: #DCDCAA; }      /* Yellow */
    .log-report { color: #ffffff; font-weight: bold; text-decoration: underline; }
    .log-error { color: #f44336; }     /* Red */
    .log-meta { color: #4EC9B0; }      /* Teal */
    
    /* Buttons */
    .stButton>button { 
        background-color: #00ff41; color: black; border: none;
        font-family: 'Courier New'; font-weight: bold; width: 100%; 
        text-transform: uppercase;
    }

    /* Q/A Section Styling */
    .qa-box {
        background-color: #1e1e1e;
        padding: 15px;
        border-radius: 5px;
        border-left: 4px solid #00ff41;
        margin-bottom: 10px;
    }
    .qa-question {
        color: #00ff41;
        font-weight: bold;
        font-size: 1.1rem;
    }
    .qa-answer {
        color: #e0e0e0;
        margin-top: 5px;
    }
</style>
""", unsafe_allow_html=True)

# --- Helper: Load Image as Base64 ---
def get_base64_image(image_path):
    if os.path.exists(image_path):
        with open(image_path, "rb") as img_file:
            return base64.b64encode(img_file.read()).decode()
    return ""

# --- Helper: Format Log Lines with Colors ---
def format_log_output(raw_output):
    formatted_lines = []
    for line in raw_output.split('\n'):
        clean_line = line.strip()
        if not clean_line: continue
        
        # Apply CSS classes based on tags
        if clean_line.startswith("[INFO]"):
            formatted_lines.append(f"<span class='log-info'>{clean_line}</span>")
        elif clean_line.startswith("[ENTROPY]"):
            formatted_lines.append(f"<span class='log-entropy'>{clean_line}</span>")
        elif clean_line.startswith("[FOUND]"):
            formatted_lines.append(f"<span class='log-found'>{clean_line}</span>")
        elif clean_line.startswith("[HASH]"):
            formatted_lines.append(f"<span class='log-hash'>&nbsp;&nbsp;└─ {clean_line}</span>")
        elif clean_line.startswith("[REPORT]"):
            formatted_lines.append(f"<span class='log-report'>{clean_line}</span>")
        elif clean_line.startswith("[ERROR]"):
            formatted_lines.append(f"<span class='log-error'>{clean_line}</span>")
        elif "Time Taken" in clean_line or "Throughput" in clean_line:
            formatted_lines.append(f"<span class='log-meta'>{clean_line}</span>")
        else:
            formatted_lines.append(f"<span>{clean_line}</span>")
            
    return "<br>".join(formatted_lines)

# --- Centered Header with Logo ---
logo_b64 = get_base64_image(LOGO_FILE)
logo_html = f'<img src="data:image/png;base64,{logo_b64}" width="120" style="margin-bottom: 10px;">' if logo_b64 else ""

st.markdown(f"""
<div class="main-header">
{logo_html}
<h1>ThreadSleuth</h1>
<div class="status-bar">
STATUS: <span class="status-active">ONLINE</span> | 
KERNEL: <span class="status-active">C++ PARALLEL</span> | 
MODE: <span class="status-active">ACTIVE</span>
</div>
</div>
""", unsafe_allow_html=True)

# ==========================================
#              SIDEBAR CONTROLS
# ==========================================
with st.sidebar:
    st.header("SYSTEM CONTROL")
    mode = st.radio("TASK SELECT", ["FORENSIC SCAN", "BENCHMARK SUITE", "PROJECT Q/A"])
    st.markdown("---")
    
    st.subheader("EVIDENCE TARGET")
    upload_mode = st.radio("INPUT METHOD", ["UPLOAD IMAGE", "MANUAL PATH"])
    
    target_file = None
    if upload_mode == "UPLOAD IMAGE":
        uploaded_file = st.file_uploader("DROP BINARY IMAGE")
        if uploaded_file:
            with open(TEMP_FILE, "wb") as f: f.write(uploaded_file.getbuffer())
            target_file = TEMP_FILE
            st.success(f"MOUNTED: {uploaded_file.name}")
    else:
        # Improved Manual Path Logic
        raw_path = st.text_input("LOCAL PATH", value="os_test.dd")
        if raw_path:
            if os.path.exists(raw_path):
                target_file = raw_path
            elif os.path.exists(os.path.join(os.getcwd(), raw_path)):
                target_file = os.path.join(os.getcwd(), raw_path)
            else:
                st.error("FILE NOT FOUND")

    st.markdown("---")
    
    num_threads = 4
    if mode == "FORENSIC SCAN":
        st.subheader("CORE ALLOCATION")
        num_threads = st.slider("WORKER THREADS", 1, 32, 4)

    if os.path.exists(REPORT_FILE):
        with open(REPORT_FILE, "r") as f:
            st.download_button("DOWNLOAD HTML REPORT", f, "report.html", "text/html")

# ==========================================
#              MAIN LOGIC
# ==========================================

if mode == "FORENSIC SCAN":
    st.markdown("<h3 style='text-align: center; color: #00ff41;'>LIVE FORENSIC INVESTIGATION</h3>", unsafe_allow_html=True)
    
    if st.button("INITIATE SCAN SEQUENCE"):
        if not target_file:
            st.error("TARGET NOT FOUND.")
        else:
            # --- AUTO-CLEANUP ---
            if os.path.exists(RECOVERED_DIR):
                for file in os.listdir(RECOVERED_DIR):
                    file_path = os.path.join(RECOVERED_DIR, file)
                    try:
                        if os.path.isfile(file_path):
                            os.unlink(file_path)
                    except Exception as e:
                        pass
            # --------------------

            status = st.empty()
            status.warning("SCANNING SECTORS...")
            bar = st.progress(0)
            
            # Run C++ Engine
            try:
                start = time.time()
                proc = subprocess.run([C_EXEC, target_file, str(num_threads)], capture_output=True, text=True)
                bar.progress(100)
                
                output = proc.stdout
                
                # Parse Entropy for Chart
                entropy_data = []
                for line in output.split("\n"):
                    if "[ENTROPY]" in line:
                        try:
                            parts = line.split(":")
                            entropy_data.append({"Chunk": int(parts[0].split(" ")[1]), "Entropy": float(parts[1])})
                        except: pass
                
                status.success("SCAN COMPLETE.")
                
                # 1. Metrics
                st.markdown("### TELEMETRY")
                c1, c2, c3, c4 = st.columns(4)
                
                exec_time = re.search(r"Time Taken\s*:\s*([\d\.]+)", output)
                speed = re.search(r"Throughput\s*:\s*([\d\.]+)", output)
                artifacts = re.search(r"Artifacts\s*:\s*(\d+)", output)

                c1.metric("EXECUTION TIME", f"{exec_time.group(1)} s" if exec_time else "N/A")
                c2.metric("THROUGHPUT", f"{speed.group(1)} MB/s" if speed else "N/A")
                c3.metric("THREADS", num_threads)
                c4.metric("ARTIFACTS", f"{artifacts.group(1)}" if artifacts else "0")
                
                # 2. Entropy Chart
                st.subheader("ENTROPY ANALYSIS")
                if entropy_data:
                    st.line_chart(pd.DataFrame(entropy_data).set_index("Chunk"))
                
                # 3. Formatted Kernel Logs
                st.subheader("KERNEL LOGS")
                formatted_logs = format_log_output(output)
                st.markdown(f'<div class="terminal-box">{formatted_logs}</div>', unsafe_allow_html=True)

                # 4. RECOVERED EVIDENCE GALLERY
                st.markdown("---")
                st.subheader("RECOVERED EVIDENCE GALLERY")
                
                time.sleep(0.5)
                
                if os.path.exists(RECOVERED_DIR):
                    files = os.listdir(RECOVERED_DIR)
                    images = [f for f in files if f.endswith(('.jpg', '.jpeg', '.png'))]
                    
                    if images:
                        cols = st.columns(4)  # Grid View
                        for idx, img_file in enumerate(images):
                            img_path = os.path.join(RECOVERED_DIR, img_file)
                            try:
                                image = Image.open(img_path)
                                with cols[idx % 4]:
                                    st.image(image, caption=img_file, width="stretch")
                            except:
                                pass
                    else:
                        st.info("No images recovered yet (or only non-image files found).")
                else:
                    st.warning("No recovery directory found (Check write permissions).")
                
            except Exception as e:
                st.error(f"EXECUTION ERROR: {e}")

elif mode == "BENCHMARK SUITE":
    st.subheader("PERFORMANCE TEST")
    if st.button("RUN STRESS TEST"):
        if not target_file: st.error("NO TARGET")
        else:
            results = []
            threads = [1, 2, 4, 8, 16]
            bar = st.progress(0)
            for i, t in enumerate(threads):
                proc = subprocess.run([C_EXEC, target_file, str(t)], capture_output=True, text=True)
                match = re.search(r"Time Taken\s*:\s*([\d\.]+)", proc.stdout)
                if match: results.append({"Threads": t, "Time": float(match.group(1))})
                bar.progress((i+1)/len(threads))
            
            df = pd.DataFrame(results)
            if not df.empty:
                # Calculate Speedup
                base_time = df.loc[df['Threads'] == 1, 'Time'].values[0]
                df['Speedup'] = base_time / df['Time']
                
                st.markdown("#### PERFORMANCE METRICS")
                st.table(df)
                
                st.markdown("#### SPEEDUP FACTOR (Scalability)")
                st.bar_chart(df.set_index("Threads")["Speedup"])

                # --- NEW GRAPH: TIME vs THREADS ---
                st.markdown("#### EXECUTION TIME (Drop-off)")
                st.line_chart(df.set_index("Threads")["Time"])

elif mode == "PROJECT Q/A":
    st.markdown("<h3 style='text-align: center; color: #00ff41;'>PROJECT KNOWLEDGE BASE</h3>", unsafe_allow_html=True)
    st.markdown("---")

    qa_pairs = [
        ("What does ThreadSleuth do?", 
         "It is a Parallel Forensic Engine that reads raw disk images to find deleted files (File Carving) and detect encryption (Entropy Analysis) using multi-threading."),
        
        ("Where is Parallel Computing (PDC) used?", 
         "We use a <b>Producer-Consumer Model</b>. The Main Thread reads disk chunks and pushes them to a Task Queue. A Pool of Worker Threads pops chunks and processes them simultaneously."),
        
        ("How does File Carving work?", 
         "We use a <b>Signature Scanner</b> (Boyer-Moore logic). We look for magic bytes like <code>FF D8 FF</code> (JPEG) or <code>89 50 4E 47</code> (PNG) and extract the data following them."),
        
        ("What is Entropy Analysis?", 
         "We calculate Shannon Entropy for every 10MB chunk. If entropy is > 7.5, it indicates high randomness, which usually means the data is Encrypted or Compressed."),
        
        ("How do you handle Synchronization?", 
         "We use <code>std::mutex</code> and <code>std::unique_lock</code> in the Task Queue and Result Collector to prevent Race Conditions.")
    ]

    for q, a in qa_pairs:
        st.markdown(f"""
        <div class="qa-box">
            <div class="qa-question">Q: {q}</div>
            <div class="qa-answer">A: {a}</div>
        </div>
        """, unsafe_allow_html=True)

    st.markdown("---")
    st.subheader("ASK THE ENGINE")
    user_q = st.text_input("Enter your question regarding the architecture:")
    if user_q:
        st.info(f"Analyzing query: '{user_q}'... (AI Module Offline - Please refer to documentation above)")

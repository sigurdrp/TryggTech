import os
import random
import time
from datetime import datetime, timezone

import firebase_admin
from firebase_admin import credentials, firestore

KEY_PATH = os.environ.get("GOOGLE_APPLICATION_CREDENTIALS")
if not KEY_PATH or not os.path.exists(KEY_PATH):
    raise RuntimeError(
        "GOOGLE_APPLICATION_CREDENTIALS is not set or points to a missing file.\n"
        "Set it in PowerShell: $env:GOOGLE_APPLICATION_CREDENTIALS='C:\\Keys\\tryggtech-admin.json'"
    )

if not firebase_admin._apps:
    cred = credentials.Certificate(KEY_PATH)
    firebase_admin.initialize_app(cred)

db = firestore.client()

NAMES = [
    "Sigurd Randers-Pehrson", "Martin Tandberg", "Johannes Bævre-Jensen",
    "Magnus Kjølhamar", "Brage Bjørsvik Sætra", "Peter Døvle"
]
STATUSES = ["OK", "OK", "OK", "ALERT"]

def now_iso():
    return datetime.now(timezone.utc).isoformat()

def rand_vitals():
    return {
        "temp_c": round(random.uniform(36.0, 39.5), 1),
        "bp_sys": random.randint(100, 145),
        "bp_dia": random.randint(60, 95),
        "hr_bpm": random.randint(55, 115),
    }

def upsert_patient(name: str):
    doc_id = name.replace(" ", "_")
    data = {
        "name": name,
        "vitals": rand_vitals(),
        "status": random.choice(STATUSES),
        "updated_at": now_iso(),
    }
    db.collection("patients").document(doc_id).set(data)
    print(f"Upserted {doc_id}: {data}")

if __name__ == "__main__":
    for n in NAMES:
        upsert_patient(n)

    print("\nStreaming updates until ctrl+C")
    try:
        while True:
            upsert_patient(random.choice(NAMES))
            time.sleep(5)
    except KeyboardInterrupt:
        print("\nStopped.")

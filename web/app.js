import { initializeApp } from "https://www.gstatic.com/firebasejs/10.12.3/firebase-app.js";
import {
  getFirestore,
  collection,
  onSnapshot
} from "https://www.gstatic.com/firebasejs/10.12.3/firebase-firestore.js";

const firebaseConfig = {
  apiKey: "AIzaSyCrNKSzVBssQS_ZI2qV4xlPNHIcG03sm8E",
  authDomain: "tryggtech-backend.firebaseapp.com",
  projectId: "tryggtech-backend",
  storageBucket: "tryggtech-backend.firebasestorage.app",
  messagingSenderId: "843198960491",
  appId: "1:843198960491:web:970d00958e1b4e18c5322e",
  measurementId: "G-CQJ3KP2Z9Q"
};

const app = initializeApp(firebaseConfig);
const db = getFirestore(app);

const container = document.getElementById("grid-container");

function renderCard(data) {
  const v = data.vitals ?? {};
  return `
    <h3>${data.name ?? "-"}</h3>
    <p class="js-status">Status: ${data.status ?? "-"}</p>
    <p>Temp: ${v.temp_c ?? "-"} °C</p>
    <p>BT: ${v.bp_sys ?? "-"} / ${v.bp_dia ?? "-"}</p>
    <p>Puls: ${v.hr_bpm ?? "-"}</p>
    <small>${data.updated_at ? new Date(data.updated_at).toLocaleString() : ""}</small>
  `;
}

onSnapshot(collection(db, "patients"), (snapshot) => {
  snapshot.docChanges().forEach((change) => {
    const id = change.doc.id;
    const data = change.doc.data();

    if (change.type === "added" || change.type === "modified") {
      let div = document.getElementById(id);
      if (!div) {
        div = document.createElement("div");
        div.id = id;
        div.className = "card";
        container.appendChild(div);
      }
      div.innerHTML = renderCard(data);
    }

    if (change.type === "removed") {
      const div = document.getElementById(id);
      if (div) div.remove();
    }
  });
});

// src/firebase.js 'firebase/database';
// src/firebase.js
import { initializeApp } from 'firebase/app';
import { getDatabase } from 'firebase/database';
// TODO: Add SDKs for Firebase products that you want to use
// https://firebase.google.com/docs/web/setup#available-libraries

// Your web app's Firebase configuration
// For Firebase JS SDK v7.20.0 and later, measurementId is optional
const firebaseConfig = {
  apiKey: "AIzaSyCrMRlowDO5V0GidSbrJaRbytXPeyHNrxQ",
  authDomain: "esp32-firebase-demo-791ad.firebaseapp.com",
  databaseURL: "https://esp32-firebase-demo-791ad-default-rtdb.asia-southeast1.firebasedatabase.app",
  projectId: "esp32-firebase-demo-791ad",
  storageBucket: "esp32-firebase-demo-791ad.appspot.com",
  messagingSenderId: "819484628457",
  appId: "1:819484628457:web:1c28d4c02e85349219a778",
  measurementId: "G-924JP16SD2"
};

const app = initializeApp(firebaseConfig);
const database = getDatabase(app);

export default database;
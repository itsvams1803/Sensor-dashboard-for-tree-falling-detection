const functions = require('firebase-functions');
const admin = require('firebase-admin');
const nodemailer = require('nodemailer');

admin.initializeApp();
const db = admin.database();

// Configure the email transport using the default SMTP transport and a GMail account.
// For other types of transports (Amazon SES, Sendgrid, Postmark, etc.) see Nodemailer docs.
const transporter = nodemailer.createTransport({
  service: 'gmail',
  auth: {
    user: 'vamsi4project@gmail.com', // Your email
    pass: 'vamsi_iitRoorkee2024' // Your email password
  }
});

// Create a function to send the email
const sendEmail = (altitude) => {
  const mailOptions = {
    from: 'vamsi4project@gmail.com',
    to: 'vamsi180303@gmail.com',
    subject: 'Tree Fall Alert!',
    text: `Alert! The tree has fallen to ground level. Current altitude: ${altitude} meters.`
  };

  return transporter.sendMail(mailOptions, (error, info) => {
    if (error) {
      console.log('Error sending email:', error);
    } else {
      console.log('Email sent:', info.response);
    }
  });
};

exports.checkAltitude = functions.database.ref('/devices/{deviceId}/sensor_data/{timestamp}')
  .onCreate((snapshot, context) => {
    const sensorData = snapshot.val();
    const altitude = sensorData.altitude;

    // Check if the altitude is 0 or below the threshold
    if (altitude <= 0) {
      return sendEmail(altitude);
    } else {
      return null;
    }
  });

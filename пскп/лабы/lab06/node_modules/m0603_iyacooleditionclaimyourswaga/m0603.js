const nodemailer = require('nodemailer');

const args = process.argv.slice(2);
const FIXED_EMAIL = "example@gmail.com";
const FIXED_PASSWORD = args[0];

const transporter = nodemailer.createTransport({
    service: 'gmail',
    auth: {
        user: FIXED_EMAIL,
        pass: FIXED_PASSWORD
    }
});

function send(messageString) {
    const email = {
        from: FIXED_EMAIL,
        to: FIXED_EMAIL, 
        subject: "Сообщение из модуля m0603",
        html: `${messageString}`,
    };
    
    return transporter.sendMail(email);
}

module.exports = { send };
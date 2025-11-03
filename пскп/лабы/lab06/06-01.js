const nodemailer = require('nodemailer');
const http = require('http');
const fs = require('fs');
const url = require('url');
const {parse} = require('querystring'); 

const port = 5000;

const server = http.createServer((req, res) => {
    const parsedUrl = url.parse(req.url, true);

    if(req.method === 'GET' && url.parse(req.url).pathname === '/') {
        res.statusCode = 200;
        res.setHeader("Content-Type", "text/html");
        fs.createReadStream('./index.html').pipe(res);
    }
    else if(req.method === 'POST' && parsedUrl.pathname === '/') {
        let body = '';
        req.on('data', chunk => {body += chunk;});
        req.on('end', () => {
            let parm = parse(body);
            const nm = nodemailer.createTransport({
               service: 'gmail',
               auth : {
                user: process.env.userm,
                pass: process.env.passm    
               }
            })
            nm.sendMail(mailOptions, (error, info) => {
                if(error) {
                    res.writeHead(500, { 'Content-Type': 'text/html; charset=utf-8' });
                    res.end(`<h2>Ошибка при отправке: ${error.message}</h2>`);
                }
                else {
                    console.log(`Письмо отправлено: ${info.response}`);
                    res.writeHead(200, { 'Content-Type': 'text/html; charset=utf-8' });
                    res.end(`<h2>Письмо успешно отправлено!</h2>`);
                }
            })
        });
    }
    else {
        res.writeHead(404, { 'Content-Type': 'text/html; charset=utf-8' });
        res.end('<h2>Not found</h2>');
    }
});

server.listen(port, () => {
    console.log(`Сервер запущен на http://localhost:${port}`);
});
const nodemailer = require('nodemailer');
const http = require('http');
const fs = require('fs');
const url = require('url');
const {parse} = require('querystring'); 

const port = 5000;
const args = process.argv.slice(2);
const EMAIL = args[0]; //почту и пароль получаю через командную строку. 
const PASS = args[1]; //команда должна выглядеть так: 
                            //  node .\06-91.js "example@gmail.com" "ffff ffff ffff ffff"
                            //пароль (второй параметр) тот, который вы получили с двухфакторки, не тот, что почте принадлежит

const nm = nodemailer.createTransport({
    service: 'gmail',
        auth : {
            user: EMAIL,
            pass: PASS   
        },
    });

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
            let params = parse(body);

            const email = {
                from: params.from || EMAIL,
                to: params.to || EMAIL,
                subject: params.subject || "без темы",
                html: `<div>${params.message || "нет текста"}</div>`,
            };
            
            nm.sendMail(email)
                .then(info => {
                    console.log(info, "\n===========================\n");
                    res.writeHead(200, { "Content-Type": "text/html; charset=utf-8" });
                    res.end(`<h2>Письмо успешно отправлено!</h2>`);
                })
                .catch(err => {
                    console.error(err);
                    res.writeHead(200, { "Content-Type": "text/html; charset=utf-8" });
                    res.end(`<h2>Ошибка при отправке:</h2><p>${err.message}</p>`);
                });
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
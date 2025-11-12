const http = require('http');

const {HandlePath, handleStaticFiles} = require('./module.js');

const port = 5000;
const dir = './static';

const server = http.createServer(handleStaticFiles(dir));

server.listen(port, () => {
    console.log(`Сервер запущен на http://localhost:${port}`);
});
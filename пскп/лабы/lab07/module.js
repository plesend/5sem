const http = require('http');
const fs = require('fs');
const path = require('path');
const url = require('url'); 
 
const dir = './static';

const MIME_TYPES = {
    'html': 'text/html',
    'css': 'text/css',
    'js': 'text/javascript',
    'png': 'image/png',
    'docx': 'application/msword',
    'json': 'application/json',
    'xml': 'application/xml',
    'mp4': 'video/mp4'
};

function handleStaticFiles(staticDir) {
    return function(req, res) {
        if (req.method !== 'GET') {
            res.writeHead(405, { 'Content-Type': 'text/html; charset=utf-8' });
            return res.end('<h1>Method Not Allowed</h1>');
        }

        const parsedUrl = url.parse(req.url, true);
        const pathname = parsedUrl.pathname;
        const requestPath = pathname.substring(1);
        
        const fullPath = path.join(__dirname, staticDir, requestPath);
        const extension = path.extname(requestPath).toLowerCase().substring(1);

        if (!MIME_TYPES[extension]) {
            res.writeHead(405, { 'Content-Type': 'text/html; charset=utf-8' });
            return res.end('<h1>Not Found: ' + extension + '</h1>');
        }

        fs.access(fullPath, fs.constants.F_OK, (err) => {
            if (err) {
                res.writeHead(404, { 'Content-Type': 'text/html; charset=utf-8' });
                return res.end('<h1>File not found: ' + pathname + '</h1>');
            }

            res.setHeader('Content-Type', MIME_TYPES[extension]);
            const readStream = fs.createReadStream(fullPath);
            
            readStream.on('error', (error) => {
                console.error('Error reading file:', error);
                res.writeHead(500, { 'Content-Type': 'text/html; charset=utf-8' });
                res.end('<h1>Internal Server Error</h1>');
            });

            readStream.pipe(res);
        });
    };
}

module.exports = { handleStaticFiles }; 
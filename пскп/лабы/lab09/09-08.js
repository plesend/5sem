const http = require('http');
const fs = require('fs');
const path = require('path');

const filename = 'MyFile.png';
const outPath = path.join(__dirname, 'uploads-' + filename);

const options = {
  hostname: 'localhost',
  port: 5000,
  path: '/files/' + encodeURIComponent(filename),
  method: 'GET'
};

console.log(options.path);

const req = http.request(options, res => {
  console.log('status:', res.statusCode);

  if (res.statusCode !== 200) {
    let body = ''; 
    res.on('data', chunk => body += chunk);
    res.on('end', () => console.log('body:', body));
    return;
  }

  const ws = fs.createWriteStream(outPath);
  res.pipe(ws);
  ws.on('finish', () => console.log('Saved to', outPath));
});

req.on('error', console.error);
req.end();

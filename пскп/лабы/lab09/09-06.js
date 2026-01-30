const http = require('http');
const fs = require('fs');
const path = require('path');

const filePath = path.join(__dirname, 'files/MyFile.txt'); 
if (!fs.existsSync(filePath)) {
  console.log(filePath);
  console.error('file not found');
  process.exit(1);
}

const boundary = '----SpecialKatesBoundary' + Date.now();
const stat = fs.statSync(filePath);
const filename = path.basename(filePath);
const pre = `--${boundary}\r\nContent-Disposition: attachment; name="file"; filename="${filename}"\r\nContent-Type: text/plain\r\n\r\n`;
const post = `\r\n--${boundary}--\r\n`;
const preBuf = Buffer.from(pre, 'utf8');
const postBuf = Buffer.from(post, 'utf8');


const options = {
  hostname: 'localhost',
  port: 5000,
  path: '/upload',
  method: 'POST',
  headers: {
    'Content-Type': 'multipart/form-data; boundary=' + boundary,
  }
};

const req = http.request(options, res => {
  console.log('status:', res.statusCode);

  let body = ''; 

  res.on('data', chunk => body += chunk); 
  res.on('end', () => console.log('body:', body)); 
});

req.on('error', console.error);
req.write(preBuf);
const rs = fs.createReadStream(filePath);
  rs.on('end', () => {
    req.write(postBuf);
    req.end();
});
rs.pipe(req, { end: false });

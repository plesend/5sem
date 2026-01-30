const http = require('http');

const x = 7, y = 3;
const options = {
  hostname: 'localhost',
  port: 5000,
  path: `/parameter?x=${x}&y=${y}`,
  method: 'GET'
};

const req = http.request(options, (res) => {
  console.log('status:', res.statusCode);

  let body = '';

  res.on('data', chunk => body += chunk);

  res.on('end', () => {
    console.log('body:', body);
  });
});

req.on('error', console.error);

req.end();

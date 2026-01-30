const http = require('http');

const xml = `
<request id="28">
  <x value="7"/>
  <x value="2"/>
  <x value="3"/>
  <m value="a"/>
  <m value="b"/>
  <m value="c"/>
</request>`;

const options = {
  hostname: 'localhost',
  port: 5000,
  path: '/xml',
  method: 'POST',
  headers: {
    'Content-Type': 'application/xml',
  }
};

const req = http.request(options, res => {
  console.log('status:', res.statusCode, res.statusMessage);
  let body = '';
  res.on('data', chunk => body += chunk);
  res.on('end', () => console.log('body:', body));
});
req.on('error', console.error);
req.write(xml);
req.end();

const coap = require('coap');
const server = coap.createServer();

// Tạo một file firmware giả lập chứa 2048 chữ 'A'
const firmware = Buffer.alloc(2048, 'A'); 

server.on('request', (req, res) => {
  // Nếu Client hỏi xin file firmware
  if (req.url === '/firmware') {
    console.log('Firmware requested (Co ai do xin tai file)');
    
    // Server node-coap sẽ TỰ ĐỘNG chia nhỏ file này ra (Blockwise)
    res.end(firmware);
  }
});

server.listen(() => {
  console.log('CoAP Blockwise Server (server3.js) started on port 5683');
});
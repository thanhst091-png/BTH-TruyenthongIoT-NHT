const coap = require('coap');
const server = coap.createServer();

server.on('request', (req, res) => {
  console.log('Received:', req.payload.toString());
  
  // --- TÌNH HUỐNG THỬ NGHIỆM ---
  // Để test cơ chế gửi lại (Retransmission) của gói CON:
  // bỏ chú thích (uncomment) dòng dưới để Server không trả lời
  // return; 

  res.end('OK'); 
});

server.listen(() => {
  console.log('CoAP server started');
});
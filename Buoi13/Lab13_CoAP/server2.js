const coap = require('coap');
const server = coap.createServer();

let temperature = 30.0;
let observers = []; // Danh sách các thiết bị đang "hóng" biến động

// Giả lập nhiệt độ thay đổi tự động mỗi 3 giây
setInterval(() => {
  temperature += 0.5;
  if (temperature > 40) temperature = 30; // Reset nếu nóng quá
  
  console.log('Temperature updated:', temperature);
  
  // Gửi thông báo cho tất cả các bên đang Observe
  observers.forEach(res => {
    res.write(`temp=${temperature}`);
  });
}, 3000);

server.on('request', (req, res) => {
  if (req.url === '/sensor/temp') {
    
    // Kiểm tra xem Client có đòi Observe không?
    if (req.headers['Observe'] === 0) {
      console.log('Client registered Observe (Da dang ky theo doi)');
      observers.push(res);
      
      // Giữ kết nối để đẩy dữ liệu sau này
      res.write(`temp=${temperature}`);
    } 
    else {
      // Nếu chỉ GET bình thường (Pull)
      console.log('Client sent GET request (Hoi tham binh thuong)');
      res.end(`temp=${temperature}`);
    }
  }
});

server.listen(() => {
  console.log('CoAP Observe Server started on port 5683');
});
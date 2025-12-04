// routes/index.js
var express = require('express');
var router = express.Router();

/* GET home page. */
router.get('/', function(req, res, next) {
  // Lấy cổng hiện tại của server (lúc này là 4000)
  const port = req.app.get('port'); 
  
  // Render view và truyền tham số port vào template
  res.render('index', { title: 'IoT Real-time Dashboard', port: port });
});

module.exports = router;
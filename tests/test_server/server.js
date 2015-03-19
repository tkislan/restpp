var express = require('express');
var app = express();
var bodyParser = require('body-parser');
var path = require('path');
var fs = require('fs');

app.use(bodyParser.urlencoded({ extended: true }));
app.use(bodyParser.json());

var router = express.Router();

router.get('/', function(req, res) {
  var httpMethod = req.method;
  console.log("Method:", httpMethod);

  var httpPath = req.baseUrl;
  console.log("Path:", httpPath);

  var queryParams = req.query;
  console.log("Query Params:", queryParams);

  var headers = req.headers;
  console.log("Headers:", headers);

  res.json({ message: 'hooray! welcome to our api!' });
});

var filepath = path.resolve(__dirname, __filename);
// var filepath = path.resolve('C:\\Users\\Kto\\Downloads', 'jdk-8u25-windows-i586.exe');

router.get('/server.js', function(req, res) {
  // res.sendFile(filepath, function(err) {
  //   console.log("sendFile callback:", err);
  // });

  fs.createReadStream(filepath).pipe(res);
});

app.use('/api', router);

app.listen(8000);

console.log("Server running at http://localhost:8000/");

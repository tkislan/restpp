var express = require('express');
var app = express();
var bodyParser = require('body-parser');

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

app.use('/api', router);

app.listen(8000);

console.log("Server running at http://localhost:8000/");

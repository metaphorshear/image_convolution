var express = require('express');
var app = express();
var bodyParser = require('body-parser');

app.use(express.static('public'));
app.set('view engine', 'pug');


app.use(bodyParser.json({limit: '50mb'}));
app.use(bodyParser.raw({limit: '50mb'}));
app.use(bodyParser.urlencoded({ extended: true, limit: '50mb'}));

var types = [
//    {
//        title: "single-threaded",
//        description: "This is the sequential version of the code."
//    },
    {
        title: "multi-threaded",
        description: "This is the parallel version of the code."
    }
];
types.forEach(function (type){
    app.use('/'+type.title, require('./routes/' + type.title));
});

app.get('/', function (req, res) {
    res.render('multi-threaded', { routes: types, target: 'multi-threaded'});
});

var server = app.listen(3000, function () {
    console.log('Web server listening at http://localhost:%s',
        server.address().port);
});

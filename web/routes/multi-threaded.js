var express = require('express');
var path = require('path')
var router = express.Router();

var type = path.basename(__filename).slice(0, -3)

router.get('/', function(req, res) {
  res.render('multi-threaded', {target:type});
});


router.post('/', function(req, res) {
    var ffi = require('ffi');
    var ref = require('ref');
    var Struct = require('ref-struct');
    var ArrayType = require('ref-array');
    var ucharArray = ArrayType(ref.types.uchar);
    var floatarr = ArrayType(ref.types.float);
    var floatarrarr = ArrayType(floatarr);
    var cImage = Struct({
		'data': ucharArray,
		'w': 'int',
		'h': 'int',
		'c': 'int'
	});
	var fmatrix = Struct({
		'rows': 'int',
		'cols': 'int',
		'array': floatarrarr
	});
	var cKernel = Struct({
		'psf' : ref.refType(fmatrix),
		'divisor': 'int'
	});
	var lib = ffi.Library('../c/build/Release/image_threaded', {
		'new_psf' : [ref.refType(cKernel), ['int', 'int', 'int']],
		'free_psf' : ['void', [ref.refType(cKernel)]],
		'convolve_image' : ['void', [ref.refType(cImage),ref.refType(cImage),ref.refType(cKernel),'int']]
	});
	var divisor = parseInt(req.body.divisor);
	var threads = parseInt(req.body.threads);
	var fsp = JSON.parse(req.body.fsp);
	var src = new cImage();
	var data = JSON.parse(req.body.data);
	src.w = parseInt(req.body.width);
	src.h = parseInt(req.body.height);
	src.c = 4; //canvas is always RGBA
	src.data = new ucharArray(src.w * src.h * src.c );
	for (var i = 0; i < (src.w * src.h * src.c); ++i){
		src.data[i] = data[i];
	}
	var psf = new fmatrix();
	//fix this to be variable size later
	psf.rows = 3; 
	psf.cols = 3;
	psf.array = new floatarrarr(3);
	psf.array[0] = new floatarr(3);
	psf.array[1] = new floatarr(3);
	psf.array[2] = new floatarr(3);
	for (var i = 0; i < 3; ++i){
		for (var j = 0; j < 3; ++j){
			psf.array[i][j] = fsp[i][j].value;
		}
	}
	var kernel = new cKernel();
	kernel.psf = psf.ref();
	kernel.divisor = divisor;
	
	var dst = new cImage();
	dst.w = src.w;
	dst.h = src.h;
	dst.c = src.c;
	dst.data = new ucharArray(dst.w*dst.h*dst.c);
	lib.convolve_image(src.ref(), dst.ref(), kernel.ref(), threads);
	var imagedata = new Array(dst.w * dst.h * dst.c);
	for (var i = 0; i < (dst.w * dst.h * dst.c); ++i){
		imagedata[i] = dst.data[i];
	}
	res.setHeader('Content-Type', 'application/json');
    res.end(JSON.stringify({
      result: imagedata
    }));
    
	
    
});
module.exports = router;

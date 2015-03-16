var fs = require("fs");

var plexer = require("plexer");

var openser = require("./build/Release/openser").openser;

exports.openSync = function serial$openAsStream(path, baud) {
  var fd = openser(String(path), ~~baud);
  return plexer(fs.createReadStream(null, { fd: fd }),
                fs.createWriteStream(null, { fd: fd }));
};

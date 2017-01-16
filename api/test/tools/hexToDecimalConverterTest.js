var htdConverter = require("../../srv/tools/hexToDecimalConverter.js");
var should = require('should');

describe('hexToDecimalConverter', function() {
  describe('convert()', function() {
    it('should return decimal output of a hexadecimal input', function() {
      should(htdConverter.convert("FF")).equal("255");
    });
  });
});


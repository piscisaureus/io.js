// Patch classes for chakra
var extend = function(dest, from) {
  var props = Object.getOwnPropertyNames(from);
  props.forEach(function(name) {
    var destination = Object.getOwnPropertyDescriptor(from, name);
    try {
      Object.defineProperty(dest, name, destination);
    }
    catch (e) {
      // ignore errors
    }
  });
};

function patchErrorStack() {
  var util = require('util');
  // the 2nd argument is ignored for now
  Error.__proto__.captureStackTrace = function(err, func) {
    var currentStack;
    try { throw new Error; } catch (e) { currentStack = e.stack; }
    // remove the first line so this function won't be seen
    var splittedStack = currentStack.split('\n');
    var firstLine = splittedStack[0];
    splittedStack.splice(0, 3);

    if (err.name) {
      firstLine = err.name + ': ' + err.message;
    }

    err.stack = firstLine + '\n' + splittedStack.join('\n');
  }

  var err = new Error;
  err.name = 'Trace';
  err.message = util.format.apply(this, arguments);
  Error.captureStackTrace(err, arguments.callee);
}

function patchTypedArrays() {
  ArrayBuffer.prototype.slice = function(begin, end) {
    if (!begin) {
      throw new Error('Wrong number of arguments.');
    }

    // boundary/arguments check - we do exactly the same ones as in node's
    // ArrayBuffer

    if (!end) {
      end = this.byteLength;
    }

    if (begin < 0) {
      begin = this.byteLength + begin;
    }

    if (begin < 0) {
      begin = 0;
    }

    if (begin > this.byteLength) {
      begin = this.byteLength;
    }

    if (end < 0) {
      end = this.byteLength + end;
    }

    if (end < 0) {
      end = 0;
    }

    if (end > this.byteLength) {
      end = this.byteLength;
    }

    if (begin > end) {
      begin = end;
    }

    var slice_length = end - begin;
    var slicedBuffer = new ArrayBuffer(slice_length);

    var sourceDataView = new DataView(this);
    var destDataView = new DataView(slicedBuffer);

    for (var i = 0; i < slice_length; i++)
    {
      destDataView.setInt8(i, sourceDataView.getInt8(begin + i));
    }

    return slicedBuffer;
  };

  // patch slice method for each type:
  [Int8Array,
   Uint8Array,
   Uint8ClampedArray,
   Int16Array,
   Uint16Array,
   Int32Array,
   Uint32Array,
   Float32Array,
   Float64Array].forEach(function(item) {
    item.prototype.slice = item.prototype.subarray;
    item.prototype.get = function(i) { return this[i]; };
    item.prototype.set = function(first, second) {
      if (first instanceof Array) {
        // in this case - first is an array of values, second is the offset
        for (var j = 0; j < first.length; j++) {
          this[j + second] = first[j];
        }
      }
      else {
        this[first] = second;
      }
    }
  });
}

function patchJsonParse() {
  var JsonParseOriginal = JSON.parse;

  var JsonParse = function() {
    return JsonParseOriginal.apply(this, arguments);
  }

  JSON.parse = JsonParse;
}

if (!global.__chakra_patched) {
  // patch console
  patchErrorStack();
  patchTypedArrays();
  patchJsonParse();
  Object.defineProperty(
      global,
      '__chakra_patched',
      { enumerable: false, value: true });
}

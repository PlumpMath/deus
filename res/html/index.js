function load(js, data, wasm, size) {
  var s = document.getElementById("status");
  var p = document.getElementById("progress");

  function status(message) {
    if (0 < message.length) {
      s.innerHTML = message;
      s.className = "";
    }
  }

  function progress(value, error) {
    if (error) {
      p.value = 100;
      p.className = "error";
    } else {
      if (!value) {
        p.value = 0;
        p.className = "";
      } else {
        p.value = Math.max(p.value, value / size * 100);
      }
    }
  }

  function error(what, info) {
    what = "string" === typeof what ? what.trim() : String(what).trim();
    if (!what.match(/^(trying binaryen method|binaryen method succeeded)/)) {
      console.warn(what, info);
      info = "string" === typeof info ? info.trim() : "";
      if (!info.length) {
        var x = what.indexOf(":");
        if (0 < x) {
          what = what.substr(0, x);
          info = what.substr(x + 1);
        }
        info = what.indexOf(".");
        if (0 < x) {
          what = what.substr(0, x + 1);
          info = what.substr(x + 1);
        }
      }
      s.innerHTML = '<p class="error">' + what + "</p>" + (0 < info.length ? "<p>" + info + "</p>" : "");
      progress(100, true);
    }
  }

  var url = null;
  var src = null;

  function main() {
    try {
      if (url && src && "function" === typeof deus) {
        status("running ...");
        deus({
          print: status,
          printErr: error,
          canvas: document.getElementById("view"),
          arguments: ["view"],
          wasmBinary: src,
          locateFile: function(name) {
            return "deus.data" === name ? url : name;
          },
          preRun: [function() {
            URL.revokeObjectURL(url);
            url = null;
          }]
        });
        src = null;
      }
    } catch (e) {
      var s = "string" === typeof e.message ? e.message : String(e);
      if (!s.match(/no binaryen method succeeded/)) {
        error("load wasm error", s);
      }
    }
  }

  var total = 0;
  function get(url, type, callback) {
    var loaded = 0;
    var xhr = new XMLHttpRequest();
    xhr.open("GET", url, true);
    xhr.responseType = type;
    xhr.onprogress = function (event) {
      if (event.loaded) {
        total += event.loaded - loaded;
        loaded = event.loaded;
        progress(total);
      }
    };
    xhr.onerror = function(event) {
      error("could not load file: " + url);
    };
    xhr.onload = function (event) {
      if (200 === xhr.status || 304 === xhr.status || 206 === xhr.status || !xhr.status && xhr.response) {
        if (event.total) {
          total += event.total - loaded;
          progress(total);
        }
        callback(xhr.response);
      } else {
        error("could not load file: " + url, xhr.statusText);
      }
    };
    xhr.send(null);
  }

  progress(0);
  status("loading ...");

  get(js, "text", function(text) {
    var script = document.createElement("script");
    script.innerHTML = text;
    document.body.appendChild(script);
    main();
  });

  get(data, "blob", function(blob) {
    url = URL.createObjectURL(blob);
    main();
  });

  get(wasm, "arraybuffer", function(arraybuffer) {
    src = arraybuffer;
    main();
  });

  var canvas = document.createElement("canvas");
  canvas.id = "view";
  canvas.width = window.innerWidth;
  canvas.height = window.innerHeight;
  document.body.appendChild(canvas);
}
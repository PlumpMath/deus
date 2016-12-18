mergeInto(LibraryManager.library, {
  write_message: function(m, e) {
    (e ? Module.printErr : Module.print)(Pointer_stringify(m));
  },
  show_canvas: function() {
    Module.print = function(s) { s.length > 0 && console.log(s); };
    Module.printErr = function(s) { s.length > 0 && console.warn(s); };
    var s = document.getElementById("status");
    s.className = "";
    var p = document.getElementById("progress");
    p.className = "";
    p.value = 100;
    var i = document.getElementById("info");
    i.style.pointerEvents = "none";
    i.style.transition = "opacity 400ms ease-out";
    i.style.opacity = 0.0;
    setTimeout(function() {
      i.parentNode.removeChild(i);
    }, 500);
  },
});